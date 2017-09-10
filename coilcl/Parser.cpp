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

	//if (_type == nullptr) {
	//	throw something
	//}

	if (tmpSCP != Value::StorageClassSpecifier::NONE) {
		_type->StorageClass(tmpSCP);
	}
	if (tmpTQ != Value::TypeQualifier::NONE) {
		_type->Qualifier(tmpTQ);
	}

	if (_type != nullptr) {
		m_elementStack.push(std::move(std::make_unique<ValueNode>(_type)));
		return true;
	}
	return false;
}

bool Parser::UnaryOperator()
{
	switch (m_currentToken)
	{
	case TK_AND_OP:
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
		// EMIT
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
	case TK_INC_OP:
		//TODO: save
		UnaryExpression();
		break;
	case TK_DEC_OP:
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

	/*if (m_currentToken == TK_REFERENCE) { // -> AndExpression() '&' EqualityExpression()
		EqualityExpression();
	}*/
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
	//
	//LogicalAndExpression() AND_OP ExclusiveOrExpression()
}

void Parser::LogicalOrExpression()
{
	LogicalAndExpression();
	//
	//LogicalOrExpression() OR_OP LogicalAndExpression()
}

void Parser::ConditionalExpression()
{
	LogicalOrExpression();

	/*if (m_currentToken == TK_QUESTION_MARK) {
		Expression();
		ExpectToken(TK_COLON);
		ConditionalExpression();
	}*/
}

void Parser::AssignmentExpression()
{
	ConditionalExpression();
	//
	// UnaryExpression() assignment_operator AssignmentExpression()
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

void Parser::JumpStatement()
{
	switch (m_currentToken)
	{
		//case TK_GOTO:
		//	ExpectToken(TK_IDENTIFIER);
		// EMIT
	case TK_CONTINUE:
		// EMIT
		break;
	case TK_BREAK:
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
		ExpectToken(TK_BRACE_OPEN);
		Expression();
		ExpectToken(TK_BRACE_CLOSE);
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
	default:
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
void Parser::CompoundStatement()
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
	}
}

// Labeled statements
void Parser::LabeledStatement()
{
	switch (m_currentToken)
	{
	case TK_IDENTIFIER:
		//ExpectToken(TK_COLON);
		Statement();
		break;
	case TK_CASE:
		ConstantExpression();
		//ExpectToken(TK_COLON);
		Statement();
		break;
		/*case TK_DEFAULT:
			ExpectToken(TK_COLON);
			Statement();
			break;*/
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
			//case TK_DOT:
			//	NextToken();
			//	ExpectToken(TK_IDENTIFIER);
			//	// EMIT
			//  cont = true;
			//	break;
		default:
			break;
		}
	} while (cont);
}

void Pointer()
{
	//ExpectToken(TK_POINTER);

	//type_qualifier_list
	//
	//Pointer();
	//
	//type_qualifier_list Pointer();
}

bool Parser::Declarator()
{
	/*if (<pointer>) {
		Pointer();
	}*/

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
			if (m_currentToken == TK_BRACE_CLOSE) {
				// EMIT
				NextToken();
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
				// EMIT
				NextToken();
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

void Parser::DeclarationList()
{
	for (;;) {
		if (!Declarator()) {
			break;
		}
	}
}

void Parser::FunctionDefinition()
{
	// Return type for function declaration
	DeclarationSpecifiers();

	Declarator();

	DeclarationList();
	//
	CompoundStatement();

	//auto localFunc = new FunctionNode(m_currentData);

	/*localFunc->ReturnType(std::move(m_elementStack.top()));
	m_elementStack.pop();*/

	//stree.PushNode(std::move(std::unique_ptr<ASTNode>{ localFunc }));
}

void Parser::ExternalDeclaration()
{
	FunctionDefinition();
	//
	//Declaration();
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
