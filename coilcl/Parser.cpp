#include "Parser.h"

#include <iostream>
#include <exception>
#include <sstream>

#define CURRENT_DATA() m_comm.Current().FetchData()
#define PREVIOUS_TOKEN() m_comm.Previous().FetchToken()
#define CURRENT_TOKEN() m_comm.Current().FetchToken()
#define MATCH_TOKEN(t) (CURRENT_TOKEN() == t)
#define NOT_TOKEN(t) (CURRENT_TOKEN() != t)

class UnexpectedTokenException : public std::exception
{
public:
	UnexpectedTokenException() throw()
	{
	}

	explicit UnexpectedTokenException(char const* const message) throw()
		: m_err{ message }
	{
	}

	explicit UnexpectedTokenException(char const* const message, int line, int column) throw()
		: m_err{ message }
		, m_line{ line }
		, m_column{ column }
	{
	}

	UnexpectedTokenException(UnexpectedTokenException const& rhs) throw()
	{
		// TODO: copy over the private data of this class
	}

	virtual int Line() const throw ()
	{
		return m_line;
	}

	virtual int Column() const throw ()
	{
		return m_column;
	}

	// TODO: nullpointer exception
	virtual const char *what() const throw ()
	{
		std::stringstream ss;
		ss << "Semantic error: " << m_err;
		ss << " before " << "' TOKEN '" << " token at "; //TODO: token
		ss << m_line << ':' << m_column;
		return ss.str().c_str();
	}

private:
	const char *m_err = nullptr;
	int m_line;
	int m_column;
};

class SyntaxException : public std::exception
{
public:
	SyntaxException() throw()
	{
	}

	explicit SyntaxException(char const* const message) throw()
		: m_err{ message }
	{
	}

	explicit SyntaxException(char const* const message, char token, int line, int column) throw()
		: m_err{ message }
		, m_token{ token }
		, m_line{ line }
		, m_column{ column }
	{
	}

	SyntaxException(SyntaxException const& rhs) throw()
	{
		// TODO: copy over the private data of this class
	}

	virtual int Line() const throw ()
	{
		return m_line;
	}

	virtual int Column() const throw ()
	{
		return m_column;
	}

	// TODO: nullpointer exception
	virtual const char *what() const throw ()
	{
		std::stringstream ss;
		ss << "Syntax error: " << m_err << " at ";
		ss << m_line << ':' << m_column;
		return ss.str().c_str();
	}

private:
	const char *m_err = nullptr;
	char m_token;
	int m_line;
	int m_column;
};

Parser::Parser(const std::string& input)
	: lex{ input }
{
	lex.ErrorHandler([](const std::string& err, char token, int line, int column) {
		std::cerr << "Syntax error: " << err << " at " << line << ":" << column << std::endl;
		//TODO: throw SyntaxException()
	});
}

void Parser::Error(const char* err)
{
	int line = 0;//TODO
	int column = 1;//TODO

	throw UnexpectedTokenException(err, line, column);
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

	//assert(m_currentData.get() != nullptr);
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
	// - struct_or_union_specifier
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
	std::unique_ptr<Value> _type = nullptr;
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
			_type = std::move(type);
		}

		auto tq = TypeQualifier();
		if (static_cast<int>(tq)) {
			tmpTQ = tq;
			NextToken();
			cont = true;
		}
	}

	if (_type == nullptr) {
		return false;
	}

	if (tmpSCP != Value::StorageClassSpecifier::NONE) {
		_type->StorageClass(tmpSCP);
	}
	if (tmpTQ != Value::TypeQualifier::NONE) {
		_type->Qualifier(tmpTQ);
	}

	m_elementStack.push(std::move(std::make_unique<ValueNode>(_type)));
	return true;
}

void Parser::StructOrUnionSpecifier()
{
	switch (CURRENT_TOKEN())
	{
	case TK_STRUCT:
		NextToken();
		// EMIT
		break;
	case TK_UNION:
		NextToken();
		// EMIT
		break;
	default: // return if no union or struct was found
		return;
	}

	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		NextToken();
		// EMIT
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
			NextToken();
			// EMIT
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
			NextToken();
			// EMIT

			if (MATCH_TOKEN(TK_ASSIGN)) {
				NextToken();
				ConstantExpression();
			}
		}
	} while (MATCH_TOKEN(TK_COMMA));
}

