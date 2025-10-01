#ifndef LEXER_H
/* A lexical scanner */
#define LEXER_H

#include "token.h"
#include <vector>
#include <fstream>

class Lexer {
    std::ifstream in;
    std::vector <Token> tokens;
    int line = 1, col = 0;
public:
    Lexer(const char *filename);
    ~Lexer();
    void printTokens();
    Token getNextToken();
};

#endif /* LEXER_H */