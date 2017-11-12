// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "Parser.h"

#include <iostream>
#include <exception>
#include <sstream>

#define CURRENT_DATA() m_comm.Current().FetchData()
#define PREVIOUS_TOKEN() m_comm.Previous().FetchToken()
#define CURRENT_TOKEN() m_comm.Current().FetchToken()
#define MATCH_TOKEN(t) (CURRENT_TOKEN() == t)
#define NOT_TOKEN(t) (CURRENT_TOKEN() != t)

#define AST_ROOT() m_ast

//TODO: remove EMIT helpers
#define EMIT_IDENTIFIER() std::cout << "EMIT::IDENTIFIER" << "("<< CURRENT_DATA()->As<std::string>() << ")" << std::endl;

#define MAKE_RESV_REF() std::make_shared<DeclRefExpr>(m_identifierStack.top()); m_identifierStack.pop();
#define MAKE_BUILTIN_FUNC(n) std::make_shared<BuiltinExpr>(std::make_shared<DeclRefExpr>(n));

class UnexpectedTokenException : public std::exception
{
public:
	UnexpectedTokenException() noexcept
	{
	}

	explicit UnexpectedTokenException(char const* const message, int line, int column, Token token) noexcept
		: m_line{ line }
		, m_column{ column }
	{
		std::stringstream ss;
		ss << "Semantic error: " << message;
		ss << " before " << "'" + Keyword{ token }.Print() + "'" << " token at ";
		ss << line << ':' << column;
		_msg = ss.str();
	}

	virtual int Line() const noexcept
	{
		return m_line;
	}

	virtual int Column() const noexcept
	{
		return m_column;
	}

	virtual const char *what() const noexcept
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;

private:
	int m_line;
	int m_column;
};

class SyntaxException : public std::exception
{
public:
	SyntaxException() noexcept
	{
	}

	explicit SyntaxException(char const* const message, char token, int line, int column) noexcept
		: m_token{ token }
		, m_line{ line }
		, m_column{ column }
	{
		std::stringstream ss;
		ss << "Syntax error: " << message << " at ";
		ss << line << ':' << column;
		_msg = ss.str();
	}

	virtual int Line() const noexcept
	{
		return m_line;
	}

	virtual int Column() const noexcept
	{
		return m_column;
	}

	virtual const char *what() const noexcept
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;

private:
	char m_token;
	int m_line;
	int m_column;
};

class ParseException : public std::exception
{
public:
	ParseException() noexcept = default;

	explicit ParseException(char const* const message, int line, int column) noexcept
		: m_line{ line }
		, m_column{ column }
	{
		std::stringstream ss;
		ss << "Parser error: " << message;
		ss << " " << line << ':' << column;
		_msg = ss.str();
	}

	virtual int Line() const noexcept
	{
		return m_line;
	}

	virtual int Column() const noexcept
	{
		return m_column;
	}

	virtual const char *what() const noexcept
	{
		return _msg.c_str();
	}

protected:
	std::string _msg;

private:
	int m_line;
	int m_column;
};

Parser::Parser(std::shared_ptr<Compiler::Profile>& profile)
	: Stage{ this }
	, m_profile{ profile }
	, lex{ profile }
{
	lex.ErrorHandler([](const std::string& err, char token, int line, int column)
	{
		throw SyntaxException(err.c_str(), token, line, column);
	});
}

Parser& Parser::CheckCompatibility()
{
	//TODO: check profile options here
	return (*this);
}

void Parser::Error(const char* err, Token token)
{
	throw UnexpectedTokenException{ err, m_comm.Current().FetchLine(), m_comm.Current().FetchColumn(), token };
}

// Expect next token to be provided token, if so
// move lexer forward. If not report error and bail.
void Parser::ExpectToken(Token token)
{
	if (NOT_TOKEN(token)) {
		Error("expected expression", token);
	}

	NextToken();
}

//TODO: remove ?
void Parser::ExpectIdentifier()
{
	if (NOT_TOKEN(TK_IDENTIFIER)) {
		Error("expected identifier", TK_IDENTIFIER);
	}

	assert(m_comm.Current().HasData());

	NextToken();
}

// Storage class specifiers determine the lifetime and scope of the object
auto Parser::StorageClassSpecifier()
{
	using namespace Typedef;

	switch (CURRENT_TOKEN()) {
	case TK_EXTERN:
		return TypedefBase::StorageClassSpecifier::EXTERN;
	case TK_REGISTER:
		return TypedefBase::StorageClassSpecifier::REGISTER;
	case TK_STATIC:
		return TypedefBase::StorageClassSpecifier::STATIC;
	case TK_AUTO:
		return TypedefBase::StorageClassSpecifier::AUTO;
	case TK_TYPEDEF:
		return TypedefBase::StorageClassSpecifier::TYPEDEF;
	}

	return TypedefBase::StorageClassSpecifier::NONE;
}

