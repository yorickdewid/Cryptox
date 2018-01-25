// Copyright (c) 2017 Quenza Inc. All rights reserved.
//
// This file is part of the Cryptox project.
//
// Use of this source code is governed by a private license
// that can be found in the LICENSE file. Content can not be 
// copied and/or distributed without the express of the author.

#include "AST.h"
#include "Parser.h"

#include <iostream>
#include <exception>
#include <sstream>

#define CURRENT_DATA() m_comm.Current().FetchData()
#define PREVIOUS_TOKEN() m_comm.Previous().FetchToken()
#define CURRENT_TOKEN() m_comm.Current().FetchToken()
#define CURRENT_LOCATION() m_comm.Current().FetchLocation()
#define MATCH_TOKEN(t) (CURRENT_TOKEN() == t)
#define NOT_TOKEN(t) (CURRENT_TOKEN() != t)

#define AST_ROOT() m_ast

#define MAKE_RESV_REF() CoilCl::AST::MakeASTNode<DeclRefExpr>(m_identifierStack.top()); m_identifierStack.pop();

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

Parser::Parser(std::shared_ptr<CoilCl::Profile>& profile)
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
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::SIGNED));
		return true;
	case TK_UNSIGNED:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::UNSIGNED));
		return true;
	case TK_BOOL:
		m_typeStack.push(Util::MakeBuiltinType(BuiltinType::Specifier::BOOL));
		return true;
	case TK_COMPLEX:
		throw StageBase::StageException{ Name(), "complex types are not supported" };
	case TK_IMAGINARY:
		throw StageBase::StageException{ Name(), "imaginary types are not supported" };
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
	auto typeCount = m_typeStack.size();
	auto isInline = false;

	bool cont = true;
	while (cont) {
		cont = false;

		// Find a type specifier, per preference only once. Certain types allow for
		// multiple specifiers to combine into a single type base, if we found such a
		// type, we permit another run.
		if ((typeCount == m_typeStack.size() || m_typeStack.top()->AllowCoalescence())
			&& TypeSpecifier()) {
			NextToken();
			cont = true;
		}

		// Only one specifier can be applied per object type
		auto sc = StorageClassSpecifier();
		if (IsSet(sc)) {
			NextToken();
			tmpSCP = sc;
			cont = true;
		}

		// Can have multiple type qualifiers, list them
		auto tq = TypeQualifier();
		if (IsSet(tq)) {
			NextToken();
			tmpTQ.push_back(tq);
			cont = true;
		}

		// Function inline specifier
		if (MATCH_TOKEN(TK_INLINE)) {
			NextToken();
			isInline = true;
			cont = true;
		}
	}

	// No type was found, bail
	if (typeCount == m_typeStack.size()) {
		return false;
	}

	// Result yields multiple types, try type coalescence
	auto deltaTypeCount = m_typeStack.size() - typeCount;
	if (deltaTypeCount > 1) {
		auto lastType = m_typeStack.top();
		m_typeStack.pop();
		for (size_t i = 0; i < (deltaTypeCount - 1); ++i) {
			lastType->Consolidate(m_typeStack.top());
			m_typeStack.pop();
		}

		m_typeStack.push(std::move(lastType));
	}

	auto& baseType = m_typeStack.top();
	if (tmpSCP != TypedefBase::StorageClassSpecifier::NONE) {
		baseType->SetStorageClass(tmpSCP);
	}
	
	// Append all stacked qualifiers onto the value object
	for (const auto& tq : tmpTQ) {
		baseType->SetQualifier(tq);
	}

	// Function inlining
	if (isInline) {
		baseType->SetInline();
	}

	return true;
}

