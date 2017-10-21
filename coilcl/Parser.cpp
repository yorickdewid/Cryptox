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
#define EMIT(m) std::cout << "EMIT::" << m << std::endl;
#define EMIT_IDENTIFIER() std::cout << "EMIT::IDENTIFIER" << "("<< CURRENT_DATA()->As<std::string>() << ")" << std::endl;

class UnexpectedTokenException : public std::exception
{
public:
	UnexpectedTokenException() noexcept
	{
	}

	explicit UnexpectedTokenException(char const* const message, int line, int column) noexcept
		: m_line{ line }
		, m_column{ column }
	{
		std::stringstream ss;
		ss << "Semantic error: " << message;
		ss << " before " << "' TOKEN '" << " token at "; //TODO: token
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
	ParseException() noexcept
	{
	}

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
	: m_profile{ profile }
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
	return *this;
}

void Parser::Error(const char* err)
{
	throw UnexpectedTokenException{ err, m_comm.Current().FetchLine(), m_comm.Current().FetchColumn() };
}

void Parser::ExpectToken(Token token)
{
	if (NOT_TOKEN(token)) {
		Error("expected expression");
	}

	NextToken();
}

void Parser::ExpectIdentifier()
{
	if (NOT_TOKEN(TK_IDENTIFIER)) {
		Error("expected identifier");
	}

	assert(m_comm.Current().HasData());

	NextToken();
}

auto Parser::StorageClassSpecifier()
{
	switch (CURRENT_TOKEN()) {
	case TK_REGISTER:
		return Value::StorageClassSpecifier::REGISTER;
	case TK_STATIC:
		return Value::StorageClassSpecifier::STATIC;
	case TK_TYPEDEF:
		return Value::StorageClassSpecifier::TYPEDEF;
	}

	return Value::StorageClassSpecifier::NONE;
}

std::unique_ptr<Value> Parser::TypeSpecifier()
{
	// TODO:
	// - Complex
	// - Imaginary
	// - TYPE_NAME
	switch (CURRENT_TOKEN()) {
	case TK_VOID:
		return std::move(std::make_unique<ValueObject<void>>(Value::TypeSpecifier::T_VOID));
	case TK_CHAR:
		return std::move(std::make_unique<ValueObject<std::string>>(Value::TypeSpecifier::T_CHAR));
	case TK_SHORT:
		return std::move(std::make_unique<ValueObject<short>>(Value::TypeSpecifier::T_SHORT));
	case TK_INT:
		return std::move(std::make_unique<ValueObject<int>>(Value::TypeSpecifier::T_INT));
	case TK_LONG:
		return std::move(std::make_unique<ValueObject<long>>(Value::TypeSpecifier::T_LONG));
	case TK_FLOAT:
		return std::move(std::make_unique<ValueObject<float>>(Value::TypeSpecifier::T_FLOAT));
	case TK_DOUBLE:
		return std::move(std::make_unique<ValueObject<double>>(Value::TypeSpecifier::T_DOUBLE));
	case TK_SIGNED:
		return std::move(std::make_unique<ValueObject<signed>>(Value::TypeSpecifier::T_INT));
	case TK_UNSIGNED:
		return std::move(std::make_unique<ValueObject<unsigned>>(Value::TypeSpecifier::T_INT));
	case TK_BOOL:
		return std::move(std::make_unique<ValueObject<bool>>(Value::TypeSpecifier::T_BOOL));
	}

	EnumSpecifier();
	StructOrUnionSpecifier();

	return nullptr;
}

auto Parser::TypeQualifier()
{
	switch (CURRENT_TOKEN()) {
	case TK_CONST:
		return Value::TypeQualifier::CONST;
	case TK_VOLATILE:
		return Value::TypeQualifier::VOLATILE;
	}

	return Value::TypeQualifier::NONE;
}

bool Parser::DeclarationSpecifiers()
{
	std::shared_ptr<Value> tmpType = nullptr;
	Value::StorageClassSpecifier tmpSCP = Value::StorageClassSpecifier::NONE;
	Value::TypeQualifier tmpTQ = Value::TypeQualifier::NONE;

	bool cont = true;
	while (cont) {
		cont = false;

		auto sc = StorageClassSpecifier();
		if (static_cast<int>(sc)) {
			tmpSCP = sc;
			NextToken();
			cont = true;
		}

		auto type = TypeSpecifier();
		if (type != nullptr) {
			NextToken();
			cont = true;
			tmpType = std::move(type);
		}

		auto tq = TypeQualifier();
		if (static_cast<int>(tq)) {
			tmpTQ = tq;
			NextToken();
			cont = true;
		}
	}

	if (tmpType == nullptr) {
		return false;
	}

	if (tmpSCP != Value::StorageClassSpecifier::NONE) {
		tmpType->StorageClass(tmpSCP);
	}
	if (tmpTQ != Value::TypeQualifier::NONE) {
		tmpType->Qualifier(tmpTQ);
	}

	//m_specifiekStack.push(std::make_unique<ValueNode>(type));
	return true;
}

void Parser::StructOrUnionSpecifier()
{
	switch (CURRENT_TOKEN()) {
	case TK_STRUCT:
		NextToken();
		EMIT("STRUCT");
		break;
	case TK_UNION:
		NextToken();
		EMIT("UNION");
		break;
	default: // return if no union or struct was found
		return;
	}

	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		EMIT_IDENTIFIER();
		NextToken();
	}

