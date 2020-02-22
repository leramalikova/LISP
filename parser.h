#pragma once

#include "tokenizer.h"
#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <variant>

class Scope;

class Object : public std::enable_shared_from_this<Object> {
public:
    virtual std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);

    virtual ~Object() = default;

    virtual bool IsSymbol();

    virtual bool IsNumber();

    virtual bool IsCell();

    virtual bool IsFunction();

    virtual bool IsSyntax();

    virtual bool IsList();

    virtual bool IsPair();

    virtual bool IsFalse();

    virtual  bool IsLambda();

    virtual std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope);
};

class Scope {
public:
    Scope();

    Scope(const Scope &rhs);

    void Clear();

    void ChangeVariable(const std::string &x, std::shared_ptr<Object> &val);

    void OverrideVariable(const std::string &x, const std::shared_ptr<Object> &val);

    std::shared_ptr<Object> LookUp(const std::string &var);

    ~Scope();
private:
    std::unordered_map<std::string, std::shared_ptr<Object>> variables_;
};

std::vector<std::shared_ptr<Object>> ToVector(std::shared_ptr<Object> head);

class Cell : public Object {
public:
    bool IsCell();

    void SetSecond(std::shared_ptr<Object> second);

    void SetFirst(std::shared_ptr<Object> first);

    Cell(std::shared_ptr<Object> first = nullptr, std::shared_ptr<Object> second = nullptr);

    std::shared_ptr<Object> GetFirst() const;

    std::shared_ptr<Object> GetSecond() const;

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope);

private:
    std::shared_ptr<Object> first_ptr_;
    std::shared_ptr<Object> second_ptr_;
};

class Number : public Object {
public:
    bool IsNumber();

    Number(int64_t val);

    int64_t GetValue();

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope);

private:
    int64_t value_;
};

class Symbol : public Object {
public:
    Symbol(std::string str);

    bool IsSymbol();

    std::string GetName();

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope);

    bool IsFalse();

private:
    std::string name_;
};

class List : public Object {
public:
    List(std::shared_ptr<Object> head);

    std::shared_ptr<Cell> Head();

    std::shared_ptr<Object> Eval(std::shared_ptr<Scope> scope);

    std::shared_ptr<Object> SetCdr(std::shared_ptr<Object> new_tail);

    std::shared_ptr<Object> SetCar(std::shared_ptr<Object> new_first);

    std::shared_ptr<Object> Car();

    std::shared_ptr<Object> Cdr();

    bool IsList();

    bool IsPair();
private:
    std::shared_ptr<Object> head_;
};

/***********************************************************
 ***********************************************************
 ***********************************************************/

class Function : public Object {
public:
    bool IsFunction();

    void EvalArgs(std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Scope> scope);
};

class Quote : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Max : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Min : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Abs : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Add : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Multiply : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Subtract : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Divide : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Not : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

/***********************************************************
 ***********************************************************
 ***********************************************************/

class QNull : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class QPair : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class QList : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class QNumber : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class QBoolean : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class QSymbol : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

/***********************************************************
 ***********************************************************
 ***********************************************************/

class Less : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class LessEq : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class More : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class MoreEq : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Eq : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

/***********************************************************
 ***********************************************************
 ***********************************************************/

class And : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Or : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Syntax : public Object {
public:
    virtual std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                          std::shared_ptr<Scope> scope) = 0;

    bool IsSyntax();
};

/***********************************************************
 ***********************************************************
 ***********************************************************/

bool IsNumber(const std::shared_ptr<Object> &obj);

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object> &obj);

bool IsCell(const std::shared_ptr<Object> &obj);

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object> &obj);

bool IsSymbol(const std::shared_ptr<Object> &obj);

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object> &obj);

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer);

std::shared_ptr<Object> Read(Tokenizer *tokenizer);

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer);

std::vector<std::shared_ptr<Object>> ToVector(std::shared_ptr<Object> head);

/***********************************************************
 ***********************************************************
 ***********************************************************/

class SetCar : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class SetCdr : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Car : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Cdr : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Cons : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class NewList : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class ListTail : public Function {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class ListRef : public Function {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Lambda : public Function {
public:
    Lambda(const std::vector<std::string> &variables,
            const std::vector<std::shared_ptr<Object>> &body,
            std::shared_ptr<Scope> old_scope);

    bool IsLambda();

    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);

    void InsertMeToScope(const std::string &var);
private:
    std::vector<std::string> defined_variables_;
    std::vector<std::shared_ptr<Object>> body_of_function_;
    std::shared_ptr<Scope> my_scope_;
};

class CreateLambda : public Syntax {
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Define : public Syntax {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class If : public Syntax {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

class Set : public Syntax {
public:
    std::shared_ptr<Object> Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope);
};

std::string PrintTo(const std::shared_ptr<Object> &obj);