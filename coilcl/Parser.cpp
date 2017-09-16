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
	assert(0);
	//TODO: throw something
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
	case TK_REGISTER:
		return Value::StorageClassSpecifier::REGISTER;
	case TK_STATIC:
		return Value::StorageClassSpecifier::STATIC;
	case TK_TYPEDEF:
		return Value::StorageClassSpecifier::TYPEDEF;
	default:
		break;
	}

	return Value::StorageClassSpecifier::NONE;
}

std::unique_ptr<Value> Parser::TypeSpecifier()
{
	/*if (m_currentToken != TK_CONSTANT) {
		return nullptr;
	}

	switch (m_currentData->DataType()) {
	case Value::TypeSpecifier::T_FLOAT:
		std::cout << " = " << m_currentData->As<float>();
		break;

	case Value::TypeSpecifier::T_CHAR:
		if (m_currentData->IsArray()) {
			std::cout << " = " << m_currentData->As<std::string>();
		}
		else {
			std::cout << " = " << m_currentData->As<char>();
		}
		break;

	default:
		std::cout << " = " << m_currentData->As<int>();
		break;
	}*/

	// TODO:
	// - Complex
	// - Imaginary
	// - struct_or_union_specifier
	// - enum_specifier
	// - TYPE_NAME
	switch (m_currentToken) {
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
	default:
		break;
	}

	return nullptr;
}

