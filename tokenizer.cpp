#include "tokenizer.h"

#include <sstream>
#include <variant>
#include <cctype>
#include <iostream>

SyntaxError::SyntaxError(const std::string &what) : std::runtime_error(what) {
}

NameError::NameError(const std::string &what) : std::runtime_error(what) {
}

RuntimeError::RuntimeError(const std::string &what) : std::runtime_error(what) {
}

SymbolToken::SymbolToken(std::string init) : name_(init) {
}

bool SymbolToken::operator==(const SymbolToken &rhs) const {
    return name_ == rhs.name_;
}

QuoteToken::QuoteToken() {
}

bool QuoteToken::operator==(const QuoteToken &) const {
    return true;
}

DotToken::DotToken() {
}

bool DotToken::operator==(const DotToken &) const {
    return true;
}

enum class BracketToken;

ConstantToken::ConstantToken(int64_t val) : value_(val) {
}

bool ConstantToken::operator==(const ConstantToken &rhs) const {
    return value_ == rhs.value_;
}

typedef std::variant<SymbolToken, ConstantToken, BracketToken, QuoteToken, DotToken> Token;

bool IsSymbol(char c) {
    return isalnum(c) || c == '+' || c == '-' || c == '*' || c == '?' || c == '!' || c == '<'
           || c == '>' || c == '=' || c == '#' || c == '/';
}

void SkipSpaces(std::istream *in) {
    while (!in->eof() && in->peek() != '(' && in->peek() != ')' && in->peek() != '.' &&
           in->peek() != '\'' &&  !IsSymbol(in->peek())) {
        in->get();
    }
}

Token ReadToken(std::istream *in) {
    SkipSpaces(in);
    if (in->eof()) {
        return SymbolToken("");
    }

    std::string token;
    if (in->peek() == '(') {
        in->get();
        return BracketToken::OPEN;
    }

    if (in->peek() == ')') {
        in->get();
        return BracketToken::CLOSE;
    }

    if (in->peek() == '\'') {
        in->get();
        return QuoteToken();
    }

    if (in->peek() == '.') {
        in->get();
        return DotToken();
    }

    if (isdigit(in->peek())) {
        while (!in->eof() && isdigit(in->peek())) {
            token += in->peek();
            in->get();
        }
        return ConstantToken(std::stoi(token));
    }

    if (isalpha(in->peek())) {
        while (!in->eof() && IsSymbol(in->peek())) {
            token += in->peek();
            in->get();
        }
        return SymbolToken(token);
    }

    if (in->peek() == '*') {
        token += '*';
        in->get();
        return SymbolToken(token);
    }

    if (in->peek() == '/') {
        token += '/';
        in->get();
        return SymbolToken(token);
    }

    if (in->peek() == '=') {
        token += '=';
        in->get();
        return SymbolToken(token);
    }

    if (in->peek() == '<') {
        token += '<';
        in->get();
        if (!in->eof() && in->peek() == '=') {
            token += '=';
            in->get();
        }
        return SymbolToken(token);
    }

    if (in->peek() == '>') {
        token += '>';
        in->get();
        if (!in->eof() && in->peek() == '=') {
            token += '=';
            in->get();
        }
        return SymbolToken(token);
    }

    if (in->peek() == '#') {
        token += '#';
        in->get();
        if (!in->eof() && (in->peek() == 'f' || in->peek() == 't')) {
            token += in->peek();
            in->get();
        } else {
            throw SyntaxError{"Ожидалось #f или #t, но что-то пошло не так!"};
        }
        return SymbolToken(token);
    }

    if (in->peek() == '+') {
        in->get();
        if (!in->eof() && isdigit(in->peek())) {
            while (!in->eof() && isdigit(in->peek())) {
                token += in->peek();
                in->get();
            }
            return ConstantToken(std::stoll(token));
        }
        return SymbolToken("+");
    }

    if (in->peek() == '-') {
        in->get();
        if (!in->eof() && isdigit(in->peek())) {
            while (!in->eof() && isdigit(in->peek())) {
                token += in->peek();
                in->get();
            }
            return ConstantToken(-std::stoll(token));
        }
        return SymbolToken("-");
    }

    // throw SyntaxError{"Не удалось обнаружить лексему"};
    return SymbolToken("");
}

Tokenizer::Tokenizer(std::istream *in) : in_(in), current_token_(SymbolToken("")) {
    SkipSpaces(in_);
    is_end_ = in_->eof();
}

Tokenizer::Tokenizer() : in_(nullptr), current_token_(SymbolToken("")) {
    is_end_ = true;
}

// Достигли мы конца потока или нет.
bool Tokenizer::IsEnd() {
    return is_end_;
}

// Попытаться прочитать следующий токен.
// Либо IsEnd() станет false, либо токен можно будет получить через Token().
void Tokenizer::Next() {
    current_token_ = ReadToken(in_);
    if (current_token_ == Token{SymbolToken("")}) {
        is_end_ = true;
    }
}

// Получить текущий токен.
Token Tokenizer::GetToken() {
    if (current_token_ == Token{SymbolToken("")}) {
        Next();
    }
    return current_token_;
}


