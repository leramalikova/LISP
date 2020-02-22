#pragma once

#include <sstream>
#include <memory>
#include "parser.h"

class Scheme {
public:
    Scheme();

    void Clear();

    void SetTokenizer(std::stringstream *in);

    std::string Interpret();

private:
    std::shared_ptr<Scope> global_scope_;
    Tokenizer tokenizer_;
};