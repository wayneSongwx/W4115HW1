#include "lexer.h"
#include "exception.h"
#include <cctype>
#include <sstream>
#include <unordered_set>

Lexer::Lexer(const char *filename) : in(filename) {
    if (!in) {
        throw std::runtime_error("Failed to open file");
    }
}

Lexer::~Lexer() {
    in.close();
}

Token Lexer::getNextToken() {
    // fill code here
    return Token(Token::EndOfFile, "", this->line, this->col);
}

void Lexer::printTokens() {
    for (const auto& token : tokens) {
        token.print();
    }
}
