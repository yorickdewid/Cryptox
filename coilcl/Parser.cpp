#include "Parser.h"

#include <iostream>

Parser::Parser(const std::string& input)
	: lex{ input }
{
	lex.ErrorHandler([](const std::string& err, char token, int line, int column) {
		std::cerr << "Syntax error: " << err << " at " << line << ":" << column << std::endl;
	});
}

void Parser::Error(const std::string& err)
{
	int line = 0;
	int column = 1;

	std::cerr << "Semantic error: " << err << " before '" << m_currentToken << "' token at " << line << ":" << column << std::endl;
}

void Parser::ExpectToken(Token token)
{
	if (m_currentToken != token) {
		Error("expected expression");
	}

	NextToken();
}

void Parser::ExpectIdentifier()
{
	if (m_currentToken != TK_IDENTIFIER) {
		Error("expected identifier");
	}

	assert(m_currentData.get() != nullptr);

	NextToken();
}

auto Parser::StorageClassSpecifier()
{
	switch (m_currentToken) {
	case TK_TM_REGISTER:
		return Value::StorageClassSpecifier::REGISTER;
	case TK_TM_STATIC:
		return Value::StorageClassSpecifier::STATIC;
	case TK_TM_TYPEDEF:
		return Value::StorageClassSpecifier::TYPEDEF;
	default:
		break;
	}

	return Value::StorageClassSpecifier::NONE;
}

std::unique_ptr<Value> Parser::TypeSpecifier()
{
	switch (m_currentToken) {
	case TK_TM_CHAR:
		return std::move(std::make_unique<ValueObject<std::string>>(Value::TypeSpecifier::T_CHAR));
	case TK_TM_INT:
		return std::move(std::make_unique<ValueObject<int>>(Value::TypeSpecifier::T_INT));
	case TK_TM_FLOAT:
		return std::move(std::make_unique<ValueObject<float>>(Value::TypeSpecifier::T_FLOAT));
	case TK_TM_DOUBLE:
		return std::move(std::make_unique<ValueObject<double>>(Value::TypeSpecifier::T_DOUBLE));
	case TK_TM_SIGNED:
		return std::move(std::make_unique<ValueObject<signed>>(Value::TypeSpecifier::T_INT));
	case TK_TM_UNSIGNED:
		return std::move(std::make_unique<ValueObject<unsigned>>(Value::TypeSpecifier::T_INT));
	default:
		break;
	}

	return nullptr;
}

auto Parser::TypeQualifier()
{
	switch (m_currentToken) {
	case TK_TM_CONST:
		return Value::TypeQualifier::CONST;
	case TK_TM_VOLATILE:
		return Value::TypeQualifier::VOLATILE;
	default:
		break;
	}

	return Value::TypeQualifier::NONE;
}

bool Parser::DeclarationSpecifier()
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

	//if (_type == nullptr) {
	//	throw something
	//}

	if (tmpSCP != Value::StorageClassSpecifier::NONE) {
		_type->StorageClass(tmpSCP);
	}
	if (tmpTQ != Value::TypeQualifier::NONE) {
		_type->Qualifier(tmpTQ);
	}

	m_elementStack.push(std::move(std::make_unique<ValueNode>(_type)));
	return true;
}

bool Parser::UnaryOperator()
{
	switch (m_currentToken)
	{
	case TK_AND:
		NextToken();
		return true;
		//case '&':
		//	return true;
		//case '+':
		//	return true;
		//case '-':
		//	return true;
	case TK_TILDE:
		NextToken();
		return true;
		//case '!':
		//	break;
	default:
		break;
	}

	return false;
}