bool Parser::TypenameSpecifier()
{
	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		const auto& name = CURRENT_DATA()->As<std::string>();

		auto& res = m_typedefList[name];
		if (res == nullptr) {
			return false;
		}

		m_typeStack.push(Util::MakeTypedefType(name, res));
		return true;
	}

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

	std::string name;
	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		name = CURRENT_DATA()->As<std::string>();
		NextToken();
	}

	if (MATCH_TOKEN(TK_BRACE_OPEN)) {
		using RecType = RecordDecl::RecordType;

		NextToken();
		auto rec = CoilCl::AST::MakeASTNode<RecordDecl>(isUnion ? RecType::UNION : RecType::STRUCT);
		rec->SetLocation(CURRENT_LOCATION());

		if (!name.empty()) {
			rec->SetName(name);
			m_recordList[std::make_pair(name, !static_cast<int>(isUnion))] = rec;
		}

		do {
			SpecifierQualifierList();

			for (;;) {
				Declarator();

				auto& decl = m_identifierStack.top();
				m_identifierStack.pop();
				auto field = CoilCl::AST::MakeASTNode<FieldDecl>(decl, m_typeStack.top());
				field->SetLocation(CURRENT_LOCATION());
				field->ReturnType().SetPointer(m_pointerCounter);
				m_pointerCounter = 0;

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

		m_elementDescentPipe.push(rec);
		m_elementDescentPipe.lock();
	}
	else {
		using Specifier = Typedef::RecordType::Specifier;

		auto& res = m_recordList[std::make_pair(name, !static_cast<int>(isUnion))];
		if (res == nullptr) {
			return false;
		}

		m_typeStack.push(Util::MakeRecordType(name, isUnion ? Specifier::UNION : Specifier::STRUCT));
		m_comm.ShiftBackward();
	}

	return true;
}

void Parser::SpecifierQualifierList()
{
	std::vector<CoilCl::Typedef::TypedefBase::TypeQualifier> tmpTQ;
	auto typeCount = m_typeStack.size();

	bool cont = false;
	do {
		cont = false;

		// Find a type specifier
		if (typeCount == m_typeStack.size() && TypeSpecifier()) {
			NextToken();
			cont = true;
		}

		// Can have multiple type qualifiers, list them
		auto tq = TypeQualifier();
		if (tq != Typedef::TypedefBase::TypeQualifier::NONE) {
			NextToken();
			tmpTQ.push_back(tq);
			cont = true;
		}
	} while (cont);

	// Append all stacked storage classes and qualifiers onto the value object
	auto& baseType = m_typeStack.top();
	for (const auto& tq : tmpTQ) {
		baseType->SetQualifier(tq);
	}
}

bool Parser::EnumSpecifier()
{
	if (MATCH_TOKEN(TK_ENUM)) {
		NextToken();

		auto enm = CoilCl::AST::MakeASTNode<EnumDecl>();
		enm->SetLocation(CURRENT_LOCATION());

		if (MATCH_TOKEN(TK_IDENTIFIER)) {
			enm->SetName(CURRENT_DATA()->As<std::string>());
			NextToken();
		}

		if (MATCH_TOKEN(TK_BRACE_OPEN)) {
			for (;;) {
				NextToken();

				if (MATCH_TOKEN(TK_IDENTIFIER)) {
					auto enmConst = CoilCl::AST::MakeASTNode<EnumConstantDecl>(CURRENT_DATA()->As<std::string>());
					enmConst->SetLocation(CURRENT_LOCATION());

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

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::ADDR, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_ASTERISK:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::PTRVAL, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_PLUS:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INTPOS, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_MINUS:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INTNEG, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_TILDE:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::BITNOT, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);
		break;
	}
	case TK_NOT:
	{
		NextToken();
		CastExpression();

		auto resv = m_elementDescentPipe.next();
		m_elementDescentPipe.pop();
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::BOOLNOT, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, resv);
		unaryOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::ASSGN, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MUL, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::DIV, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MOD, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::ADD, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::SUB, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::LEFT, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::RIGHT, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::AND, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::XOR, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		auto comOp = CoilCl::AST::MakeASTNode<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::OR, resv);
		comOp->SetLocation(CURRENT_LOCATION());
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
		m_identifierStack.push(CURRENT_DATA()->As<std::string>());
		NextToken();
		break;

	case TK_CONSTANT:
		switch (CURRENT_DATA()->DataType<BuiltinType>()->TypeSpecifier()) {
		case BuiltinType::Specifier::INT:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<int>>(CURRENT_DATA());
			auto literal = CoilCl::AST::MakeASTNode<IntegerLiteral>(std::move(object));
			literal->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(literal);
			break;
		}
		case BuiltinType::Specifier::DOUBLE:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<double>>(CURRENT_DATA());
			auto literal = CoilCl::AST::MakeASTNode<FloatingLiteral>(std::move(object));
			literal->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(literal);
			break;
		}
		case BuiltinType::Specifier::CHAR:
		{
			if (CURRENT_DATA()->IsArray()) {
				auto object = std::dynamic_pointer_cast<ValueObject<std::string>>(CURRENT_DATA());
				auto literal = CoilCl::AST::MakeASTNode<StringLiteral>(std::move(object));
				literal->SetLocation(CURRENT_LOCATION());
				m_elementDescentPipe.push(literal);
			}
			else {
				auto object = std::dynamic_pointer_cast<ValueObject<char>>(CURRENT_DATA());
				auto literal = CoilCl::AST::MakeASTNode<CharacterLiteral>(std::move(object));
				literal->SetLocation(CURRENT_LOCATION());
				m_elementDescentPipe.push(literal);
			}
			break;
		}
		}
		NextToken();
		break;

	case TK_PARENTHESE_OPEN:
		NextToken();
		Expression();

		auto parenthesis = CoilCl::AST::MakeASTNode<ParenExpr>(m_elementDescentPipe.next());
		parenthesis->SetLocation(CURRENT_LOCATION());
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

void Parser::CompoundLiteral()
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

			auto list = CoilCl::AST::MakeASTNode<InitListExpr>();
			list->SetLocation(CURRENT_LOCATION());

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

			auto comp = CoilCl::AST::MakeASTNode<CompoundLiteralExpr>(list);
			comp->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(comp);
		}

		// Cannot cast, rollback the command state
		catch (const UnexpectedTokenException&) {
			m_comm.Revert();
		}
	}
}