// Type specifiers determine size of the object in memory
bool Parser::TypeSpecifier()
{
	using namespace Typedef;

	switch (CURRENT_TOKEN()) {
	case TK_VOID:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::VOID));
		return true;
	case TK_CHAR:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::CHAR));
		return true;
	case TK_SHORT:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::SHORT));
		return true;
	case TK_INT:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::INT));
		return true;
	case TK_LONG:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::LONG));
		return true;
	case TK_FLOAT:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::FLOAT));
		return true;
	case TK_DOUBLE:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::DOUBLE));
		return true;
	case TK_SIGNED:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::INT));
		return true;
	case TK_UNSIGNED:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::INT));
		return true;
	case TK_BOOL:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::BOOL));
		return true;
	case TK_COMPLEX:
		return true;
	case TK_IMAGINARY:
		return true;
	}

	// Check for enum declarations
	if (EnumSpecifier()) {
		return true;
	}

	// Check for struct or union declarations
	if (StructOrUnionSpecifier()) {
		return true;
	}

	// Check if identifier is registered as typename
	if (TypenameSpecifier()) {
		return true;
	}

	return false;
}

// Specifying type correctness
auto Parser::TypeQualifier()
{
	using namespace Typedef;

	switch (CURRENT_TOKEN()) {
	case TK_CONST:
		return TypedefBase::TypeQualifier::CONST;
	case TK_VOLATILE:
		return TypedefBase::TypeQualifier::VOLATILE;
	}

	return TypedefBase::TypeQualifier::NONE;
}

template<typename _Ty, typename _Decay = int>
auto IsSet(_Ty v) -> bool
{
	return static_cast<bool>(static_cast<_Decay>(v));
}

bool Parser::DeclarationSpecifiers()
{
	using namespace Typedef;

	TypedefBase::StorageClassSpecifier tmpSCP = TypedefBase::StorageClassSpecifier::NONE;
	std::vector<TypedefBase::TypeQualifier> tmpTQ;
	auto isInline = false;

	bool cont = true;
	while (cont) {
		cont = false;

		// Find a type specifier
		if (TypeSpecifier()) {
			NextToken();
			cont = true;
		}

		// Only one specifier can be applied per object type
		auto sc = StorageClassSpecifier();
		if (IsSet(sc)) {
			tmpSCP = sc;
			NextToken();
			cont = true;
		}

		// Can have multiple type qualifiers, list them
		auto tq = TypeQualifier();
		if (IsSet(tq)) {
			tmpTQ.push_back(tq);
			NextToken();
			cont = true;
		}

		// Function inline specifier
		if (MATCH_TOKEN(TK_INLINE)) {
			NextToken();
			isInline = true;
			cont = true;
		}
	}

	// No type was found
	if (m_typeStack.empty()) {
		return false;
	}

	// Append all stacked storage classes and qualifiers onto the value object
	auto baseType = m_typeStack.top();
	if (tmpSCP != TypedefBase::StorageClassSpecifier::NONE) {
		baseType->StorageClass(tmpSCP);
	}
	for (const auto& tq : tmpTQ) {
		baseType->Qualifier(tq);
	}

	//TODO: function inliner
	/*if (isInline) {
		baseType->SetInline();
	}*/

	return true;
}

//TODO
bool Parser::TypenameSpecifier()
{
	return false;
}

bool Parser::StructOrUnionSpecifier()
{
	auto isUnion = false;

	switch (CURRENT_TOKEN()) {
	case TK_STRUCT:
		NextToken();
		break;
	case TK_UNION:
		NextToken();
		isUnion = true;
		break;
	default:
		return false;
	}

	auto rec = std::make_shared<RecordDecl>(isUnion ? RecordDecl::RecordType::UNION : RecordDecl::RecordType::STRUCT);

	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		EMIT_IDENTIFIER();
		rec->SetName(CURRENT_DATA()->As<std::string>());
		NextToken();
	}

	if (MATCH_TOKEN(TK_BRACE_OPEN)) {
		NextToken();

		do {
			SpecifierQualifierList();

			for (;;) {
				Declarator();

				auto decl = m_identifierStack.top();
				m_identifierStack.pop();
				auto field = std::make_shared<FieldDecl>(decl, m_typeStack.top());

				if (MATCH_TOKEN(TK_COLON)) {
					NextToken();
					ConstantExpression();
					field->SetBitField(std::dynamic_pointer_cast<IntegerLiteral>(m_elementDescentPipe.next()));
					m_elementDescentPipe.pop();
				}

				m_typeStack.pop();

				rec->AddField(field);

				if (NOT_TOKEN(TK_COMMA)) {
					break;
				}

				NextToken();
			}

			ExpectToken(TK_COMMIT);
		} while (NOT_TOKEN(TK_BRACE_CLOSE));
		ExpectToken(TK_BRACE_CLOSE);
	}

	m_elementDescentPipe.push(rec);
	m_elementDescentPipe.lock();
	return true;
}

void Parser::SpecifierQualifierList()
{
	bool cont = false;
	do {
		cont = false;
		if (TypeSpecifier()) {
			NextToken();
			cont = true;
		}

		auto tq = TypeQualifier();
		if (tq != Typedef::TypedefBase::TypeQualifier::NONE) {
			cont = true;
		}
	} while (cont);
}

