#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <stdexcept>
#include <string>

class LexerException : public std::runtime_error {
public:
    LexerException(int line, int col) : std::runtime_error("Lexer Error at line " + std::to_string(line) +
                                  ", column " + std::to_string(col)) {}
};
#endif /* EXCEPTION_H */