void Parser::PostfixExpression()
{
	CompoundLiteral();

	auto startSz = m_identifierStack.size();

	PrimaryExpression();

	switch (CURRENT_TOKEN()) {
	case TK_BRACKET_OPEN:
	{
		NextToken();

		auto resv = MAKE_RESV_REF();
		resv->SetLocation(CURRENT_LOCATION());

		Expression();
		ExpectToken(TK_BRACKET_CLOSE);

		auto arrsub = CoilCl::AST::MakeASTNode<ArraySubscriptExpr>(resv, m_elementDescentPipe.next());
		arrsub->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(arrsub);
		break;
	}
	case TK_PARENTHESE_OPEN:
	{
		NextToken();

		auto resv = MAKE_RESV_REF();
		resv->SetLocation(CURRENT_LOCATION());

		if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
			NextToken();
			auto call = CoilCl::AST::MakeASTNode<CallExpr>(resv);
			call->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(call);
		}
		else {
			auto startState = m_elementDescentPipe.state();
			ArgumentExpressionList();
			m_elementDescentPipe.release_until(startState);
			ExpectToken(TK_PARENTHESE_CLOSE);

			auto arg = CoilCl::AST::MakeASTNode<ArgumentStmt>();
			arg->SetLocation(CURRENT_LOCATION());
			while (!m_elementDescentPipe.empty()) {
				arg->AppendArgument(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();
			}

			auto call = CoilCl::AST::MakeASTNode<CallExpr>(resv, arg);
			call->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(call);
		}
		break;
	}
	case TK_DOT:
	{
		auto resv = MAKE_RESV_REF();
		resv->SetLocation(CURRENT_LOCATION());

		NextToken();
		//ExpectIdentifier();

		auto member = CURRENT_DATA()->As<std::string>();
		auto expr = CoilCl::AST::MakeASTNode<MemberExpr>(MemberExpr::MemberType::REFERENCE, member, resv);
		expr->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(expr);
		NextToken();
		break;
	}
	case TK_PTR_OP:
	{
		auto resv = MAKE_RESV_REF();
		resv->SetLocation(CURRENT_LOCATION());

		NextToken();
		//ExpectIdentifier();

		auto member = CURRENT_DATA()->As<std::string>();
		auto expr = CoilCl::AST::MakeASTNode<MemberExpr>(MemberExpr::MemberType::POINTER, member, resv);
		expr->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(expr);
		NextToken();
		break;
	}
	case TK_INC_OP:
	{
		if (m_identifierStack.size() == startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}

		auto resv = MAKE_RESV_REF();
		resv->SetLocation(CURRENT_LOCATION());
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		unaryOp->SetLocation(CURRENT_LOCATION());
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
		resv->SetLocation(CURRENT_LOCATION());
		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::DEC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, std::dynamic_pointer_cast<ASTNode>(resv));
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(unaryOp);

		NextToken();
		break;
	}
	default:
		// An identifier was found without any postfix expression thereafter.
		// This is either a referenced identifier or typedef specifier. Check
		// typedef first before assuming referenced identifier. If no typedef
		// was found wrap the declaration in a declaration reference.
		if (m_identifierStack.size() > startSz) {
			if (m_typedefList[m_identifierStack.top()] == nullptr) {
				auto resv = MAKE_RESV_REF();
				resv->SetLocation(CURRENT_LOCATION());
				m_elementDescentPipe.push(resv);
			}
			// Identifier is typedef specifier, shift back and pop identifier
			// stack as typedef cannot be used as statement expression.
			else {
				m_comm.ShiftBackward();
				m_identifierStack.pop();
			}
		}
	}
}