void Parser::PrimaryExpression()
{
	switch (m_currentToken)
	{
	case TK_IDENTIFIER:
		//TODO
		break;

	case TK_INTEGER:
		assert(m_currentData->DataType() == Value::TypeSpecifier::T_INT);
		m_elementStack.push(std::move(std::make_unique<ValueNode>(m_currentData)));
		NextToken();
		break;
	case TK_STRING_LITERAL:
		assert(m_currentData->DataType() == Value::TypeSpecifier::T_CHAR);
		m_elementStack.push(std::move(std::make_unique<ValueNode>(m_currentData)));
		NextToken();
		break;
	case TK_CHARACTER:
		assert(m_currentData->DataType() == Value::TypeSpecifier::T_CHAR);
		m_elementStack.push(std::move(std::make_unique<ValueNode>(m_currentData)));
		NextToken();
		break;
	case TK_FLOAT:
		assert(m_currentData->DataType() == Value::TypeSpecifier::T_FLOAT);
		m_elementStack.push(std::move(std::make_unique<ValueNode>(m_currentData)));
		NextToken();
		break;
	case TK_DOUBLE:
		assert(m_currentData->DataType() == Value::TypeSpecifier::T_DOUBLE);
		m_elementStack.push(std::move(std::make_unique<ValueNode>(m_currentData)));
		NextToken();
		break;
	case TK_PARENTHES_OPEN:
		Expression();
		ExpectToken(TK_PARENTHES_CLOSE);
	}
}

void Parser::PostfixExpression()
{
	PrimaryExpression();

	//PostfixExpression() '[' expression ']'
	//
	//PostfixExpression();
	//ExpectToken(TK_PARENTHES_OPEN);
	//ExpectToken(TK_PARENTHES_CLOSE);
	//
	//PostfixExpression() '(' argument_expression_list ')'
	//
	//PostfixExpression() '.' IDENTIFIER
	//
	//PostfixExpression() PTR_OP IDENTIFIER
	//
	//PostfixExpression() INC_OP
	//
	//PostfixExpression() DEC_OP
	//
	//'(' type_name ')' '{' initializer_list '}'
	//
	//'(' type_name ')' '{' initializer_list ',' '}'
}