	if (MATCH_TOKEN(TK_BRACE_OPEN)) {
		NextToken();
		StructDeclarationList();
		ExpectToken(TK_BRACE_CLOSE);
	}
}

void Parser::StructDeclarationList()
{
	do {
		SpecifierQualifierList();
		StructDeclaratorList();
		ExpectToken(TK_COMMIT);
	} while (NOT_TOKEN(TK_BRACE_CLOSE));
}

void Parser::SpecifierQualifierList()
{
	bool cont = false;
	do {
		cont = false;
		auto type = TypeSpecifier();
		if (type != nullptr) {
			NextToken();
			cont = true;
		}

		auto tq = TypeQualifier();
		if (tq != Value::TypeQualifier::NONE) {
			cont = true;
		}
	} while (cont);
}

void Parser::StructDeclaratorList()
{
	do {
		Declarator();

		if (MATCH_TOKEN(TK_COLON)) {
			NextToken();
			ConstantExpression();
		}
	} while (MATCH_TOKEN(TK_COMMA));
}

void Parser::EnumSpecifier()
{
	if (MATCH_TOKEN(TK_ENUM)) {
		NextToken();
		if (MATCH_TOKEN(TK_IDENTIFIER)) {
			EMIT_IDENTIFIER();
			NextToken();
		}
		if (MATCH_TOKEN(TK_BRACE_OPEN)) {
			EnumeratorList();
			ExpectToken(TK_BRACE_CLOSE);
		}
	}
}

void Parser::EnumeratorList()
{
	do {
		NextToken();
		if (MATCH_TOKEN(TK_IDENTIFIER)) {
			EMIT_IDENTIFIER();
			NextToken();
			if (MATCH_TOKEN(TK_ASSIGN)) {
				NextToken();
				ConstantExpression();
			}
		}
	} while (MATCH_TOKEN(TK_COMMA));
}

bool Parser::UnaryOperator()
{
	switch (CURRENT_TOKEN()) {
	case TK_AND_OP:
		NextToken();
		EMIT("AND");
		return true;
	case TK_AMPERSAND:
		NextToken();
		EMIT("AMPERSAND");
		return true;
	case TK_PLUS:
		NextToken();
		EMIT("PLUS");
		return true;
	case TK_MINUS:
		NextToken();
		EMIT("MINUS");
		return true;
	case TK_TILDE:
		NextToken();
		EMIT("TILDE");
		return true;
	case TK_NOT:
		NextToken();
		EMIT("NOT");
		return true;
	}

	return false;
}