bool Parser::EnumSpecifier()
{
	if (MATCH_TOKEN(TK_ENUM)) {
		NextToken();

		auto enm = std::make_shared<EnumDecl>();

		if (MATCH_TOKEN(TK_IDENTIFIER)) {
			EMIT_IDENTIFIER();
			enm->SetName(CURRENT_DATA()->As<std::string>());
			NextToken();
		}

		if (MATCH_TOKEN(TK_BRACE_OPEN)) {
			for (;;) {
				NextToken();

				if (MATCH_TOKEN(TK_IDENTIFIER)) {
					EMIT_IDENTIFIER();

					auto enmConst = std::make_shared<EnumConstantDecl>(CURRENT_DATA()->As<std::string>());

					NextToken();
					if (MATCH_TOKEN(TK_ASSIGN)) {
						NextToken();
						ConstantExpression();
						enmConst->SetAssignment(m_elementDescentPipe.next());
						m_elementDescentPipe.pop();
					}

					enm->AddConstant(enmConst);
				}

				if (NOT_TOKEN(TK_COMMA)) {
					break;
				}
			}
			ExpectToken(TK_BRACE_CLOSE);
		}

		m_elementDescentPipe.push(enm);
		m_elementDescentPipe.lock();
		return true;
	}

	return false;
}