void Parser::UnaryExpression()
{
	switch (m_currentToken)
	{
	case TK_INCR:
		//TODO: save
		UnaryExpression();
		break;
	case TK_DECR:
		//TODO: save
		UnaryExpression();
		break;
	case TK_SIZEOF:
		NextToken();
		if (m_currentToken == TK_PARENTHES_OPEN) {
			// <type_name>
			ExpectToken(TK_PARENTHES_CLOSE);
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
	if (m_currentToken == TK_PARENTHES_OPEN) {
		// <type_name>
		ExpectToken(TK_PARENTHES_CLOSE);
		CastExpression();
	}
	else {
		UnaryExpression();
	}
}

void Parser::MultiplicativeExpression()
{
	CastExpression();
	//
	//MultiplicativeExpression() '*' CastExpression()
	//
	//MultiplicativeExpression() '/' CastExpression()
	//
	//MultiplicativeExpression() '%' CastExpression()
}

void Parser::AdditiveExpression()
{
	MultiplicativeExpression();
	//
	//AdditiveExpression() '+' MultiplicativeExpression()
	//
	//AdditiveExpression() '-' MultiplicativeExpression()
}

void Parser::ShiftExpression()
{
	AdditiveExpression();
	//
	//ShiftExpression() LEFT_OP AdditiveExpression()
	//
	//ShiftExpression() RIGHT_OP AdditiveExpression()
}

void Parser::RelationalExpression()
{
	ShiftExpression();
	//
	//RelationalExpression() '<' ShiftExpression()
	//
	//RelationalExpression() '>' ShiftExpression()
	//
	//RelationalExpression() LE_OP ShiftExpression()
	//
	//RelationalExpression() GE_OP ShiftExpression()
}

void Parser::EqualityExpression()
{
	RelationalExpression();
	//
	//EqualityExpression() EQ_OP RelationalExpression()
	//
	//EqualityExpression() NE_OP RelationalExpression()
}

void Parser::AndExpression()
{
	EqualityExpression();
	//
	// AndExpression() '&' EqualityExpression()
}

void Parser::ExclusiveOrExpression()
{
	AndExpression();
	//
	// ExclusiveOrExpression() '^' AndExpression()
}

void Parser::LogicalAndExpression()
{
	ExclusiveOrExpression();
	//
	//LogicalAndExpression() AND_OP ExclusiveOrExpression()
}

//XXX
void Parser::LogicalOrExpression()
{
	LogicalAndExpression();
	//
	//LogicalOrExpression() OR_OP LogicalAndExpression()
}

//XXX
void Parser::ConditionalExpression()
{
	LogicalOrExpression();
	//
	//LogicalOrExpression() '?' expression ':' conditional_expression
}

//XXX; 
void Parser::AssignmentExpression()
{
	ConditionalExpression();
	//
	// UnaryExpression() assignment_operator AssignmentExpression()
}

void Parser::Expression()
{
	AssignmentExpression();

	// i = 0, r = 2;
	Expression();
	ExpectToken(TK_COMMA);
	AssignmentExpression();
}

void Parser::JumpStatement()
{
	switch (m_currentToken)
	{
		//case TK_GOTO:
		//	ExpectToken(TK_IDENTIFIER);
	case TK_CONTINUE:
		ExpectToken(TK_COMMIT);
		break;
	case TK_BREAK:
		ExpectToken(TK_COMMIT);
		break;
	case TK_RETURN:
		NextToken();
		if (m_currentToken == TK_COMMIT) {
			m_elementStack.push(std::move(std::make_unique<ValueNode>()));
		}
		else {
			Expression();
		}

		ExpectToken(TK_COMMIT);
		break;
	}
}

// For, do and while loop
void Parser::IterationStatement()
{
	switch (m_currentToken)
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
	switch (m_currentToken)
	{
	case TK_IF:
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		if (m_currentToken == TK_ELSE) {
			Statement();
		}
		break;
	case TK_SWITCH:
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
		Statement();
		break;
	default:
		break;
	}
}

void Parser::ExpressionStatement()
{
	if (m_currentToken == TK_COMMIT) {
		// Useless statement
	}
	else {
		Expression();
	}
}

// Compound statements contain code block
void Parser::CompoundStatement()
{
	if (m_currentToken == TK_BRACE_OPEN) {
		if (m_currentToken == TK_BRACE_CLOSE) {
			// Useless statement
		}
		else {
			//BlockItemList();
			ExpectToken(TK_BRACE_CLOSE);
		}
	}
}

// Labeled statements
void Parser::LabeledStatement()
{
	switch (m_currentToken)
	{
	case TK_IDENTIFIER:
		//ExpectToken(':');
		Statement();
		break;
	case TK_CASE:
		//ConstantExpression();
		//ExpectToken(':');
		Statement();
		break;
		//case TK_DEFAULT:
		//	ExpectToken(':');
		//	Statement();
		//	break;
	default:
		break;
	}
}

void Parser::Statement()
{
	while (m_currentToken != TK_BRACE_CLOSE) {
		LabeledStatement();
		//
		CompoundStatement();
		//
		ExpressionStatement();
		//
		SelectionStatement();
		//
		IterationStatement();
		//
		JumpStatement();
	}
}

void Parser::FuncDef()
{
	auto localFunc = new FunctionNode(m_currentData);

	localFunc->ReturnType(std::move(m_elementStack.top()));
	m_elementStack.pop();

	ExpectToken(TK_PARENTHES_OPEN);

	//TODO: params

	ExpectToken(TK_PARENTHES_CLOSE);

	// Function inner declaration
	ExpectToken(TK_BRACE_OPEN);
	Statement();
	ExpectToken(TK_BRACE_CLOSE);//TODO check

								// <compound-statement> ?

								//stree.PushNode(std::move(std::unique_ptr<ASTNode>{ localFunc }));
}

void Parser::TranslationUnit()
{
	// Return type for function declaration
	DeclarationSpecifier();

	// Function name as identifier
	ExpectIdentifier();

	if (m_currentToken == TK_PARENTHES_OPEN) {
		FuncDef();
	}


	// <declaration>
}

void Parser::Execute()
{
	NextToken();

	// For each translation unit run the parser
	/*for (size_t i = 0; i < length; i++)
	{

	}*/

	do {
		TranslationUnit();
	} while (!lex.IsDone());
}

/*while (!lex.IsDone()) {
	auto token = lex.Lex();
	std::cout << "Token: " << Keyword{ token }.Print();

	if (lex.HasData()) {
		auto val = lex.Data();
		switch (val->DataType()) {

		case Value::TypeSpecifier::T_FLOAT:
			std::cout << " = " << val->As<float>();
			break;

		case Value::TypeSpecifier::T_CHAR:
			if (val->IsArray()) {
				std::cout << " = " << val->As<std::string>();
			} else {
				std::cout << " = " << val->As<char>();
			}
			break;

		default:
			std::cout << " = " << val->As<int>();
			break;
		}
	}

	std::cout << std::endl;
}*/