void Parser::AssignmentOperator()
{
	switch (CURRENT_TOKEN()) {
	case TK_ASSIGN:
		NextToken();
		EMIT("ASSIGN");
		break;
	case TK_MUL_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MUL, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_DIV_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::DIV, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_MOD_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::MOD, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_ADD_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::ADD, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_SUB_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::SUB, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_LEFT_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::LEFT, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_RIGHT_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::RIGHT, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_AND_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::AND, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_XOR_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::XOR, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	case TK_OR_ASSIGN:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& funcPtr) -> bool
		{
			return funcPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto comOp = std::make_shared<CompoundAssignOperator>(CompoundAssignOperator::CompoundAssignOperand::OR, ref);

		NextToken();
		AssignmentExpression();

		comOp->SetRightSide(m_elementDescentPipe.next());
		m_elementDescentPipe.pop();
		m_elementDescentPipe.push(comOp);
		break;
	}
	}
}

void Parser::PrimaryExpression()
{
	switch (CURRENT_TOKEN()) {
	case TK_IDENTIFIER:
		EMIT_IDENTIFIER();
		m_identifierStack.push(CURRENT_DATA()->As<std::string>());
		NextToken();
		break;

	case TK_CONSTANT:
		switch (CURRENT_DATA()->DataType()) {
		case Value::TypeSpecifier::T_INT:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<int>>(CURRENT_DATA());
			m_elementDescentPipe.push(std::make_shared<IntegerLiteral>(std::move(object)));
			break;
		}
		case Value::TypeSpecifier::T_DOUBLE:
		{
			auto object = std::dynamic_pointer_cast<ValueObject<double>>(CURRENT_DATA());
			m_elementDescentPipe.push(std::make_shared<FloatingLiteral>(std::move(object)));
			break;
		}
		case Value::TypeSpecifier::T_CHAR:
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
//TODO: merge with Expression()
void Parser::ArgumentExpressionList()
{
	auto cont = false;
	do {
		cont = false;
		auto itemState = m_elementDescentPipe.state();
		AssignmentExpression();
		if (m_elementDescentPipe.is_changed(itemState)) {
			m_elementDescentPipe.lock();
		}

		if (MATCH_TOKEN(TK_COMMA)) {
			NextToken();
			cont = true;
		}
	} while (cont);
}

void Parser::PostfixExpression()
{
	//TODO: Unknown flow
	/*if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
		NextToken();
		TypeName();
		ExpectToken(TK_PARENTHESE_CLOSE);
		ExpectToken(TK_BRACE_OPEN);
		do {
			InitializerList();
		} while (MATCH_TOKEN(TK_COMMA));
		ExpectToken(TK_BRACE_CLOSE);
	}*/

	PrimaryExpression();

	switch (CURRENT_TOKEN()) {
	case TK_BRACKET_OPEN:
		NextToken();
		Expression();
		ExpectToken(TK_BRACKET_CLOSE);
		break;
	case TK_PARENTHESE_OPEN:
		NextToken();
		if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
			NextToken();

			const auto& refIdentifier = m_identifierStack.top();
			auto funcDelc = stash->Resolve<FunctionDecl, Decl>([&refIdentifier](std::shared_ptr<FunctionDecl>& funcPtr) -> bool
			{
				return funcPtr->Identifier() == refIdentifier;
			});

			if (funcDelc == nullptr) {
				throw ParseException{ std::string{ "implicit declaration of function '" + refIdentifier + "' is invalid" }.c_str(), 0, 0 };
			}

			auto ref = make_ref(funcDelc);

			m_elementDescentPipe.push(std::make_shared<CallExpr>(ref));
		}
		else {
			auto startState = m_elementDescentPipe.state();
			ArgumentExpressionList();
			m_elementDescentPipe.release_until(startState);

			auto arg = std::make_shared<ArgumentStmt>();
			while (!m_elementDescentPipe.empty()) {
				arg->AppendArgument(m_elementDescentPipe.next());
				m_elementDescentPipe.pop();
			}

			//std::make_shared<DeclRefExpr>(m_identifierStack.top());
			//m_elementDescentPipe.push(std::make_shared<CallExpr>(arg));
			EMIT("CALL EXPRESSION");
			ExpectToken(TK_PARENTHESE_CLOSE);
		}
		break;
	case TK_DOT:
		NextToken();
		EMIT("DOT");
		ExpectIdentifier();
		break;
	case TK_PTR_OP:
		NextToken();
		EMIT("POINTER");
		ExpectIdentifier();
		break;
	case TK_INC_OP:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& varPtr) -> bool
		{
			return varPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "use of undeclared identifier '" + refIdentifier + "'" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::INC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, ref);
		m_elementDescentPipe.push(unaryOp);

		NextToken();
		break;
	}
	case TK_DEC_OP:
	{
		const auto& refIdentifier = m_identifierStack.top();
		auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& varPtr) -> bool
		{
			return varPtr->Identifier() == refIdentifier;
		});

		if (decl == nullptr) {
			throw ParseException{ std::string{ "use of undeclared identifier '" + refIdentifier + "'" }.c_str(), 0, 0 };
		}

		m_identifierStack.pop();
		auto ref = make_ref(decl);

		auto unaryOp = std::make_shared<CoilCl::AST::UnaryOperator>(CoilCl::AST::UnaryOperator::UnaryOperator::DEC, CoilCl::AST::UnaryOperator::OperandSide::POSTFIX, ref);
		m_elementDescentPipe.push(unaryOp);

		NextToken();
		break;
	}
	}
}