bool Parser::UnaryOperator()
{
	switch (CURRENT_TOKEN())
	{
	case TK_AND_OP:
		NextToken();
		// EMIT
		return true;
	case TK_AMPERSAND:
		NextToken();
		// EMIT
		return true;
	case TK_PLUS:
		NextToken();
		// EMIT
		return true;
	case TK_MINUS:
		NextToken();
		// EMIT
		return true;
	case TK_TILDE:
		NextToken();
		// EMIT
		return true;
	case TK_NOT:
		NextToken();
		// EMIT
		return true;
	}

	return false;
}

bool Parser::AssignmentOperator()
{
	switch (CURRENT_TOKEN())
	{
	case TK_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_MUL_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_DIV_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_MOD_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_ADD_ASSIGN:
		NextToken();
		// EMIT
		return true; break;
	case TK_SUB_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_LEFT_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_RIGHT_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_AND_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_XOR_ASSIGN:
		NextToken();
		// EMIT
		return true;
	case TK_OR_ASSIGN:
		NextToken();
		// EMIT
		return true;
	}

	return false;
}

void Parser::PrimaryExpression()
{
	switch (CURRENT_TOKEN())
	{
	case TK_IDENTIFIER:
		NextToken();
		// EMIT
		break;

	case TK_CONSTANT:
		switch (CURRENT_DATA()->DataType()) {
		case Value::TypeSpecifier::T_VOID:
			// EMIT
			break;
		case Value::TypeSpecifier::T_INT:
			// EMIT
			break;
		case Value::TypeSpecifier::T_SHORT:
			// EMIT
			break;
		case Value::TypeSpecifier::T_LONG:
			// EMIT
			break;
		case Value::TypeSpecifier::T_BOOL:
			// EMIT
			break;

		case Value::TypeSpecifier::T_FLOAT:
			//std::cout << " = " << m_currentData->As<float>();
			// EMIT
			break;

		case Value::TypeSpecifier::T_DOUBLE:
			break;

		case Value::TypeSpecifier::T_CHAR:
			if (CURRENT_DATA()->IsArray()) {
				//std::cout << " = " << m_currentData->As<std::string>();
				// EMIT
			}
			else {
				//std::cout << " = " << m_currentData->As<char>();
				// EMIT
			}
			break;
		}
		NextToken();
		break;

	case TK_PARENTHESE_OPEN:
		NextToken();
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);
	}
}

void Parser::ArgumentExpressionList()
{
	do {
		AssignmentExpression();
	} while (MATCH_TOKEN(TK_COMMA));
}

void Parser::PostfixExpression()
{
	// Unknown flow
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

	switch (CURRENT_TOKEN())
	{
	case TK_BRACKET_OPEN:
		NextToken();
		Expression();
		ExpectToken(TK_BRACKET_CLOSE);
		break;
	case TK_PARENTHESE_OPEN:
		NextToken();
		if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
			NextToken();
			// EMIT
		}
		else {
			ArgumentExpressionList();
			ExpectToken(TK_PARENTHESE_CLOSE);
		}
		break;
	case TK_DOT:
		NextToken();
		// EMIT
		ExpectIdentifier();
		break;
	case TK_PTR_OP:
		NextToken();
		// EMIT
		ExpectIdentifier();
		break;
	case TK_INC_OP:
		NextToken();
		// EMIT
		break;
	case TK_DEC_OP:
		NextToken();
		// EMIT
		break;
	}
}

