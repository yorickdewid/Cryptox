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

	assert(TokenHasData());

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

//XXX
void AssignmentExpression()
{
	// conditional_expression
	//
	// unary_expression assignment_operator assignment_expression
}

void Parser::Expression()
{
	//AssignmentExpression();
	//
	//Expression();
	//ExpectToken(TK_COMMA);
	//AssignmentExpression();
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
	Declaration();
	ExpectToken(TK_BRACE_CLOSE);//TODO check

								// <compound-statement> ?

   //stree.PushNode(std::move(std::unique_ptr<ASTNode>{ localFunc }));
}

void Parser::Declaration()
{
	while (m_currentToken != TK_RETURN) {
		NextToken();
	}

	if (m_currentToken == TK_RETURN) {
		NextToken();
		if (m_currentToken == TK_COMMIT) {
			m_elementStack.push(std::move(std::make_unique<ValueNode>([]() -> Value* {
				return new ValueObject<void>(Value::TypeSpecifier::T_VOID);
			})));
		}
		else {
			Expression();
		}
	}
	//ExpectToken(TK_RETURN);
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

	do {
		TranslationUnit();
	} while (!lex.IsDone());

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
}