void Parser::UnaryExpression()
{
	switch (CURRENT_TOKEN()) {
	case TK_INC_OP:
		NextToken();
		EMIT("++");
		UnaryExpression();
		break;
	case TK_DEC_OP:
		NextToken();
		EMIT("--");
		UnaryExpression();
		break;
	case TK_SIZEOF:
		NextToken();
		if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
			EMIT("SIZEOF");
			TypeName();
			ExpectToken(TK_PARENTHESE_CLOSE);
		}
		else {
			UnaryExpression();
		}
		break;
	default:
		if (UnaryOperator()) {
			CastExpression();
		}
		else {
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

			// Remove snapshot since we can continue this path
			m_comm.DisposeSnapshot();
			CastExpression();

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
		NextToken();
		EMIT("CMP <");
		ShiftExpression();
		break;
	case TK_GREATER_THAN:
		NextToken();
		EMIT("CMP >");
		ShiftExpression();
		break;
	case TK_LE_OP:
		NextToken();
		EMIT("CMP <=");
		ShiftExpression();
		break;
	case TK_GE_OP:
		NextToken();
		EMIT("CMP >=");
		ShiftExpression();
		break;
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
		NextToken();
		EMIT("&&");
		ExclusiveOrExpression();
	}
}

void Parser::LogicalOrExpression()
{
	LogicalAndExpression();

	if (MATCH_TOKEN(TK_OR_OP)) {
		NextToken();
		EMIT("||");
		LogicalAndExpression();
	}
}

void Parser::ConditionalExpression()
{
	LogicalOrExpression();

	if (MATCH_TOKEN(TK_QUESTION_MARK)) {
		NextToken();
		Expression();
		ExpectToken(TK_COLON);
		ConditionalExpression();
	}
}

void Parser::AssignmentExpression()
{
	ConditionalExpression();

	UnaryExpression();
	AssignmentOperator();
}

void Parser::Expression()
{
	do {
		AssignmentExpression();
	} while (MATCH_TOKEN(TK_COMMA));
}

void Parser::ConstantExpression()
{
	ConditionalExpression();
}