auto Parser::TypeQualifier()
{
	switch (m_currentToken) {
	case TK_CONST:
		return Value::TypeQualifier::CONST;
	case TK_VOLATILE:
		return Value::TypeQualifier::VOLATILE;
	default:
		break;
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

bool Parser::UnaryOperator()
{
	switch (m_currentToken)
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
	switch (m_currentToken)
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
	switch (m_currentToken)
	{
	case TK_IDENTIFIER:
		NextToken();
		// EMIT
		break;

	case TK_CONSTANT:
		NextToken();
		// EMIT
		break;

	case TK_PARENTHES_OPEN:
		Expression();
		ExpectToken(TK_PARENTHES_CLOSE);
	}
}

void Parser::PostfixExpression()
{
	PrimaryExpression();

	switch (m_currentToken)
	{
	case TK_BRACKET_OPEN:
		NextToken();
		Expression();
		ExpectToken(TK_BRACKET_CLOSE);
		break;
	case TK_PARENTHES_OPEN:
		NextToken();
		if (m_currentToken == TK_PARENTHES_CLOSE) {
			NextToken();
			// EMIT
		}
		else {
			// argument_expression_list
			ExpectToken(TK_PARENTHES_CLOSE);
		}
		break;
	case TK_DOT:
		NextToken();
		ExpectIdentifier();
		break;
	case TK_PTR_OP:
		NextToken();
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

	//'(' type_name ')' '{' initializer_list '}'
	//
	//'(' type_name ')' '{' initializer_list ',' '}'
}

void Parser::UnaryExpression()
{
	switch (m_currentToken)
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

	switch (m_currentToken)
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

	switch (m_currentToken) {
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

	switch (m_currentToken) {
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

	switch (m_currentToken)
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

	switch (m_currentToken)
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

	if (m_currentToken == TK_AMPERSAND) {
		NextToken();
		EqualityExpression();
	}
}

void Parser::ExclusiveOrExpression()
{
	AndExpression();

	if (m_currentToken == TK_CARET) {
		NextToken();
		AndExpression();
	}
}

void Parser::LogicalAndExpression()
{
	ExclusiveOrExpression();

	if (m_currentToken == TK_AND_OP) {
		NextToken();
		// EMIT
		ExclusiveOrExpression();
	}
}

void Parser::LogicalOrExpression()
{
	LogicalAndExpression();

	if (m_currentToken == TK_OR_OP) {
		NextToken();
		// EMIT
		LogicalAndExpression();
	}
}

void Parser::ConditionalExpression()
{
	LogicalOrExpression();

	if (m_currentToken == TK_QUESTION_MARK) {
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
	} while (m_currentToken == TK_COMMA);
}

void Parser::ConstantExpression()
{
	ConditionalExpression();
}

// Labels and gotos
void Parser::JumpStatement()
{
	switch (m_currentToken)
	{
	case TK_GOTO:
		NextToken();
		ExpectToken(TK_IDENTIFIER);
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
		if (m_currentToken == TK_COMMIT) {
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
		NextToken();
		ExpectToken(TK_PARENTHES_OPEN);
		Expression();
		ExpectToken(TK_PARENTHES_CLOSE);
		Statement();
		if (m_currentToken == TK_ELSE) {
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
	if (m_currentToken == TK_COMMIT) {
		// EMIT
	}
	else {
		Expression();
	}
}

// Compound statements contain code block
bool Parser::CompoundStatement()
{
	if (m_currentToken == TK_BRACE_OPEN) {
		NextToken();
		if (m_currentToken == TK_BRACE_CLOSE) {
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
	switch (m_currentToken)
	{
	case TK_IDENTIFIER:
		NextToken();
		ExpectToken(TK_COLON);
		Statement();
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
	default:
		break;
	}
}

void Parser::Statement()
{
	LabeledStatement();
	//
	CompoundStatement();
	//
	SelectionStatement();
	//
	IterationStatement();
	//
	JumpStatement();
	//
	ExpressionStatement();
}

void Parser::BlockItems()
{
	do {
		Declaration();
		if (m_currentToken == TK_BRACE_CLOSE) {
			break;
		}
		Statement();
	} while (m_currentToken != TK_BRACE_CLOSE);
}

void Parser::Declaration()
{
	DeclarationSpecifiers();

	if (m_currentToken == TK_COMMIT) {
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
		InitDeclarator();
	} while (m_currentToken == TK_COMMA);
}

void Parser::InitDeclarator()
{
	Declarator();

	if (m_currentToken == TK_ASSIGN) {
		NextToken();
		Initializer();
	}
}

void Parser::Initializer()
{
	if (m_currentToken == TK_BRACE_OPEN) {
		do {
			InitializerList();
		} while (m_currentToken == TK_COMMA);
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
	} while (m_currentToken == TK_COMMA);
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
		switch (m_currentToken)
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
			// EMIT
			cont = true;
			break;
		default:
			break;
		}
	} while (cont);
}

void Parser::Pointer()
{
	if (m_currentToken == TK_ASTERISK) {
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
	if (m_currentToken == TK_IDENTIFIER) {
		NextToken();
		// EMIT
	}
	else if (m_currentToken == TK_PARENTHES_OPEN) {
		NextToken();
		Declarator();
		ExpectToken(TK_PARENTHES_CLOSE);
	}
	else {
		return false;
	}

	// Declarations following an identifier
	for (;;) {
		switch (m_currentToken)
		{
		case TK_BRACKET_OPEN:
			NextToken();
			if (m_currentToken == TK_BRACKET_CLOSE) {
				NextToken();
				// EMIT
				return true;
			}
			/*else if (m_currentToken == TK_POINTER) {
				// EMIT
				ExpectToken(TK_BRACE_CLOSE);
				return;
			}*/

			break;
		case TK_PARENTHES_OPEN:
			NextToken();
			if (m_currentToken == TK_PARENTHES_CLOSE) {
				NextToken();
				// EMIT
				return true;
			}
			else {
				//ParameterTypeList();
				//
				//IdentifierList();
				NextToken();//TODO: temp
				ExpectToken(TK_PARENTHES_CLOSE);
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
		//
		//'(' parameter_type_list ')'
		//'(' identifier_list ')'

	}
}

bool Parser::FunctionDefinition()
{
	// Return type for function declaration
	DeclarationSpecifiers();

	while (Declarator());

	return CompoundStatement();

	//auto localFunc = new FunctionNode(m_currentData);

	/*localFunc->ReturnType(std::move(m_elementStack.top()));
	m_elementStack.pop();*/

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
