#include "Lexer.h"

Token::Token(TokenType type, const char* startPtr, uint32_t length, uint32_t currentLine, uint32_t currentColumn):
	type(type), startPtr(startPtr), length(length), currentLine(currentLine), currentColumn(currentColumn) {}

Lexer::Lexer(const char* data, size_t len)
{

	if (!data) throw std::logic_error("Invalid pointer data");

	beginPtr = data;
	currentPtr = data;
	endPtr = data + len;

}

char Lexer::process()
{

	if (currentPtr >= endPtr)  return '\0';

	char result = *currentPtr;
	currentPtr += 1;

	if (result == '\n')
	{

		currentLine += 1;
		currentColumn = 1;

	}
	else
	{

		currentColumn += 1;

	}

	return result;

}

bool Lexer::match(char expectedSymbol)
{

	if (peek() != expectedSymbol) return false;

	process();
	return true;

}

Token Lexer::generateToken(TokenType type, const char* ptr)
{

	if (!ptr) throw std::logic_error("Error");
	return Token(type, ptr, (uint32_t)(currentPtr - ptr), currentLine, currentColumn);

}

void Lexer::trim()
{

	while (true)
	{

		char currentSymbol = peek();

		switch (currentSymbol)
		{

		case ' ': case '\t': case '\r': case '\n': process(); continue;
		case '/':

			if (peekNext() == '/')
			{
				while (peek() != '\n' && peek() != '\0') process();
				continue;
			}

			break;

		default: break;

		}

		break;

	}

}

Token Lexer::nextToken()
{

	trim();

	const char* tokenStart = currentPtr;
	char currentSymbol = process();

	switch (currentSymbol)
	{

	case '\0':return { TokenType::END_OF_FILE, currentPtr, 0, currentLine, currentColumn };
	case '+': return generateToken(TokenType::PLUS, tokenStart);
	case '-': return generateToken(TokenType::MINUS, tokenStart);
	case '*': return generateToken(TokenType::STAR, tokenStart);
	case '/': return generateToken(TokenType::SLASH, tokenStart);
	case '(': return generateToken(TokenType::LPAREN, tokenStart);
	case ')': return generateToken(TokenType::RPAREN, tokenStart);
	case '[': return generateToken(TokenType::LBRACKET, tokenStart);
	case ']': return generateToken(TokenType::RBRACKET, tokenStart);
	case '{': return generateToken(TokenType::LBRACE, tokenStart);
	case '}': return generateToken(TokenType::RBRACE, tokenStart);
	case ';': return generateToken(TokenType::SEMICOLON, tokenStart);
	case ',': return generateToken(TokenType::COMMA, tokenStart);
	case '!': return generateToken(match('=') ? TokenType::NOT_EQUAL: TokenType::NEGATION, tokenStart);
	case '=': return generateToken(match('=') ? TokenType::EQUAL_EQUAL : TokenType::ASSIGN, tokenStart);
	case '<': return generateToken(match('=') ? TokenType::LESS_EQUAL : TokenType::LESS, tokenStart);
	case '>': return generateToken(match('=') ? TokenType::GREATER_EQUAL : TokenType::GREATER, tokenStart);
	case '&': return generateToken(match('&') ? TokenType::LOGICAL_AND: TokenType::END_OF_FILE, tokenStart);
	case '|': return generateToken(match('|') ? TokenType::LOGICAL_OR: TokenType::END_OF_FILE, tokenStart);
	case ':': return generateToken(TokenType::COLON, tokenStart);
	case '\'': return character(tokenStart);
	default:

		if (isAlpha(currentSymbol) || currentSymbol == '_') return identifier(tokenStart);
		if (isDigit(currentSymbol)) return number(tokenStart);
		throw std::runtime_error("Unexpected character in lexer");

	}

}

Token Lexer::identifier(const char* ptr)
{

	while (isAlnum(peek()) || peek() == '_') process();
	std::string variableId(ptr, currentPtr - ptr);
	return generateToken(keywordMatcher(variableId), ptr);

}

Token Lexer::number(const char* ptr)
{

	bool isFloat = false;

	while (isDigit(peek())) process();

	if (peek() == '.' && isDigit(peekNext()))
	{

		isFloat = true;
		process();
		while (isDigit(peek())) process();

	}

	if (isFloat) return generateToken(TokenType::FLOAT, ptr);
	else return generateToken(TokenType::INT, ptr);

}

Token Lexer::character(const char* ptr)
{

	if (peek() == '\\\\') { process(); process(); }
	else process();

	if (peek() != '\'') throw std::runtime_error("unterminated char literal");
	process();

	return generateToken(TokenType::CHAR_LITERAL, ptr);

}

bool Lexer::isAlpha(char ch)
{

	return (ch >= 'a' && ch <= 'z')
		|| (ch >= 'A' && ch <= 'Z');

}

bool Lexer::isAlnum(char ch)
{

	return isAlpha(ch) || isDigit(ch);

}

bool Lexer::isDigit(char ch)
{

	return ch >= '0' && ch <= '9';

}

TokenType Lexer::keywordMatcher(const std::string& variableId)
{

	if (hashedKeywords.find(variableId) != hashedKeywords.end()) return hashedKeywords[variableId];
	return TokenType::IDENTIFIER;

}

char Lexer::peek()
{

	if (currentPtr < endPtr) return *currentPtr;
	else return '\0';

}

char Lexer::peekNext()
{

	if (currentPtr + 1 < endPtr) return *(currentPtr + 1);
	else return '\0';

}

void Lexer::tokenize()
{

	tokens.reserve((endPtr - beginPtr) / 2);

	for (;;)
	{

		Token currentToken = nextToken();
		tokens.push_back(currentToken);

		if (currentToken.type == TokenType::END_OF_FILE) break;

	}

}