// Labels and gotos
bool Parser::JumpStatement()
{
	switch (CURRENT_TOKEN()) {
	case TK_GOTO:
		NextToken();
		ExpectIdentifier();
		EMIT("JMP GOTO");
		break;
	case TK_CONTINUE:
		NextToken();
		EMIT("ITR CONTINUE");
		break;
	case TK_BREAK:
		NextToken();
		EMIT("ITR/SWTCH BREAK");
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
	default: // Return if no match
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
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		return true;
	case TK_DO:
		Statement();
		ExpectToken(TK_WHILE);
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		ExpectToken(TK_COMMIT);
		return true;
	case TK_FOR:
		ExpectToken(TK_BRACE_OPEN);
		// expression_statement expression_statement
		//
		// expression_statement expression_statement expression
		//
		// declaration expression_statement
		//
		// declaration expression_statement expression
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		return true;
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
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		return true;
	}
	}

	return false;
}

void Parser::ExpressionStatement()
{
	if (MATCH_TOKEN(TK_COMMIT)) {
		NextToken();
		// EMIT
	}
	else {
		Expression();
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
		EMIT_IDENTIFIER();
		NextToken();
		if (MATCH_TOKEN(TK_COLON)) {
			NextToken();
			Statement();
			return true;
		}
		return false;
	case TK_CASE:
		NextToken();
		ConstantExpression();
		ExpectToken(TK_COLON);
		Statement();
		return true;
	case TK_DEFAULT:
		NextToken();
		ExpectToken(TK_COLON);
		Statement();
		return true;
	}

	return false;
}

void Parser::Statement()
{
	if (CompoundStatement()) { return; }
	if (SelectionStatement()) { return; }
	if (IterationStatement()) { return; }
	if (JumpStatement()) { return; }
	ExpressionStatement();
	if (LabeledStatement()) { return; }
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
	DeclarationSpecifiers();

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
			Initializer();

			std::shared_ptr<VarDecl> var;
			if (!m_elementDescentPipe.empty()) {

				if (m_elementDescentPipe.size() > 1) {
					auto list = std::make_shared<InitListExpr>();
					while (!m_elementDescentPipe.empty()) {
						list->AddListItem(m_elementDescentPipe.next());
						m_elementDescentPipe.pop();
					}
					m_elementDescentPipe.push(list);
				}

				var = std::make_shared<VarDecl>(m_identifierStack.top(), m_elementDescentPipe.next());
				m_identifierStack.pop();
				m_elementDescentPipe.pop();
			}
			else {
				const auto& refIdentifier = m_identifierStack.top();
				auto decl = stash->Resolve<VarDecl, Decl>([&refIdentifier](std::shared_ptr<VarDecl>& varPtr) -> bool
				{
					return varPtr->Identifier() == refIdentifier;
				});

				if (decl == nullptr) {
					throw ParseException{ std::string{ "use of undeclared identifier '" + refIdentifier + "'" }.c_str(), 0, 0 };
				}

				m_identifierStack.pop();
				auto ref = make_ref(decl);

				var = std::make_shared<VarDecl>(m_identifierStack.top(), ref);
				m_identifierStack.pop();
			}

			assert(var != nullptr);

			stash->Enlist(var);

			m_elementDescentPipe.push(var);
			m_elementDescentPipe.lock();
		}
		else {
			auto var = std::make_shared<VarDecl>(m_identifierStack.top());
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

// Typenames are primitive types
// and used defined structures
void Parser::TypeName()
{
	SpecifierQualifierList();

	AbstractDeclarator();
}

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
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();
				EMIT("FUNC DELC EMPTY");
				cont = true;
			}
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
			if (MATCH_TOKEN(TK_BRACKET_CLOSE)) {
				NextToken();
				EMIT("UNINIT ARRAY?");
				cont = true;
			}
			else if (MATCH_TOKEN(TK_ASTERISK)) {
				NextToken();
				ExpectToken(TK_BRACKET_CLOSE);
				EMIT("UNINIT ARRAY VARIABLE SZ?");
				cont = true;
			}
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
	}
	else {
		AssignmentExpression();
	}
}