bool Parser::UnaryOperator()
{
	switch (CURRENT_TOKEN()) {
	case TK_AMPERSAND:
	{
		NextToken();
		CastExpression();

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::ADDR, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_ASTERISK:
	{
		NextToken();
		CastExpression();

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::PTRVAL, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_PLUS:
	{
		NextToken();
		CastExpression();

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INTPOS, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_MINUS:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INTNEG, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_TILDE:
	{
		NextToken();
		CastExpression();

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::BITNOT, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_NOT:
	{
		NextToken();
		CastExpression();

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::BOOLNOT, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	default:
		return false;
	}

	return true;
}

bool Parser::AssignmentOperator()
{
	switch (CURRENT_TOKEN()) {
	case TK_ASSIGN:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::ASSGN, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_MUL_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MUL, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_DIV_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::DIV, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_MOD_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MOD, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_ADD_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::ADD, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_SUB_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::SUB, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_LEFT_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::LEFT, resv);
		m_elementDescentPipe.pop();

		m_elementDescentPipe.pop();
		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_RIGHT_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::RIGHT, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_AND_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::AND, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_XOR_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::XOR, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_OR_ASSIGN:
	{
		auto resv = std::dynamic_pointer_cast<DeclRefExpr>(m_elementDescentPipe.next());
		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::OR, resv);
		m_elementDescentPipe.pop();

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	default:
		return false;
	}

	return true;
}

void Parser::PrimaryExpression()
{
	using Typedef::BuiltinType;

	switch (CURRENT_TOKEN()) {
	case TK_IDENTIFIER:
		EMIT_IDENTIFIER();
		m_identifierStack.push(CURRENT_DATA()->As<std::string>());
		NextToken();
		break;

	case TK_CONSTANT:
		switch (CURRENT_DATA()->DataType<BuiltinType>()->TypeSpecifier()) {
		case BuiltinType::Specifier::INT:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<int>>(CURRENT_DATA());
			m_elementDescentPipe.push(std::make_shared<IntegerLiteral>(std::move(object)));
			break;
		}
		case BuiltinType::Specifier::DOUBLE:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<double>>(CURRENT_DATA());
			m_elementDescentPipe.push(std::make_shared<FloatingLiteral>(std::move(object)));
			break;
		}
		case BuiltinType::Specifier::CHAR:
		{
			if (CURRENT_DATA()->IsArray()) {
				auto object = std::dynamic_pointer_cast<ValueObject<std::string>>(CURRENT_DATA());
				m_elementDescentPipe.push(std::make_shared<StringLiteral>(std::move(object)));
			}
			else {
				auto object = std::dynamic_pointer_cast<ValueObject<char>>(CURRENT_DATA());
				m_elementDescentPipe.push(std::make_shared<CharacterLiteral>(std::move(object)));
			}
			break;
		}
		}
		NextToken();
		break;

	case TK_PARENTHESE_OPEN:
		NextToken();
		Expression();

		auto parenthesis = std::make_shared<ParenExpr>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(parenthesis);
		ExpectToken(TK_PARENTHESE_CLOSE);
	}
}

void Parser::ArgumentExpressionList()
{
	for (;;) {
		auto itemState = m_elementDescentPipe.state();
		AssignmentExpression();
		if (m_elementDescentPipe.is_changed(itemState)) {
			m_elementDescentPipe.lock();
		}

		if (NOT_TOKEN(TK_COMMA)) {
			break;
		}

		NextToken();
	}
}

void Parser::PostfixExpression()
{
	if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
		// Snapshot current state in case of rollback
		m_comm.Snapshot();
		try {
			NextToken();
			TypeName();
			ExpectToken(TK_PARENTHESE_CLOSE);
			ExpectToken(TK_BRACE_OPEN);

			// Remove snapshot since we can continue this path
			m_comm.DisposeSnapshot();

			auto list = std::make_shared<InitListExpr>();

			for (;;) {
				Initializer();

				list->AddListItem(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();

				if (NOT_TOKEN(TK_COMMA)) {
					break;
				}

				NextToken();
			}
			ExpectToken(TK_BRACE_CLOSE);

			m_elementDescentPipe.push(std::make_shared<CompoundLiteralExpr>(list));
		}

		// Cannot cast, rollback the command state
		catch (const UnexpectedTokenException&) {
			m_comm.Revert();
		}
	}

	auto startSz = m_identifierStack.size();

	PrimaryExpression();

	switch (CURRENT_TOKEN()) {
	case TK_BRACKET_OPEN:
	{
		NextToken();

		auto resv = MAKE_RESV_REF();

		Expression();
		ExpectToken(TK_BRACKET_CLOSE);

		auto arrsub = std::make_shared<ArraySubscriptExpr>(resv, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(arrsub);
		break;
	}
	case TK_PARENTHESE_OPEN:
	{
		NextToken();

		auto resv = MAKE_RESV_REF();

		if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
			NextToken();
			m_elementDescentPipe.push(std::make_shared<CallExpr>(resv));
		}
		else {
			auto startState = m_elementDescentPipe.state();
			ArgumentExpressionList();
			m_elementDescentPipe.release_until(startState);
			ExpectToken(TK_PARENTHESE_CLOSE);

			auto arg = std::make_shared<ArgumentStmt>();
			while (!m_elementDescentPipe.empty()) {
				arg->AppendArgument(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();
			}

			m_elementDescentPipe.push(std::make_shared<CallExpr>(resv, arg));
		}
		break;
	}
	case TK_DOT:
	{
		auto resv = MAKE_RESV_REF();

		NextToken();
		EMIT_IDENTIFIER();
		//ExpectIdentifier();

		auto member = CURRENT_DATA()->As<std::string>();
		m_elementDescentPipe.push(std::make_shared<MemberExpr>(MemberExpr::MemberType::REFERENCE, member, resv));
		NextToken();
		break;
	}
	case TK_PTR_OP:
	{
		auto resv = MAKE_RESV_REF();

		NextToken();
		EMIT_IDENTIFIER();
		//ExpectIdentifier();

		auto member = CURRENT_DATA()->As<std::string>();
		m_elementDescentPipe.push(std::make_shared<MemberExpr>(MemberExpr::MemberType::POINTER, member, resv));
		NextToken();
		break;
	}
	case TK_INC_OP:
	{
		if (m_identifierStack.size() == startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);

		NextToken();
		break;
	}
	case TK_DEC_OP:
	{
		if (m_identifierStack.size() == startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}

		auto resv = MAKE_RESV_REF();
		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::DEC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		m_elementDescentPipe.push(unaryOp);

		NextToken();
		break;
	}
	default:
		if (m_identifierStack.size() > startSz) {
			auto resv = MAKE_RESV_REF();
			m_elementDescentPipe.push(resv);
		}
	}
}

void Parser::UnaryExpression()
{
	auto startSz = m_identifierStack.size();

	switch (CURRENT_TOKEN()) {
	case TK_INC_OP:
	{
		NextToken();
		UnaryExpression();

		/*if (m_identifierStack.size() != startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}*/

		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INC, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(unaryOp);

		break;
	}
	case TK_DEC_OP:
	{
		NextToken();
		UnaryExpression();

		/*if (m_identifierStack.size() != startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}*/

		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::DEC, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(unaryOp);

		break;
	}
	case TK_SIZEOF:
	{
		NextToken();
		auto func = MAKE_BUILTIN_FUNC("sizeof");

		// Snapshot current state in case of rollback
		m_comm.Snapshot();
		try {
			if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
				NextToken();
				TypeName();
				ExpectToken(TK_PARENTHESE_CLOSE);

				// Remove snapshot since we can continue this path
				m_comm.DisposeSnapshot();
				m_elementDescentPipe.push(func);
				break;
			}
		}
		// No typename, rollback the command state
		catch (const UnexpectedTokenException&) {
			m_comm.Revert();
		}

		UnaryExpression();

		func->SetExpression(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(func);
		break;
	}
	default:
		if (!UnaryOperator()) {
			PostfixExpression();
		}
	}
}

void Parser::CastExpression()
{
	if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
		// Snapshot current state in case of rollback
		m_comm.Snapshot();
		try {
			NextToken();
			TypeName();
			ExpectToken(TK_PARENTHESE_CLOSE);

			CastExpression();

			// If there are no element on the queue, no cast was found
			if (m_elementDescentPipe.empty()) {
				throw UnexpectedTokenException{};
			}

			// Remove snapshot since we can continue this path
			m_comm.DisposeSnapshot();

			auto cast = std::make_shared<CastExpr>(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
			m_elementDescentPipe.push(cast);
			return;
		}
		// Cannot cast, rollback the command state
		catch (const UnexpectedTokenException&) {
			m_comm.Revert();
		}
	}

	UnaryExpression();
}

void Parser::MultiplicativeExpression()
{
	CastExpression();

	switch (CURRENT_TOKEN()) {
	case TK_ASTERISK:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::MUL, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		CastExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_SLASH:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::DIV, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		CastExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_PERCENT:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::MOD, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		CastExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	}
}

void Parser::AdditiveExpression()
{
	MultiplicativeExpression();

	switch (CURRENT_TOKEN()) {
	case TK_PLUS:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::PLUS, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		MultiplicativeExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_MINUS:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::MINUS, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		MultiplicativeExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	}
}

void Parser::ShiftExpression()
{
	AdditiveExpression();

	switch (CURRENT_TOKEN()) {
	case TK_LEFT_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::SLEFT, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		AdditiveExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_RIGHT_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::SRIGHT, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		AdditiveExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	}
}

void Parser::RelationalExpression()
{
	ShiftExpression();

	switch (CURRENT_TOKEN()) {
	case TK_LESS_THAN:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::LT, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		ShiftExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_GREATER_THAN:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::GT, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		ShiftExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_LE_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::LE, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		ShiftExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_GE_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::GE, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		ShiftExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	}
}

void Parser::EqualityExpression()
{
	RelationalExpression();

	switch (CURRENT_TOKEN()) {
	case TK_EQ_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::EQ, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		RelationalExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	case TK_NE_OP:
	{
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::NEQ, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		RelationalExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
		break;
	}
	}
}

void Parser::AndExpression()
{
	EqualityExpression();

	if (MATCH_TOKEN(TK_AMPERSAND)) {
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::AND, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		EqualityExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
	}
}

void Parser::ExclusiveOrExpression()
{
	AndExpression();

	if (MATCH_TOKEN(TK_CARET)) {
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::XOR, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		AndExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
	}
}

void Parser::LogicalAndExpression()
{
	ExclusiveOrExpression();

	if (MATCH_TOKEN(TK_AND_OP)) {
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::LAND, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		ExclusiveOrExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
	}
}

void Parser::LogicalOrExpression()
{
	LogicalAndExpression();

	if (MATCH_TOKEN(TK_OR_OP)) {
		auto binOp = std::make_shared<BinaryOperator>(BinaryOperator::BinOperand::LOR, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		LogicalAndExpression();

		binOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(binOp);
	}
}

void Parser::ConditionalExpression()
{
	LogicalOrExpression();

	if (MATCH_TOKEN(TK_QUESTION_MARK)) {
		auto conOp = std::make_shared<ConditionalOperator>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		NextToken();
		Expression();
		conOp->SetTruthCompound(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		ExpectToken(TK_COLON);
		ConditionalExpression();

		conOp->SetAltCompound(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(conOp);
	}
}

void Parser::AssignmentExpression()
{
	ConditionalExpression();

	//UnaryExpression(); //XXX UnaryExpression is already run in the ConditionalExpression
	AssignmentOperator();
}

void Parser::Expression()
{
	for (;;) {
		AssignmentExpression();

		if (NOT_TOKEN(TK_COMMA)) {
			break;
		}

		NextToken();
	}
}

void Parser::ConstantExpression()
{
	ConditionalExpression();
}

bool Parser::JumpStatement()
{
	switch (CURRENT_TOKEN()) {
	case TK_GOTO:
		NextToken();

		//ExpectIdentifier();//XXX: possible optimization

		m_elementDescentPipe.push(std::make_shared<GotoStmt>(CURRENT_DATA()->As<std::string>()));
		NextToken();
		break;
	case TK_CONTINUE:
		NextToken();
		m_elementDescentPipe.push(std::make_shared<ContinueStmt>());
		break;
	case TK_BREAK:
		NextToken();
		m_elementDescentPipe.push(std::make_shared<BreakStmt>());
		break;
	case TK_RETURN:
		NextToken();
		if (MATCH_TOKEN(TK_COMMIT)) {
			m_elementDescentPipe.push(std::make_shared<ReturnStmt>());
		}
		else {
			Expression();

			auto returnStmt = std::make_shared<ReturnStmt>();
			while (!m_elementDescentPipe.empty()) {
				returnStmt->SetReturnNode(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();
			}

			m_elementDescentPipe.push(returnStmt);
		}
		break;
	default:
		return false;
	}

	ExpectToken(TK_COMMIT);
	return true;
}

// For, do and while loop
bool Parser::IterationStatement()
{
	switch (CURRENT_TOKEN()) {
	case TK_WHILE:
	{
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		auto whlstmt = std::make_shared<WhileStmt>(m_elementDescentPipe.next());
		ExpectToken(TK_PARENTHESE_CLOSE);
		m_elementDescentPipe.pop();

		Statement();

		if (!m_elementDescentPipe.empty()) {
			whlstmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(whlstmt);
		return true;
	}
	case TK_DO:
	{
		NextToken();
		Statement();

		auto dostmt = std::make_shared<DoStmt>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		ExpectToken(TK_WHILE);
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);
		ExpectToken(TK_COMMIT);

		dostmt->SetEval(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(dostmt);
		return true;
	}
	case TK_FOR:
	{
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);

		std::shared_ptr<ASTNode> node1;
		std::shared_ptr<ASTNode> node2;
		std::shared_ptr<ASTNode> node3;

		Declaration();
		if (!m_elementDescentPipe.empty()) {
			node1 = m_elementDescentPipe.next();
			m_elementDescentPipe.pop();

			ExpressionStatement();
			if (!m_elementDescentPipe.empty()) {
				node2 = m_elementDescentPipe.next();
				m_elementDescentPipe.pop();
			}
		}
		else {
			ExpressionStatement();
			if (!m_elementDescentPipe.empty()) {
				node1 = m_elementDescentPipe.next();
				m_elementDescentPipe.pop();
			}

			ExpressionStatement();
			if (!m_elementDescentPipe.empty()) {
				node2 = m_elementDescentPipe.next();
				m_elementDescentPipe.pop();
			}
		}

		Expression();
		if (!m_elementDescentPipe.empty()) {
			node3 = m_elementDescentPipe.next();
			m_elementDescentPipe.pop();
		}

		ExpectToken(TK_PARENTHESE_CLOSE);

		auto forstmt = std::make_shared<ForStmt>(node1, node2, node3);

		Statement();

		if (!m_elementDescentPipe.empty()) {
			forstmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(forstmt);
		return true;
	}
	}

	return false;
}

// If and switch statements
bool Parser::SelectionStatement()
{
	switch (CURRENT_TOKEN()) {
	case TK_IF:
	{
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);

		auto ifStmt = std::make_shared<IfStmt>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		Statement();

		// If the statement yields a body, save it and try
		// with alternative compound
		if (m_elementDescentPipe.size() > 0) {
			ifStmt->SetTruthCompound(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();

			if (MATCH_TOKEN(TK_ELSE)) {
				NextToken();
				Statement();
			}
		}

		m_elementDescentPipe.push(ifStmt);
		return true;
	}
	case TK_SWITCH:
	{
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);

		auto swStmt = std::make_shared<SwitchStmt>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();

		Statement();

		// If the statement yields a body, save it
		if (m_elementDescentPipe.size() > 0) {
			swStmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(swStmt);
		return true;
	}
	}

	return false;
}

void Parser::ExpressionStatement()
{
	Expression();

	if (MATCH_TOKEN(TK_COMMIT)) {
		NextToken();
		return;
	}
}

// Compound statements contain code block
bool Parser::CompoundStatement()
{
	if (MATCH_TOKEN(TK_BRACE_OPEN)) {
		NextToken();
		if (MATCH_TOKEN(TK_BRACE_CLOSE)) {
			NextToken();
		}
		else {
			auto startState = m_elementDescentPipe.state();
			BlockItems();
			m_elementDescentPipe.release_until(startState);
			ExpectToken(TK_BRACE_CLOSE);
		}

		// Squash all stack elements in compound statment
		// body and push compound on the stack
		auto compound = std::make_shared<CompoundStmt>();
		while (!m_elementDescentPipe.empty()) {
			compound->AppendChild(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(compound);
		return true;
	}

	return false;
}

// Labeled statements
bool Parser::LabeledStatement()
{
	switch (CURRENT_TOKEN()) {
	case TK_IDENTIFIER:
	{
		// Snapshot current state in case of rollback
		m_comm.Snapshot();
		try {
			auto lblName = CURRENT_DATA()->As<std::string>();
			EMIT_IDENTIFIER();
			NextToken();
			ExpectToken(TK_COLON);

			// Remove snapshot since we can continue this path
			m_comm.DisposeSnapshot();
			Statement();

			if (m_elementDescentPipe.empty()) {
				throw ParseException{ "expected statement", 0, 0 };
			}

			auto lbl = std::make_shared<LabelStmt>(lblName, m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
			m_elementDescentPipe.push(lbl);
		}
		// Not a label, rollback the command state
		catch (const UnexpectedTokenException&) {
			m_comm.Revert();
		}
		break;
	}
	case TK_CASE:
	{
		NextToken();
		ConstantExpression();

		auto lblLiteral = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();

		ExpectToken(TK_COLON);

		Statement();

		if (m_elementDescentPipe.empty()) {
			throw ParseException{ "label at end of compound statement", 0, 0 };
		}

		auto cse = std::make_shared<CaseStmt>(lblLiteral, m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(cse);
		return true;
	}
	case TK_DEFAULT:
	{
		NextToken();
		ExpectToken(TK_COLON);

		Statement();

		if (m_elementDescentPipe.empty()) {
			throw ParseException{ "label at end of compound statement", 0, 0 };
		}

		auto dflt = std::make_shared<DefaultStmt>(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(dflt);
		return true;
	}
	}

	return false;
}

void Parser::Statement()
{
	if (CompoundStatement()) { return; }
	if (SelectionStatement()) { return; }
	if (IterationStatement()) { return; }
	if (JumpStatement()) { return; }
	if (LabeledStatement()) { return; }

	// Yield no result, try statement as expression
	ExpressionStatement();
}

void Parser::BlockItems()
{
	do {
		auto itemState = m_elementDescentPipe.state();

		Statement();
		if (MATCH_TOKEN(TK_BRACE_CLOSE)) {
			break;
		}

		if (m_elementDescentPipe.is_changed(itemState)) {
			m_elementDescentPipe.lock();
			itemState = m_elementDescentPipe.state();
		}

		Declaration();
		if (m_elementDescentPipe.is_changed(itemState)) {
			m_elementDescentPipe.lock();
		}
	} while (NOT_TOKEN(TK_BRACE_CLOSE));
}

void Parser::Declaration()
{
	if (!DeclarationSpecifiers()) {
		return;
	}

	if (MATCH_TOKEN(TK_COMMIT)) {
		NextToken();
	}
	else {
		auto initState = m_elementDescentPipe.state();
		InitDeclaratorList();
		if (m_elementDescentPipe.is_changed(initState)) {
			m_elementDescentPipe.release_until(initState);

			auto decl = std::make_shared<DeclStmt>();
			while (!m_elementDescentPipe.empty()) {
				decl->AddDeclaration(std::dynamic_pointer_cast<VarDecl>(m_elementDescentPipe.next()));
				m_elementDescentPipe.pop();
			}

			m_elementDescentPipe.push(decl);
		}

		if (MATCH_TOKEN(TK_COMMIT)) {
			NextToken();
		}

		m_typeStack.pop();
	}
}

void Parser::InitDeclaratorList()
{
	auto cont = false;
	do {
		cont = false;
		if (!Declarator()) {
			return;
		}

		if (MATCH_TOKEN(TK_ASSIGN)) {
			NextToken();

			auto startState = m_elementDescentPipe.state();
			Initializer();
			m_elementDescentPipe.release_until(startState);

			auto var = std::make_shared<VarDecl>(m_identifierStack.top(), m_typeStack.top(), m_elementDescentPipe.next());
			m_identifierStack.pop();
			m_elementDescentPipe.pop();

			assert(var != nullptr);

			m_elementDescentPipe.push(var);
			m_elementDescentPipe.lock();
		}
		else {
			auto var = std::make_shared<VarDecl>(m_identifierStack.top(), m_typeStack.top());
			m_identifierStack.pop();
			m_elementDescentPipe.push(var);
			m_elementDescentPipe.lock();

			if (MATCH_TOKEN(TK_COMMA)) {
				cont = true;
				NextToken();
			}
		}
	} while (cont);
}

// Typenames are primitive types and used defined structures
void Parser::TypeName()
{
	SpecifierQualifierList();
	AbstractDeclarator();
}

// An abstract declarator is a declarator without an identifier,
// consisting of one or more pointer, array, or function modifiers.
// Abstract declarator are used by typedefs and parameter lists
void Parser::AbstractDeclarator()
{
	Pointer();
	DirectAbstractDeclarator();
}

void Parser::DirectAbstractDeclarator()
{
	bool cont = false;
	do {
		cont = false;
		switch (CURRENT_TOKEN()) {
		case TK_PARENTHESE_OPEN:
			NextToken();

			// Pointer to void
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();
				cont = true;
			}
			// Pointer to some declarator
			else {
				AbstractDeclarator();
				if (ParameterTypeList()) {
					cont = true;
				}
				ExpectToken(TK_PARENTHESE_CLOSE);
			}
			break;

		case TK_BRACKET_OPEN:
			NextToken();

			// Empty array declarator
			if (MATCH_TOKEN(TK_BRACKET_CLOSE)) {
				NextToken();
				cont = true;
			}
			// Array of pointers
			else if (MATCH_TOKEN(TK_ASTERISK)) {
				NextToken();
				ExpectToken(TK_BRACKET_CLOSE);
				cont = true;
			}
			// Array with expression initalizer
			else {
				AssignmentExpression();
				ExpectToken(TK_BRACKET_CLOSE);
				cont = true;
			}
			break;
		default:
			break;
		}
	} while (cont);
}

void Parser::Initializer()
{
	if (MATCH_TOKEN(TK_BRACE_OPEN)) {
		auto startState = m_elementDescentPipe.state();

		do {
			NextToken();

			// Snapshot current state in case of rollback
			m_comm.Snapshot();
			try {
				Designation();
				m_comm.DisposeSnapshot();
			}
			// Cannot cast, rollback the command state
			catch (const UnexpectedTokenException&) {
				m_comm.Revert();
			}

			Initializer();
		} while (MATCH_TOKEN(TK_COMMA));
		ExpectToken(TK_BRACE_CLOSE);

		m_elementDescentPipe.release_until(startState);

		auto list = std::make_shared<InitListExpr>();
		while (!m_elementDescentPipe.empty()) {
			list->AddListItem(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(list);
		m_elementDescentPipe.lock();
	}
	else {
		AssignmentExpression();
	}
}

void Parser::Designation()
{
	Designators();
	ExpectToken(TK_ASSIGN);
}

void Parser::Designators()
{
	for (;;) {
		switch (CURRENT_TOKEN()) {
		case TK_BRACKET_OPEN:
			NextToken();
			ConstantExpression();
			ExpectToken(TK_BRACKET_CLOSE);
			continue;
		case TK_DOT:
			NextToken();
			ExpectIdentifier();
			continue;
		default:
			return;
		}
	}
}

void Parser::Pointer()
{
	while (MATCH_TOKEN(TK_ASTERISK)) {
		NextToken();
		TypeQualifierList();
	}
}

bool Parser::Declarator()
{
	Pointer();
	return DirectDeclarator();
}

bool Parser::DirectDeclarator()
{
	auto foundDecl = false;

	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		auto name = CURRENT_DATA()->As<std::string>();
		m_identifierStack.push(name);
		EMIT_IDENTIFIER();
		foundDecl = true;
		NextToken();
	}
	else if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
		NextToken();
		Declarator();
		ExpectToken(TK_PARENTHESE_CLOSE);
	}
	else {
		return false;
	}

	// Declarations following an identifier
	for (;;) {
		switch (CURRENT_TOKEN()) {
		case TK_PARENTHESE_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();

				auto func = std::make_shared<FunctionDecl>(m_identifierStack.top(), m_typeStack.top());
				m_identifierStack.pop();
				m_typeStack.pop();

				m_elementDescentPipe.push(func);
				return true;
			}
			else {
				const auto IdentifierListDecl = [=]
				{
					auto param = std::make_shared<ParamStmt>();

					for (;;) {
						if (MATCH_TOKEN(TK_IDENTIFIER)) {
							EMIT_IDENTIFIER();

							auto paramDecl = std::make_shared<ParamDecl>(CURRENT_DATA()->As<std::string>());
							param->AppendParamter(std::dynamic_pointer_cast<ASTNode>(paramDecl));
							NextToken();
						}

						if (NOT_TOKEN(TK_COMMA)) {
							break;
						}

						NextToken();
					}

					m_elementDescentPipe.push(param);
				};

				// Try default parameter declarations first, if that fails
				// give the ol' K&R decls a go
				if (!ParameterTypeList()) {
					IdentifierListDecl();
				}

				auto func = std::make_shared<FunctionDecl>(m_identifierStack.top(), m_typeStack.top());
				func->SetParameterStatement(std::dynamic_pointer_cast<ParamStmt>(m_elementDescentPipe.next()));
				m_elementDescentPipe.pop();
				m_typeStack.pop();
				ExpectToken(TK_PARENTHESE_CLOSE);

				m_elementDescentPipe.push(func);
				return true;
			}
			break;

		case TK_BRACKET_OPEN:
			NextToken();
			switch (CURRENT_TOKEN()) {
			case TK_BRACKET_CLOSE:
				NextToken();
				return true;
			case TK_ASTERISK:
				NextToken();
				ExpectToken(TK_BRACKET_CLOSE);
				return true;
			case TK_STATIC:
				NextToken();
				TypeQualifierList();
				AssignmentExpression();
				ExpectToken(TK_BRACKET_CLOSE);
			default:
			{
				TypeQualifierList();

				if (MATCH_TOKEN(TK_ASTERISK)) {
					NextToken();
					return true;
				}

				if (MATCH_TOKEN(TK_STATIC)) {
					NextToken();
				}

				AssignmentExpression();

				while (!m_elementDescentPipe.empty()) {
					m_elementDescentPipe.pop();
				}

				ExpectToken(TK_BRACKET_CLOSE);
			}
			}
			break;

		default:
			goto break_loop;
		}
	}

break_loop:
	return foundDecl;
}

// Continue until a type qualifier was found
void Parser::TypeQualifierList()
{
	while (TypeQualifier() != Typedef::TypedefBase::TypeQualifier::NONE);
}

// A parameter type list must contain at least the
// parameter declaration with a type specifier
bool Parser::ParameterTypeList()
{
	bool rs = false;

	auto startState = m_elementDescentPipe.state();
	for (;;) {
		rs = ParameterDeclaration();
		if (NOT_TOKEN(TK_COMMA)) {
			break;
		}

		NextToken();
		if (MATCH_TOKEN(TK_ELLIPSIS)) {
			NextToken();
			rs = true;
			//TODO: Change function signature
		}

		if (NOT_TOKEN(TK_COMMA)) {
			break;
		}
	}

	m_elementDescentPipe.release_until(startState);

	if (rs) {
		auto param = std::make_shared<ParamStmt>();
		while (!m_elementDescentPipe.empty()) {
			param->AppendParamter(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(param);
	}

	return rs;
}

// Parameter declaration can have only a specifier,
// an specifier and declarator or specifier and abstract
// declarator
bool Parser::ParameterDeclaration()
{
	if (!DeclarationSpecifiers()) {
		return false;
	}

	if (Declarator()) {
		auto param = std::make_shared<ParamDecl>(m_identifierStack.top());
		m_identifierStack.pop();

		m_elementDescentPipe.push(param);
		m_elementDescentPipe.lock();
		return true;
	}

	AbstractDeclarator();
	return true;
}

bool Parser::FunctionDefinition()
{
	// Return type for function declaration
	DeclarationSpecifiers();

	auto startState = m_elementDescentPipe.state();

	// Must match at least one declarator to qualify as function declaration
	if (!Declarator()) {
		m_elementDescentPipe.release_until(startState);
		return false;
	}

	// No result so far, bail
	if (m_elementDescentPipe.empty()) {
		m_elementDescentPipe.release_until(startState);
		return false;
	}

	auto func = std::dynamic_pointer_cast<FunctionDecl>(m_elementDescentPipe.next());
	if (func == nullptr) {
		m_elementDescentPipe.release_until(startState);
		return false;
	}

	m_elementDescentPipe.lock();

	while (Declarator());
	auto res = CompoundStatement();
	if (res) {
		func->SetCompound(std::dynamic_pointer_cast<CompoundStmt>(m_elementDescentPipe.next()));
		m_elementDescentPipe.pop();
	}

	m_elementDescentPipe.release_until(startState);
	return true;
}

// Try as function; if that fails assume declaration
void Parser::ExternalDeclaration()
{
	// Snapshot current state in case of rollback
	m_comm.Snapshot();

	if (!FunctionDefinition()) {
		// Not a function, rollback the command state
		m_comm.Revert();

		// Clear all states since nothing should be kept moving forward
		m_elementDescentPipe.clear();
		ClearStack(m_typeStack);
		ClearStack(m_identifierStack);
		Declaration();
	}
	else {
		// Remove snapshot since we can continue this path
		m_comm.DisposeSnapshot();
	}

	// At top level, release all locks
	m_elementDescentPipe.release_until(LockPipe<decltype(m_elementDescentPipe)>::begin);
	assert(!m_elementDescentPipe.empty());
}

//XXX: For each translation unit run the parser loop
// For each transaltion unit create a new AST tree. As of this moment
// only a single translation unit is supported
void Parser::TranslationUnit()
{
	// Returns file name of current lexer input, this can also be an include
	AST_ROOT() = std::make_shared<TranslationUnitDecl>(m_profile->MetaInfo()->name);

	do {
		ExternalDeclaration();

		while (!m_elementDescentPipe.empty()) {
			AST_ROOT()->AppendChild(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		// There should be no elements left
		assert(m_elementDescentPipe.empty(true));

		// Clear all lists where possible before adding new items
		ClearStack(m_typeStack);
		ClearStack(m_identifierStack);
		m_comm.TryClear();
	} while (!lex.IsDone());
}

Parser& Parser::Execute()
{
	NextToken();
	TranslationUnit();

	return (*this);
}