void Parser::UnaryExpression()
{
	switch (CURRENT_TOKEN())
	{
	case TK_INC_OP:
		NextToken();
		// EMIT
		UnaryExpression();
		break;
	case TK_DEC_OP:
		NextToken();
		// EMIT
		UnaryExpression();
		break;
	case TK_SIZEOF:
		NextToken();
		if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
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
	bool cont = true;
	if (MATCH_TOKEN(TK_PARENTHESE_OPEN)) {
		m_comm.Snapshot();
		try {
			NextToken();
			TypeName();
			ExpectToken(TK_PARENTHESE_CLOSE);
			CastExpression();
		}
		catch (UnexpectedTokenException e)
		{
			m_comm.Revert();
		}
		//if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) { //TMP
		//	cont = false;
		//}
		//else {
		//}
	}

	if (cont) {
		UnaryExpression();
	}
}

void Parser::MultiplicativeExpression()
{
	CastExpression();

	switch (CURRENT_TOKEN())
	{
	case TK_ASTERISK:
		NextToken();
		// EMIT
		CastExpression();
		break;
	case TK_SLASH:
		NextToken();
		// EMIT
		CastExpression();
		break;
	case TK_PERCENT:
		NextToken();
		// EMIT
		CastExpression();
		break;
	}
}

void Parser::AdditiveExpression()
{
	MultiplicativeExpression();

	switch (CURRENT_TOKEN()) {
	case TK_PLUS:
		NextToken();
		// EMIT
		MultiplicativeExpression();
		break;
	case TK_MINUS:
		NextToken();
		// EMIT
		MultiplicativeExpression();
		break;
	}
}

void Parser::ShiftExpression()
{
	AdditiveExpression();

	switch (CURRENT_TOKEN()) {
	case TK_LEFT_OP:
		NextToken();
		// EMIT
		AdditiveExpression();
		break;
	case TK_RIGHT_OP:
		NextToken();
		// EMIT
		AdditiveExpression();
		break;
	}
}

void Parser::RelationalExpression()
{
	ShiftExpression();

	switch (CURRENT_TOKEN())
	{
	case TK_LESS_THAN:
		NextToken();
		// EMIT
		ShiftExpression();
		break;
	case TK_GREATER_THAN:
		NextToken();
		// EMIT
		ShiftExpression();
		break;
	case TK_LE_OP:
		NextToken();
		// EMIT
		ShiftExpression();
		break;
	case TK_GE_OP:
		NextToken();
		// EMIT
		ShiftExpression();
		break;
	}
}

void Parser::EqualityExpression()
{
	RelationalExpression();

	switch (CURRENT_TOKEN())
	{
	case TK_EQ_OP:
		NextToken();
		// EMIT
		RelationalExpression();
		break;
	case TK_NE_OP:
		NextToken();
		// EMIT
		RelationalExpression();
		break;
	}
}

void Parser::AndExpression()
{
	EqualityExpression();

	if (MATCH_TOKEN(TK_AMPERSAND)) {
		NextToken();
		EqualityExpression();
	}
}

void Parser::ExclusiveOrExpression()
{
	AndExpression();

	if (MATCH_TOKEN(TK_CARET)) {
		NextToken();
		AndExpression();
	}
}

void Parser::LogicalAndExpression()
{
	ExclusiveOrExpression();

	if (MATCH_TOKEN(TK_AND_OP)) {
		NextToken();
		// EMIT
		ExclusiveOrExpression();
	}
}

void Parser::LogicalOrExpression()
{
	LogicalAndExpression();

	if (MATCH_TOKEN(TK_OR_OP)) {
		NextToken();
		// EMIT
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
	if (AssignmentOperator()) {
		AssignmentExpression();
	}
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
void Parser::JumpStatement()
{
	switch (CURRENT_TOKEN())
	{
	case TK_GOTO:
		NextToken();
		ExpectIdentifier();
		// EMIT
		break;
	case TK_CONTINUE:
		NextToken();
		// EMIT
		break;
	case TK_BREAK:
		NextToken();
		// EMIT
		break;
	case TK_RETURN:
		NextToken();
		if (MATCH_TOKEN(TK_COMMIT)) {
			m_elementStack.push(std::move(std::make_unique<ValueNode>()));
		}
		else {
			Expression();
		}
		break;
	default: // Return if no match
		return;
	}

	ExpectToken(TK_COMMIT);
}

// For, do and while loop
void Parser::IterationStatement()
{
	switch (CURRENT_TOKEN())
	{
	case TK_WHILE:
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		break;
	case TK_DO:
		Statement();
		ExpectToken(TK_WHILE);
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		ExpectToken(TK_COMMIT);
		break;
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
		break;
	}
}

// If and switch statements
void Parser::SelectionStatement()
{
	switch (CURRENT_TOKEN())
	{
	case TK_IF:
		NextToken();
		ExpectToken(TK_PARENTHESE_OPEN);
		Expression();
		ExpectToken(TK_PARENTHESE_CLOSE);
		Statement();
		if (MATCH_TOKEN(TK_ELSE)) {
			Statement();
		}
		break;
	case TK_SWITCH:
		NextToken();
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		break;
	}
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
			BlockItems();
			ExpectToken(TK_BRACE_CLOSE);
		}

		return true;
	}

	return false;
}

// Labeled statements
void Parser::LabeledStatement()
{
	switch (CURRENT_TOKEN())
	{
	case TK_IDENTIFIER:
		NextToken();
		if (MATCH_TOKEN(TK_COLON)) {
			NextToken();
			Statement();
		}
		//ExpectToken(TK_COLON);//TODO: remove
		//Statement();
		break;
	case TK_CASE:
		NextToken();
		ConstantExpression();
		ExpectToken(TK_COLON);
		Statement();
		break;
	case TK_DEFAULT:
		NextToken();
		ExpectToken(TK_COLON);
		Statement();
		break;
	}
}

void Parser::Statement()
{
	CompoundStatement();
	SelectionStatement();
	IterationStatement();
	JumpStatement();
	ExpressionStatement();
	LabeledStatement();
}

void Parser::BlockItems()
{
	do {
		Statement();//TMP
		if (MATCH_TOKEN(TK_BRACE_CLOSE)) {
			break;
		}
		Declaration();//TMP
	} while (NOT_TOKEN(TK_BRACE_CLOSE));
}

void Parser::Declaration()
{
	DeclarationSpecifiers();

	if (MATCH_TOKEN(TK_COMMIT)) {
		NextToken();
	}
	else {
		InitDeclaratorList();
		ExpectToken(TK_COMMIT);
	}
}

void Parser::InitDeclaratorList()
{
	do {
		Declarator();

		if (MATCH_TOKEN(TK_ASSIGN)) {
			NextToken();
			Initializer();
		}
	} while (MATCH_TOKEN(TK_COMMA));
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
		switch (CURRENT_TOKEN())
		{
		case TK_PARENTHESE_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();
				// EMIT
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
				// EMIT
				cont = true;
			}
			else if (MATCH_TOKEN(TK_ASTERISK)) {
				NextToken();
				ExpectToken(TK_BRACKET_CLOSE);
				// EMIT
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
			InitializerList();
		} while (MATCH_TOKEN(TK_COMMA));
		ExpectToken(TK_BRACE_CLOSE);
	}
	else {
		AssignmentExpression();
	}
}