//void Parser::InitializerList()
//{
//	do {
//		Designation();
//		Initializer();
//	} while (MATCH_TOKEN(TK_COMMA));
//}

void Parser::Designation()
{
	Designators();
	ExpectToken(TK_ASSIGN);//TODO
}

void Parser::Designators()
{
	bool cont = false;
	do {
		switch (CURRENT_TOKEN()) {
		case TK_BRACKET_OPEN:
			NextToken();
			ConstantExpression();
			ExpectToken(TK_BRACKET_CLOSE);
			cont = true;
			break;
		case TK_DOT:
			NextToken();
			ExpectIdentifier();
			cont = true;
			break;
		}
	} while (cont);
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

				auto func = std::make_shared<FunctionDecl>(m_identifierStack.top());
				m_identifierStack.pop();

				stash->Enlist(func);
				m_elementDescentPipe.push(func);
				return true;
			}
			else {
				if (!ParameterTypeList()) {
					do {
						if (MATCH_TOKEN(TK_IDENTIFIER)) {
							EMIT_IDENTIFIER();
							NextToken();
						}
						else {
							return false;//TMP
						}
					} while (MATCH_TOKEN(TK_COMMA));
				}

				//TODO: add parameters
				auto func = std::make_shared<FunctionDecl>(m_identifierStack.top());
				ExpectToken(TK_PARENTHESE_CLOSE);

				stash->Enlist(func);
				m_elementDescentPipe.push(func);
				return true;
			}
			break;
		case TK_BRACKET_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_BRACKET_CLOSE)) {
				NextToken();
				EMIT("UNINIT ARRAY");
				return true;//TODO: return ptr
			}
			else if (MATCH_TOKEN(TK_ASTERISK)) {
				NextToken();
				EMIT("UNINIT ARRAY VARIABLE SZ");
				ExpectToken(TK_BRACE_CLOSE);
				return true;//TODO: return ptr
			}

			break;
		default:
			goto break_loop;
		}

		//'[' TypeQualifierList() AssignmentExpression(); ']'
		//'[' TypeQualifierList() ']'
		//'[' assignment_expression ']'
		//'[' STATIC TypeQualifierList() AssignmentExpression(); ']'
		//'[' TypeQualifierList() STATIC AssignmentExpression(); ']'
		//'[' TypeQualifierList() '*' ']'
	}

break_loop:
	return foundDecl;
}

void Parser::TypeQualifierList()
{
	while (TypeQualifier() != Value::TypeQualifier::NONE);
}

bool Parser::ParameterTypeList()
{
	bool rs = false;
	do {
		rs = ParameterDeclaration();
	} while (MATCH_TOKEN(TK_COMMA));

	if (MATCH_TOKEN(TK_COMMA)) {
		ExpectToken(TK_ELLIPSIS);
	}

	return rs;
}

bool Parser::ParameterDeclaration()
{
	//DeclarationSpecifiers();
	if (!DeclarationSpecifiers()) { //TMP
		return false;
	}

	Declarator();
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
		//TODO: bind ?
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
		Declaration();
	}
	else {
		// Remove snapshot since we can continue this path
		m_comm.DisposeSnapshot();
	}
}

void Parser::TranslationUnit()
{
	//TODO: For each translation unit run the parser loop

	//TODO: name returns file name of current lexer, not translation unit
	AST_ROOT() = std::make_shared<TranslationUnitDecl>(m_profile->MetaInfo()->name);

	do {
		ExternalDeclaration();

		if (!m_elementDescentPipe.empty()) {
			AST_ROOT()->AppendChild(m_elementDescentPipe.next());
			m_elementDescentPipe.pop();
		}

		// Clear all lists where possible before adding new items
		ClearStack(m_identifierStack);
		m_elementDescentPipe.clear();
		m_comm.TryClear();
	} while (!lex.IsDone());
}

Parser& Parser::Execute()
{
	NextToken();
	TranslationUnit();

	return *this;
}
