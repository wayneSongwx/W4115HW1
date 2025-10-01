#include "lexer.h"
#include "exception.h"
#include <cctype>
#include <sstream>
#include <unordered_set>
#include <unordered_map>

Lexer::Lexer(const char *filename) : in(filename) {
    if (!in) {
        throw std::runtime_error("Failed to open file");
    }
}

Lexer::~Lexer() {
    in.close();
}

int getCharAndAdvance(std::ifstream &in, int &line, int &col) {
    int ch = in.get();
    if (ch == std::char_traits<char>::eof()) return ch;
    if (ch == '\n') { line += 1; col = 0; }
    else { col += 1; }
    return ch;
}

static inline bool is_ident_char(char c) {
    return std::isalpha(static_cast<unsigned char>(c)) || c == '_';
}

Token Lexer::getNextToken() {
    // fill code here
    static const std::unordered_set<std::string> kKeywords = {
        "func","var","let","if","else","while","print","return"
    };

    // skip whitespace and comments
    while (true) {
        int p = in.peek();
        if (p == std::char_traits<char>::eof()) {
            Token eofTok(Token::EndOfFile, "", this->line, this->col);
            tokens.push_back(eofTok);
            return eofTok;
        }

        char c = static_cast<char>(p);

        // whitespace (space, tab, cr, nl)
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            getCharAndAdvance(in, this->line, this->col);
            continue;
        }

        // comment begins with '#': consume until end-of-line or EOF
        if (c == '#') {
            getCharAndAdvance(in, this->line, this->col);
            while (true) {
                int q = in.peek();
                if (q == std::char_traits<char>::eof()) break;
                char d = static_cast<char>(q);
                if (d == '\n') break;
                getCharAndAdvance(in, this->line, this->col);   
            }
            continue;
        }


    }

    // now at the start of a token
    // record starting position before consuming first char
    int startLine = this->line;
    int startCol = this->col + 1; // next char position

    // check if EOF after removing the whitespace/comments
    int p = in.peek();
    if (p == std::char_traits<char>::eof()) {
        Token eofTok(Token::EndOfFile, "", this->line, this->col);
        tokens.push_back(eofTok);
        return eofTok;
    }
    char ch = static_cast<char>(p);
    // Identifiers / Keywords: [a-zA-Z_][a-zA-Z0-9_]*
    if (is_ident_char(ch)) {
        std::string lexeme;
        getCharAndAdvance(in, this->line, this->col); // consume first char
        lexeme.push_back(ch);
        // consume rest of identifier
        while (true) {
            int p = in.peek();
            if (p == std::char_traits<char>::eof()) break;
            char c = static_cast<char>(p);
            getCharAndAdvance(in, this->line, this->col);
            lexeme.push_back(c);
        }
        Token::Type tt = kKeywords.count(lexeme) ? Token::Keyword : Token::Identifier;
        Token tok(tt, lexeme, startLine, startCol);
        tokens.push_back(tok);
        return tok;
    }
    // Integer literal: [0-9]+
    if (std::isdigit(static_cast<unsigned char>(ch))) {
        std::string lexeme;
        getCharAndAdvance(in, this->line, this->col); // consume first digit
        lexeme.push_back(ch);
        // consume rest of digits
        while (true) {
            int p = in.peek();
            if (p == std::char_traits<char>::eof()) break;
            char c = static_cast<char>(p);
            if (!std::isdigit(static_cast<unsigned char>(c))) break;
            getCharAndAdvance(in, this->line, this->col);
            lexeme.push_back(c);
        }
        Token tok(Token::Integer, lexeme, startLine, startCol);
        tokens.push_back(tok);
        return tok;
    }
    // Operators (multi-char first)
    // ==, !=, <=, >=
    if ((ch == '=' || ch == '!' || ch == '<' || ch == '>')) {
        // check for two-char operator
        int first = getCharAndAdvance(in, this->line, this->col); // consume first
        int nextp = in.peek();
        if (nextp != std::char_traits<char>::eof() && static_cast<char>(nextp) == '=') {
            getCharAndAdvance(in, this->line, this->col); // consume '='
            std::string lex;
            lex.push_back(static_cast<char>(first));
            lex.push_back('=');
            Token tok(Token::Op, lex, startLine, startCol);
            tokens.push_back(tok);
            return tok;
        } else {
            // single-char < or > is an operator, but '=' or '!' alone are NOT in spec.
            if (first == '<' || first == '>') {
                std::string lex(1, static_cast<char>(first));
                Token tok(Token::Op, lex, startLine, startCol);
                tokens.push_back(tok);
                return tok;
            }
            // else fall through to unrecognized char
            throw LexerException(startLine, startCol);
        }
    }
    // Assignment ':=' or ':' delimiter
    if (ch == ':') {
        getCharAndAdvance(in, this->line, this->col); // consume ':'
        int nextp = in.peek();
        if (nextp != std::char_traits<char>::eof() && static_cast<char>(nextp) == '=') {
            getCharAndAdvance(in, this->line, this->col); // consume '='
            Token tok(Token::AssignOp, ":=", startLine, startCol);
            tokens.push_back(tok);
            return tok;
        } else {
            Token tok(Token::Delimiter, ":", startLine, startCol);
            tokens.push_back(tok);
            return tok;
        }
    }
    // Remaining single-char operators: + - * /
    if (ch == '+' || ch == '-' || ch == '*' || ch == '/') {
        getCharAndAdvance(in, this->line, this->col);
        std::string lex(1, ch);
        Token tok(Token::Op, lex, startLine, startCol);
        tokens.push_back(tok);
        return tok;
    }
    // Delimiters: ( ) { } ; , 
    if (ch == '(' || ch == ')' || ch == '{' || ch == '}' || ch == ';' || ch == ',') {
        getCharAndAdvance(in, this->line, this->col);
        std::string lex(1, ch);
        Token tok(Token::Delimiter, lex, startLine, startCol);
        tokens.push_back(tok);
        return tok;
    }
    // If we reached here, it's an unrecognized character
    return Token(Token::EndOfFile, "", this->line, this->col);
}

void Lexer::printTokens() {
    for (const auto& token : tokens) {
        token.print();
    }
}
