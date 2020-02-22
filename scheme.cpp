#include "scheme.h"
#include "parser.cpp"
#include "tokenizer.h"

Scheme::Scheme() : global_scope_(std::make_shared<Scope>()), tokenizer_(Tokenizer()) {
}

void Scheme::Clear() {
    global_scope_->Clear();
}

void Scheme::SetTokenizer(std::stringstream *in) {
    tokenizer_ = Tokenizer(in);
}

std::string Scheme::Interpret() {
    auto result = Read(&tokenizer_);
    if (!result) {
        throw RuntimeError("нельзя звать eval от пустого списка");
    }
    if (!tokenizer_.IsEnd()) {
        throw SyntaxError("Должен быть конец ввода");
    }
    result = result->Eval(global_scope_);
    return PrintTo(result);
}