void Parser::UnaryExpression()
{
	//auto startSz = m_identifierStack.size();

	switch (CURRENT_TOKEN()) {
	case TK_INC_OP:
	{
		NextToken();
		UnaryExpression();

		/*if (m_identifierStack.size() != startSz) {
			throw ParseException{ "expression is not assignable", 0, 0 };
		}*/

		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INC, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, m_elementDescentPipe.next());
		unaryOp->SetLocation(CURRENT_LOCATION());
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

		auto unaryOp = CoilCl::AST::MakeASTNode<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::DEC, CoilCl::AST::UnaryOperator::OperandSide::PREFIX, m_elementDescentPipe.next());
		unaryOp->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(unaryOp);

		break;
	}
	case TK_SIZEOF:
	{
		NextToken();

		auto ref = CoilCl::AST::MakeASTNode<DeclRefExpr>("sizeof");
		ref->SetLocation(CURRENT_LOCATION());
		auto func = CoilCl::AST::MakeASTNode<BuiltinExpr>(ref);
		func->SetLocation(CURRENT_LOCATION());

		// Snapshot current state in case of rollback
		m_comm.Snapshot();
		try {
			if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
				NextToken();
				TypeName();
				ExpectToken(TK_PARENTHESE_CLOSE);

				// Remove snapshot since we can continue this path
				m_comm.DisposeSnapshot();

				func->SetTypename(m_typeStack.top());
				m_typeStack.pop();
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

			auto cast = CoilCl::AST::MakeASTNode<CastExpr>(m_elementDescentPipe.next());
			cast->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::MUL, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::DIV, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::MOD, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::PLUS, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::MINUS, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::SLEFT, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::SRIGHT, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::LT, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::GT, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::LE, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::GE, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::EQ, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::NEQ, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::AND, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::XOR, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::LAND, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto binOp = CoilCl::AST::MakeASTNode<BinaryOperator>(BinaryOperator::BinOperand::LOR, m_elementDescentPipe.next());
		binOp->SetLocation(CURRENT_LOCATION());
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
		auto conOp = CoilCl::AST::MakeASTNode<ConditionalOperator>(m_elementDescentPipe.next());
		conOp->SetLocation(CURRENT_LOCATION());
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
	{
		NextToken();

		//ExpectIdentifier();//XXX: possible optimization

		auto stmt = CoilCl::AST::MakeASTNode<GotoStmt>(CURRENT_DATA()->As<std::string>());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(stmt);
		NextToken();
		break;
	}
	case TK_CONTINUE:
	{
		NextToken();
		auto stmt = CoilCl::AST::MakeASTNode<ContinueStmt>();
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(stmt);
		break;
	}
	case TK_BREAK:
	{
		NextToken();
		auto stmt = CoilCl::AST::MakeASTNode<BreakStmt>();
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.push(stmt);
		break;
	}
	case TK_RETURN:
	{
		NextToken();
		if (MATCH_TOKEN(TK_COMMIT)) {
			auto stmt = CoilCl::AST::MakeASTNode<ReturnStmt>();
			stmt->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(stmt);
		}
		else {
			Expression();

			auto returnStmt = CoilCl::AST::MakeASTNode<ReturnStmt>();
			returnStmt->SetLocation(CURRENT_LOCATION());
			while (!m_elementDescentPipe.empty()) {
				returnStmt->SetReturnNode(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();
			}

			m_elementDescentPipe.push(returnStmt);
		}
		break;
	}
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
		auto stmt = CoilCl::AST::MakeASTNode<WhileStmt>(m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		ExpectToken(TK_PARENTHESE_CLOSE);
		m_elementDescentPipe.pop();

		Statement();

		if (!m_elementDescentPipe.empty()) {
			stmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(stmt);
		return true;
	}
	case TK_DO:
	{
		NextToken();
		Statement();

		auto stmt = CoilCl::AST::MakeASTNode<DoStmt>(m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();

		ExpectToken(TK_WHILE);
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);
		ExpectToken(TK_COMMIT);

		stmt->SetEval(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(stmt);
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

		auto stmt = CoilCl::AST::MakeASTNode<ForStmt>(node1, node2, node3);
		stmt->SetLocation(CURRENT_LOCATION());

		Statement();

		if (!m_elementDescentPipe.empty()) {
			stmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(stmt);
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

		auto stmt = CoilCl::AST::MakeASTNode<IfStmt>(m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();

		Statement();

		// If the statement yields a body, save it and try
		// with alternative compound
		if (m_elementDescentPipe.size() > 0) {
			stmt->SetTruthCompound(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();

			if (MATCH_TOKEN(TK_ELSE)) {
				NextToken();
				Statement();
			}
		}

		m_elementDescentPipe.push(stmt);
		return true;
	}
	case TK_SWITCH:
	{
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);

		auto stmt = CoilCl::AST::MakeASTNode<SwitchStmt>(m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();

		Statement();

		// If the statement yields a body, save it
		if (m_elementDescentPipe.size() > 0) {
			stmt->SetBody(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(stmt);
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
		auto stmt = CoilCl::AST::MakeASTNode<CompoundStmt>();
		stmt->SetLocation(CURRENT_LOCATION());
		while (!m_elementDescentPipe.empty()) {
			stmt->AppendChild(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(stmt);
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
			NextToken();
			ExpectToken(TK_COLON);

			// Remove snapshot since we can continue this path
			m_comm.DisposeSnapshot();
			Statement();

			if (m_elementDescentPipe.empty()) {
				throw ParseException{ "expected statement", 0, 0 };
			}

			auto stmt = CoilCl::AST::MakeASTNode<LabelStmt>(lblName, m_elementDescentPipe.next());
			stmt->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.pop();
			m_elementDescentPipe.push(stmt);
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

		auto stmt = CoilCl::AST::MakeASTNode<CaseStmt>(lblLiteral, m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(stmt);
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

		auto stmt = CoilCl::AST::MakeASTNode<DefaultStmt>(m_elementDescentPipe.next());
		stmt->SetLocation(CURRENT_LOCATION());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(stmt);
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

// Loop as long as there are items found within the block
void Parser::BlockItems()
{
	do {
		auto itemState = m_elementDescentPipe.state();

		// Test statement first
		Statement();
		if (MATCH_TOKEN(TK_BRACE_CLOSE)) {
			break;
		}

		if (m_elementDescentPipe.is_changed(itemState)) {
			m_elementDescentPipe.lock();
			itemState = m_elementDescentPipe.state();
		}

		// Continue with declaration
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
	// Check if the specifier results yield a typedef
	else if (m_typeStack.top()->StorageClass() == Typedef::BuiltinType::StorageClassSpecifier::TYPEDEF) {
		if (Declarator()) {
			ExpectToken(TK_COMMIT);

			auto name = m_identifierStack.top();
			auto decl = CoilCl::AST::MakeASTNode<TypedefDecl>(name, m_typeStack.top());
			decl->SetLocation(CURRENT_LOCATION());
			decl->ReturnType().SetPointer(m_pointerCounter);
			m_pointerCounter = 0;
			m_typedefList[name] = m_typeStack.top();
			m_identifierStack.pop();
			m_typeStack.pop();

			m_elementDescentPipe.push(decl);
			return;
		}
	}
	else {
		auto initState = m_elementDescentPipe.state();
		InitDeclaratorList();
		if (m_elementDescentPipe.is_changed(initState)) {
			m_elementDescentPipe.release_until(initState);

			auto decl = CoilCl::AST::MakeASTNode<DeclStmt>();
			decl->SetLocation(CURRENT_LOCATION());
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

			auto decl = CoilCl::AST::MakeASTNode<VarDecl>(m_identifierStack.top(), m_typeStack.top(), m_elementDescentPipe.next());
			decl->SetLocation(CURRENT_LOCATION());
			decl->ReturnType().SetPointer(m_pointerCounter);
			m_pointerCounter = 0;
			m_identifierStack.pop();
			m_elementDescentPipe.pop();

			m_elementDescentPipe.push(decl);
			m_elementDescentPipe.lock();
		}
		else {
			auto decl = CoilCl::AST::MakeASTNode<VarDecl>(m_identifierStack.top(), m_typeStack.top());
			decl->SetLocation(CURRENT_LOCATION());
			decl->ReturnType().SetPointer(m_pointerCounter);
			m_pointerCounter = 0;
			m_identifierStack.pop();
			m_elementDescentPipe.push(decl);
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

			// Expect function
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

			// Expect array
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

		auto expr = CoilCl::AST::MakeASTNode<InitListExpr>();
		expr->SetLocation(CURRENT_LOCATION());
		while (!m_elementDescentPipe.empty()) {
			expr->AddListItem(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(expr);
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

// As long as we find pointers, continue
void Parser::Pointer()
{
	while (MATCH_TOKEN(TK_ASTERISK)) {
		m_pointerCounter++;
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

			// Expect function
		case TK_PARENTHESE_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();

				auto decl = CoilCl::AST::MakeASTNode<FunctionDecl>(m_identifierStack.top(), m_typeStack.top());
				decl->SetLocation(CURRENT_LOCATION());
				decl->ReturnType().SetPointer(m_pointerCounter);
				m_pointerCounter = 0;
				m_identifierStack.pop();
				m_typeStack.pop();

				m_elementDescentPipe.push(decl);
				return true;
			}
			else {
				const auto IdentifierListDecl = [=]
				{
					auto stmt = CoilCl::AST::MakeASTNode<ParamStmt>();
					stmt->SetLocation(CURRENT_LOCATION());

					for (;;) {
						if (MATCH_TOKEN(TK_IDENTIFIER)) {
							//TODO
							/*auto paramDecl = CoilCl::AST::MakeASTNode<ParamDecl>(CURRENT_DATA()->As<std::string>());
							paramDecl->SetLocation(CURRENT_LOCATION());
							stmt->AppendParamter(std::dynamic_pointer_cast<ASTNode>(paramDecl));*/
							NextToken();
						}

						if (NOT_TOKEN(TK_COMMA)) {
							break;
						}

						NextToken();
					}

					m_elementDescentPipe.push(stmt);
				};

				// Try default parameter declarations first, if that fails
				// give the ol' K&R decls a go
				if (!ParameterTypeList()) {
					IdentifierListDecl();
				}

				auto decl = CoilCl::AST::MakeASTNode<FunctionDecl>(m_identifierStack.top(), m_typeStack.top());
				decl->SetLocation(CURRENT_LOCATION());
				decl->SetParameterStatement(std::dynamic_pointer_cast<ParamStmt>(m_elementDescentPipe.next()));
				decl->ReturnType().SetPointer(m_pointerCounter);
				m_pointerCounter = 0;
				m_elementDescentPipe.pop();
				m_typeStack.pop();
				ExpectToken(TK_PARENTHESE_CLOSE);

				m_elementDescentPipe.push(decl);
				return true;
			}
			break;

			// Expect array
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

			// No body, bail
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
	auto rs = false;
	auto startState = m_elementDescentPipe.state();

	for (;;) {
		if (MATCH_TOKEN(TK_ELLIPSIS)) {
			NextToken();

			auto decl = CoilCl::AST::MakeASTNode<VariadicDecl>();
			decl->SetLocation(CURRENT_LOCATION());
			m_elementDescentPipe.push(decl);
			rs = true;
		}

		if (ParameterDeclaration()) {
			rs = true;
		}

		if (NOT_TOKEN(TK_COMMA)) {
			break;
		}

		NextToken();
	}

	m_elementDescentPipe.release_until(startState);

	if (rs) {
		auto stmt = CoilCl::AST::MakeASTNode<ParamStmt>();
		stmt->SetLocation(CURRENT_LOCATION());
		while (!m_elementDescentPipe.empty()) {
			stmt->AppendParamter(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		m_elementDescentPipe.push(stmt);
	}

	return rs;
}

// Parameter declaration can have only a specifier;
// a specifier and declarator or specifier and abstract
// declarator.
bool Parser::ParameterDeclaration()
{
	if (!DeclarationSpecifiers()) {
		return false;
	}

	// If a declarator was found, create the parameter and pop all used lists
	// since the next parameter cannot depend on this data.
	if (Declarator()) {
		auto decl = CoilCl::AST::MakeASTNode<ParamDecl>(m_identifierStack.top(), m_typeStack.top());
		decl->SetLocation(CURRENT_LOCATION());
		decl->ReturnType().SetPointer(m_pointerCounter);
		m_pointerCounter = 0;
		m_identifierStack.pop();
		m_typeStack.pop();

		m_elementDescentPipe.push(decl);
		m_elementDescentPipe.lock();
		return true;
	}

	// Found a type, but no identifier. Add type as abstract declaration
	AbstractDeclarator();
	auto decl = CoilCl::AST::MakeASTNode<ParamDecl>(m_typeStack.top());
	decl->SetLocation(CURRENT_LOCATION());
	decl->ReturnType().SetPointer(m_pointerCounter);
	m_pointerCounter = 0;
	m_typeStack.pop();

	m_elementDescentPipe.push(decl);
	m_elementDescentPipe.lock();
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

	// Check if we found a function
	auto func = std::dynamic_pointer_cast<FunctionDecl>(m_elementDescentPipe.next());
	if (func == nullptr) {
		m_elementDescentPipe.release_until(startState);
		return false;
	}

	// Found a function statement, lock it
	m_elementDescentPipe.lock();

	while (Declarator());
	auto res = CompoundStatement();
	if (res) {
		func->SetCompound(std::dynamic_pointer_cast<CompoundStmt>(m_elementDescentPipe.next()));
		m_elementDescentPipe.pop();
	}

	// Release locks since function declaration
	m_elementDescentPipe.release_until(startState);
	return true;
}

// Try as function; if that fails assume declaration
void Parser::ExternalDeclaration()
{
	// Usless commits must be ignored, this saves as few trips
	// into function or declaration statements.
	if (MATCH_TOKEN(TK_COMMIT)) {
		NextToken();
	}

	// Snapshot current state in case of rollback
	m_comm.Snapshot();

	if (!FunctionDefinition()) {
		m_comm.Revert();

		// Clear all states since nothing should be kept moving forward
		m_pointerCounter = 0;
		m_elementDescentPipe.clear();
		ClearStack(m_typeStack);
		ClearStack(m_identifierStack);

		// Function failed, must succeed as declaration
		Declaration();
	}
	else {
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
	auto localAst = TranslationUnitDecl::Make(m_profile->MetaInfo()->name);
	localAst->SetLocation(0, 0);

	// Set translation unit as top level tree root
	if (AST_ROOT() == nullptr) {
		AST_ROOT() = localAst;
	}

	do {
		// All the translation units start with a declaration
		ExternalDeclaration();

		// Move elements from list into AST root
		while (!m_elementDescentPipe.empty()) {
			AST_ROOT()->AppendChild(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		// Clear all lists where possible before adding new items
		m_pointerCounter = 0;
		m_elementDescentPipe.clear();
		ClearStack(m_typeStack);
		ClearStack(m_identifierStack);
		m_comm.TryClear();
	} while (!lex.IsDone());
}

// Run the parser. If the translation unit is done after
// the first token, we either processed an empty file or
// a source file with only comments and zero tokens. When this
// occurs, return without any ast root tree
Parser& Parser::Execute()
{
	NextToken();
	if (!lex.IsDone()) {
		TranslationUnit();
	}

	return (*this);
}