void Parser::InitializerList()
{
	do {
		Designation();
		Initializer();
	} while (MATCH_TOKEN(TK_COMMA));
}

void Parser::Designation()
{
	Designators();
	ExpectToken(TK_ASSIGN);//TODO
}

void Parser::Designators()
{
	bool cont = false;
	do {
		switch (CURRENT_TOKEN())
		{
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
	if (MATCH_TOKEN(TK_ASTERISK)) {
		NextToken();
		// type_qualifier_list
		Pointer();
	}
}

bool Parser::Declarator()
{
	Pointer();

	return DirectDeclarator();
}

bool Parser::DirectDeclarator()
{
	if (MATCH_TOKEN(TK_IDENTIFIER)) {
		NextToken();
		// EMIT
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
		switch (CURRENT_TOKEN())
		{
		case TK_BRACKET_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_BRACKET_CLOSE)) {
				NextToken();
				// EMIT
				return true;
			}
			else if (MATCH_TOKEN(TK_ASTERISK)) {
				NextToken();
				// EMIT
				ExpectToken(TK_BRACE_CLOSE);
				return true;
			}

			break;
		case TK_PARENTHESE_OPEN:
			NextToken();
			if (MATCH_TOKEN(TK_PARENTHESE_CLOSE)) {
				NextToken();
				// EMIT
				return true;
			}
			else {
				if (!ParameterTypeList()) {
					do {
						if (MATCH_TOKEN(TK_IDENTIFIER)) {
							NextToken();
							// EMIT
						}
						else {
							return false;//TMP
						}
					} while (MATCH_TOKEN(TK_COMMA));
				}
				ExpectToken(TK_PARENTHESE_CLOSE);
				return true;
			}
			break;
		default:
			return false;
		}

		//'[' type_qualifier_list AssignmentExpression(); ']'
		//'[' type_qualifier_list ']'
		//'[' assignment_expression ']'
		//'[' STATIC type_qualifier_list AssignmentExpression(); ']'
		//'[' type_qualifier_list STATIC AssignmentExpression(); ']'
		//'[' type_qualifier_list '*' ']'
	}
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

	while (Declarator());

	return CompoundStatement();

	//auto localFunc = new FunctionNode(m_currentData);
	//localFunc->ReturnType(std::move(m_elementStack.top()));
	//m_elementStack.pop();
	//stree.PushNode(std::move(std::unique_ptr<ASTNode>{ localFunc }));
}

// Try as function; if that fails assume declaration
void Parser::ExternalDeclaration()
{
	if (!FunctionDefinition()) {
		Declaration();
	}
}

void Parser::TranslationUnit()
{
	// For each translation unit run the parser
	//for (size_t i = 0; i < length; i++)
	//{
	ExternalDeclaration();
	//}
}

void Parser::Execute()
{
	NextToken();

	do {
		TranslationUnit();
	} while (!lex.IsDone());
}
