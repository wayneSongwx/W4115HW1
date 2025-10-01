#ifndef TOKEN_H
#define TOKEN_H

#include <string>
#include <iostream>

class Token {
public:
    enum Type {
        Identifier, Keyword, Integer, AssignOp, Op, Delimiter, EndOfFile
    };

    Type type;
    std::string text;
    int line;
    int column;

    Token(Type t=EndOfFile, const std::string& txt="", int l=0, int c=0)
        : type(t), text(txt), line(l), column(c) {}

    std::string typeToString() const {
        switch (type) {
            case Identifier: return "Identifier";
            case Keyword: return "Keyword";
            case Integer: return "Integer";
            case AssignOp: return "AssignOp";
            case Op: return "Operator";
            case Delimiter: return "Delimiter";
            case EndOfFile: return "EOF";
            default: return "Unknown";
        }
    }

    void print() const {
        std::cout << typeToString() << "('" << text << "') at " << line << ":" << column << std::endl;
    }
};

#endif
