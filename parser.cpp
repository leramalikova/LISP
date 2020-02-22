#include <vector>
#include <string>
#include <memory>
#include <unordered_map>
#include <variant>

#include <parser.h>

std::shared_ptr<Object> Object::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    (void)args;
    (void)scope;
    throw SyntaxError("Нельзя применить к аргументам");
}

bool Object::IsSymbol() {
    return false;
}

bool Object::IsNumber() {
    return false;
}

bool Object::IsCell() {
    return false;
}

bool Object::IsFunction() {
    return false;
}

bool Object::IsSyntax() {
    return false;
}

bool Object::IsList() {
    return false;
}

bool Object::IsPair() {
    return false;
}

bool Object::IsFalse() {
    return false;
}

bool Object::IsLambda() {
    return false;
}

std::shared_ptr<Object> Object::Eval(std::shared_ptr<Scope> scope) {
    (void)scope;
    throw SyntaxError("Нельзя звать Eval");
}

/***********************************************************
 ***********************************************************
 ***********************************************************/

bool Cell::IsCell() {
    return true;
}

void Cell::SetSecond(std::shared_ptr<Object> second) {
    second_ptr_ = second;
}

void Cell::SetFirst(std::shared_ptr<Object> first) {
    first_ptr_ = first;
}

Cell::Cell(std::shared_ptr<Object> first, std::shared_ptr<Object> second)
        : first_ptr_(first), second_ptr_(second) {
}

std::shared_ptr<Object> Cell::GetFirst() const {
    return first_ptr_;
}

std::shared_ptr<Object> Cell::GetSecond() const {
    return second_ptr_;
}

bool Number::IsNumber() {
    return true;
}

Number::Number(int64_t val) : value_(val) {
}

int64_t Number::GetValue() {
    return value_;
}

std::shared_ptr<Object> Number::Eval(std::shared_ptr<Scope> scope) {
    (void)scope;
    return shared_from_this();
}

Symbol::Symbol(std::string str) : name_(str) {
}

bool Symbol::IsSymbol() {
    return true;
}

std::string Symbol::GetName() {
    return name_;
}

bool Symbol::IsFalse() {
    return name_ == "#f";
}

List::List(std::shared_ptr<Object> head) : head_(head) {
}

std::shared_ptr<Cell> List::Head() {
    return std::dynamic_pointer_cast<Cell>(head_);
}

std::shared_ptr<Object> List::Eval(std::shared_ptr<Scope> scope) {
    (void)scope;
    throw RuntimeError("Нельзя звать eval от list");
}

std::shared_ptr<Object> List::SetCdr(std::shared_ptr<Object> new_tail) {
    // cdr
    if (!head_) {
        throw SyntaxError("Редактируемый список пусть");
    }
    std::dynamic_pointer_cast<Cell>(head_)->SetSecond(new_tail);
    return shared_from_this();
}

std::shared_ptr<Object> List::SetCar(std::shared_ptr<Object> new_first) {
    // car
    if (!head_) {
        throw SyntaxError("Редактируемый список пусть");
    }
    std::dynamic_pointer_cast<Cell>(head_)->SetFirst(new_first);
    return shared_from_this();
}

std::shared_ptr<Object> List::Car() {
    if (!head_) {
        throw SyntaxError("Список пусть");
    }
    return std::dynamic_pointer_cast<Cell>(head_)->GetFirst();
}

std::shared_ptr<Object> List::Cdr() {
    if (!head_) {
        throw SyntaxError("Список пусть");
    }
    return std::dynamic_pointer_cast<Cell>(head_)->GetSecond();
}

bool List::IsList() {
    auto temp = head_;
    while (temp && temp->IsCell()) {
        temp = std::dynamic_pointer_cast<Cell>(temp)->GetSecond();
    }
    return temp == nullptr;
}

bool List::IsPair() {
    if (!head_) {
        return false;
    }
    // лист из двух штук
    if (IsList()) {
        auto cell = std::dynamic_pointer_cast<Cell>(
                std::dynamic_pointer_cast<Cell>(head_)->GetSecond());
        return (cell && !cell->GetSecond());
    }
    // пара
    return (!std::dynamic_pointer_cast<Cell>(head_)->GetSecond()->IsCell());
}

/***********************************************************
 ***********************************************************
 ***********************************************************/

bool Function::IsFunction() {
    return true;
}

void Function::EvalArgs(std::vector<std::shared_ptr<Object>> &args, std::shared_ptr<Scope> scope) {
    for (auto &arg : args) {
        if (!arg) {
            throw RuntimeError("Аргументом функции не может быть nullptr");
        }
        arg = arg->Eval(scope);
    }
}

std::shared_ptr<Object> Quote::Apply(std::vector<std::shared_ptr<Object>> &args,
                              std::shared_ptr<Scope> scope) {
    (void)scope;
    if (args.size() != 1) {
        throw SyntaxError("Недостаточно аргументов для quote");
    }
    if (!args[0] || args[0]->IsSymbol() || args[0]->IsList() ||args[0]->IsCell()) {
        return args[0];
    }
    throw SyntaxError("?? at quote");
}

std::shared_ptr<Object> Max::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() < 1) {
        throw RuntimeError("Недостаточно аргументов для максимума");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у максимума");
    }
    int64_t max = std::dynamic_pointer_cast<Number>(args[0])->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        args[i] = args[i]->Eval(scope);
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у максимума");
        }
        max = std::max(max, std::dynamic_pointer_cast<Number>(args[i])->GetValue());
    }
    return std::make_shared<Number>(max);
}


std::shared_ptr<Object> Min::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() < 1) {
        throw RuntimeError("Недостаточно аргументов для минимума");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у минимума");
    }
    int64_t min = std::dynamic_pointer_cast<Number>(args[0])->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        args[i] = args[i]->Eval(scope);
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у минимума");
        }
        min = std::min(min, std::dynamic_pointer_cast<Number>(args[i])->GetValue());
    }
    return std::make_shared<Number>(min);
}

std::shared_ptr<Object> Abs::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw RuntimeError("Недостаточно аргументов для максимума");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у деления");
    }
    return std::make_shared<Number>(
            std::abs(std::dynamic_pointer_cast<Number>(args[0])->GetValue()));
}

std::shared_ptr<Object> Add::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    int64_t result = 0;
    EvalArgs(args, scope);
    for (auto &arg : args) {
        if (!arg->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сложения");
        }
        result += std::dynamic_pointer_cast<Number>(arg)->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Multiply::Apply(std::vector<std::shared_ptr<Object>> &args,
                                        std::shared_ptr<Scope> scope) {
    int64_t result = 1;
    EvalArgs(args, scope);
    for (auto &arg : args) {
        if (!arg->IsNumber()) {
            throw RuntimeError("Неверный аргумент у произведения");
        }
        result *= std::dynamic_pointer_cast<Number>(arg)->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Subtract::Apply(std::vector<std::shared_ptr<Object>> &args,
                                        std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        throw RuntimeError("Недостаточно аргументов для вычитания");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у вычитания");
    }
    int64_t result = std::dynamic_pointer_cast<Number>(args[0])->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у вычитания");
        }
        result -= std::dynamic_pointer_cast<Number>(args[i])->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Divide::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        throw RuntimeError("Недостаточно аргументов для деления");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у деления");
    }

    int64_t result = std::dynamic_pointer_cast<Number>(args[0])->GetValue();

    for (size_t i = 1; i < args.size(); ++i) {
        args[i] = args[i]->Eval(scope);
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у деления");
        }
        result /= std::dynamic_pointer_cast<Number>(args[i])->GetValue();
    }
    return std::make_shared<Number>(result);
}

std::shared_ptr<Object> Not::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw RuntimeError("Неверное количество аргументов");
    }

    EvalArgs(args, scope);
    if (args[0] && args[0]->IsFalse()) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}




/***********************************************************
 ***********************************************************
 ***********************************************************/

std::shared_ptr<Object> QNull::Apply(std::vector<std::shared_ptr<Object>> &args,
                                     std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }

    if (args[0]) {
        args[0] = args[0]->Eval(scope);
    }
    if (!args[0]) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> QPair::Apply(std::vector<std::shared_ptr<Object>> &args,
                                     std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }
    // не пустой
    if (!args[0]) {
        return std::make_shared<Symbol>("#f");
    }
    // переменная/выражение
    if (!args[0]->IsList()) {
        args[0] = args[0]->Eval(scope);
        // became null
        if (!args[0]) {
            return std::make_shared<Symbol>("#f");
        }
    }

    // cell/list
    if (!args[0]->IsCell() && !args[0]->IsList()) {
        return std::make_shared<Symbol>("#f");
    }

    // list
    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }

    if (args[0]->IsPair()) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> QList::Apply(std::vector<std::shared_ptr<Object>> &args,
                                     std::shared_ptr<Scope> scope) {

    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }
    // не пустой
    if (!args[0]) {
        return std::make_shared<Symbol>("#f");
    }
    // переменная/выражение
    if (!args[0]->IsList()) {
        args[0] = args[0]->Eval(scope);
        //became null
        if (!args[0]) {
            return std::make_shared<Symbol>("#t");
        }
    }

    if (!args[0]->IsCell() && !args[0]->IsList()) {
        return std::make_shared<Symbol>("#f");
    }

    // list
    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }
    if (args[0]->IsList()) {
        return std::make_shared<Symbol>("#t");
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> QNumber::Apply(std::vector<std::shared_ptr<Object>> &args,
                                       std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }

    if (args[0]) {
        args[0] = args[0]->Eval(scope);
        if (args[0]->IsNumber()) {
            return std::make_shared<Symbol>("#t");
        }
    }
    return std::make_shared<Symbol>("#f");
}

std::shared_ptr<Object> QBoolean::Apply(std::vector<std::shared_ptr<Object>> &args,
                                        std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }

    if (args[0]) {
        args[0] = args[0]->Eval(scope);
        if (args[0] && args[0]->IsSymbol()) {
            auto name = std::dynamic_pointer_cast<Symbol>(args[0])->GetName();
            if (name == "#t" || name == "#f") {
                return std::make_shared<Symbol>("#t");
            }
        }
    }
    return std::make_shared<Symbol>("#f");
}


std::shared_ptr<Object> QSymbol::Apply(std::vector<std::shared_ptr<Object>> &args,
                                       std::shared_ptr<Scope> scope) {
    if (args.size() != 1) {
        throw SyntaxError("Неверное количество аргументов");
    }

    if (args[0]) {
        args[0] = args[0]->Eval(scope);
        if (args[0] && args[0]->IsSymbol()) {
            return std::make_shared<Symbol>("#t");
        }
    }
    return std::make_shared<Symbol>("#f");
}


/***********************************************************
 ***********************************************************
 ***********************************************************/

std::shared_ptr<Object> Less::Apply(std::vector<std::shared_ptr<Object>> &args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        return std::make_shared<Symbol>("#t");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у сравнения");
    }
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сравнения");
        }
        if (!(std::dynamic_pointer_cast<Number>(args[i - 1])->GetValue() <
              std::dynamic_pointer_cast<Number>(args[i])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> LessEq::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        return std::make_shared<Symbol>("#t");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у сравнения");
    }
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сравнения");
        }
        if (!(std::dynamic_pointer_cast<Number>(args[i - 1])->GetValue() <=
              std::dynamic_pointer_cast<Number>(args[i])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr <Object> More::Apply(std::vector<std::shared_ptr<Object>> &args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        return std::make_shared<Symbol>("#t");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у сравнения");
    }
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сравнения");
        }
        if (!(std::dynamic_pointer_cast<Number>(args[i - 1])->GetValue() >
              std::dynamic_pointer_cast<Number>(args[i])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}


std::shared_ptr<Object> MoreEq::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        return std::make_shared<Symbol>("#t");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у сравнения");
    }
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сравнения");
        }
        if (!(std::dynamic_pointer_cast<Number>(args[i - 1])->GetValue() >=
              std::dynamic_pointer_cast<Number>(args[i])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

std::shared_ptr<Object> Eq::Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        return std::make_shared<Symbol>("#t");
    }

    EvalArgs(args, scope);
    if (!args[0]->IsNumber()) {
        throw RuntimeError("Неверный аргумент у сравнения");
    }
    for (size_t i = 1; i < args.size(); ++i) {
        if (!args[i]->IsNumber()) {
            throw RuntimeError("Неверный аргумент у сравнения");
        }
        if (!(std::dynamic_pointer_cast<Number>(args[i - 1])->GetValue() ==
              std::dynamic_pointer_cast<Number>(args[i])->GetValue())) {
            return std::make_shared<Symbol>("#f");
        }
    }
    return std::make_shared<Symbol>("#t");
}

/***********************************************************
 ***********************************************************
 ***********************************************************/

std::shared_ptr<Object> And::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    for (auto &arg : args) {
        if (arg) {
            arg = arg->Eval(scope);
            if (arg && arg->IsFalse()) {
                return arg;
            }
        }
    }
    if (args.empty()) {
        return std::make_shared<Symbol>("#t");
    } else {
        return args[args.size() - 1];
    }
}

std::shared_ptr<Object> Or::Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope) {
    for (auto &arg : args) {
        if (arg) {
            arg = arg->Eval(scope);
            if (!arg || !arg->IsFalse()) {
                return arg;
            }
        }
    }
    if (args.empty()) {
        return std::make_shared<Symbol>("#f");
    } else {
        return args[args.size() - 1];
    }
}


bool Syntax::IsSyntax() {
    return true;
}

/***********************************************************
 ***********************************************************
 ***********************************************************/

bool IsNumber(const std::shared_ptr<Object> &obj) {
    return obj->IsNumber();
}

std::shared_ptr<Number> AsNumber(const std::shared_ptr<Object> &obj) {
    return std::static_pointer_cast<Number>(obj);
}

bool IsCell(const std::shared_ptr<Object> &obj) {
    return obj->IsCell();
}

std::shared_ptr<Cell> AsCell(const std::shared_ptr<Object> &obj) {
    return std::static_pointer_cast<Cell>(obj);
}

bool IsSymbol(const std::shared_ptr<Object> &obj) {
    return obj->IsSymbol();
}

std::shared_ptr<Symbol> AsSymbol(const std::shared_ptr<Object> &obj) {
    return std::static_pointer_cast<Symbol>(obj);
}

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer);

std::shared_ptr<Object> Read(Tokenizer *tokenizer) {
    if (tokenizer->IsEnd()) {
        return nullptr;
    }

    auto token = tokenizer->GetToken();
    tokenizer->Next();

    if (token == Token{BracketToken::OPEN}) {
        return ReadList(tokenizer);
    }

    if (std::holds_alternative<SymbolToken>(token)) {
        return std::make_shared<Symbol>(std::get<SymbolToken>(token).name_);
    }

    if (std::holds_alternative<ConstantToken>(token)) {
        return std::make_shared<Number>(std::get<ConstantToken>(token).value_);
    }

    // иначе не разобрать с концом
    if (token == Token{BracketToken::CLOSE}) {
        return std::make_shared<Symbol>(")");
    }

    if (std::holds_alternative<QuoteToken>(token)) {
        if (tokenizer->IsEnd()) {
            throw SyntaxError{"Преждевременный конец!"};
        }
        auto head = std::make_shared<Cell>(std::make_shared<Symbol>("quote"),
                std::make_shared<Cell>(Read(tokenizer)));
        return head;
    }

    // иначе как в листе понять где было точка?
    if (std::holds_alternative<DotToken>(token)) {
        return std::make_shared<Symbol>(".");
    }

    return nullptr;
}

std::shared_ptr<Object> ReadList(Tokenizer *tokenizer) {
    if (tokenizer->IsEnd()) {
        throw SyntaxError{"Преждевременный конец списка! Не получена \')\' "};
    }

    auto token = Read(tokenizer);

    // пустой список
    if (token != nullptr && token->IsSymbol() && AsSymbol(token)->GetName() == ")") {
        return nullptr;
    }
    std::shared_ptr<Cell> head = std::make_shared<Cell>();

    if (token != nullptr && token->IsSymbol() && AsSymbol(token)->GetName() == ".") {
        throw SyntaxError{"Точка может идти только после другого элемента"};
    }

    head->SetFirst(token);
    auto current = head;

    while (!tokenizer->IsEnd()) {
        token = Read(tokenizer);
        if (token && token->IsSymbol() && AsSymbol(token)->GetName() == ".") {
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Преждевременный конец!");
            }
            token = Read(tokenizer);
            AsCell(current)->SetSecond(token);
            if (tokenizer->IsEnd()) {
                throw SyntaxError("Преждевременный конец!");
            }
            token = Read(tokenizer);
            if (token == nullptr || !token->IsSymbol() || AsSymbol(token)->GetName() != ")") {
                throw SyntaxError("Должна быть скобка");
            }
            return head;
        }
        if (token != nullptr && token->IsSymbol() && AsSymbol(token)->GetName() == ")") {
            return head;
        }
        auto next = std::make_shared<Cell>(token);
        AsCell(current)->SetSecond(next);
        current = next;
    }
    if (tokenizer->IsEnd()) {
        throw SyntaxError("Преждевременный конец!");
    }
    return head;
}

void Scope::ChangeVariable(const std::string &x, std::shared_ptr<Object> &val) {
    if (variables_.find(x) == variables_.end()) {
        throw NameError("Использование необъявленной переменной");
    }
    variables_[x] = val;
}

void Scope::OverrideVariable(const std::string &x, const std::shared_ptr<Object> &val) {
    variables_[x] = val;
}

std::shared_ptr<Object> Scope::LookUp(const std::string &var) {
    auto v_iter = variables_.find(var);
    if (v_iter != variables_.end()) {
        return v_iter->second;
    }
    throw NameError("Использование необъявленной переменной");
}

Scope::~Scope() {
    variables_.clear();
}

void Scope::Clear() {
    for (auto &func : variables_) {
        if (func.second->IsLambda()) {
            std::shared_ptr<Lambda> lambda = std::dynamic_pointer_cast<Lambda>(func.second);
            lambda->
        }
    }
    variables_.clear();
}


std::shared_ptr<Object> Cell::Eval(std::shared_ptr<Scope> scope) {
    auto p = first_ptr_->Eval(scope);

    if (!p || !(p->IsFunction() || p->IsSyntax())) {
        throw RuntimeError("Первый элемент должен быть функцией или синтаксисом");
    }

    auto args = ToVector(second_ptr_);
    // не вычисляем аргументы так как не от всех функций надо вычислять аргументы,
    // они там сами разберутся
    return p->Apply(args, scope);
}

std::vector<std::shared_ptr<Object>> ToVector(std::shared_ptr<Object> head) {
    std::vector<std::shared_ptr<Object>> vect;
    while (head) {
        if (std::dynamic_pointer_cast<Cell>(head)->GetFirst() &&
            std::dynamic_pointer_cast<Cell>(head)->GetFirst()->IsSymbol() &&
            std::dynamic_pointer_cast<Symbol>(std::dynamic_pointer_cast<Cell>(head)->GetFirst())
                    ->GetName() == "quote") {
            head = std::dynamic_pointer_cast<Cell>(head)->GetSecond();
            if (!head) {
                throw SyntaxError("Ничего после quote");
            }
            if (std::dynamic_pointer_cast<Cell>(head)->GetFirst() &&
                std::dynamic_pointer_cast<Cell>(head)->GetFirst()->IsSymbol()) {
                vect.push_back(std::make_shared<Symbol>("\'" + std::dynamic_pointer_cast<Symbol>
                        (std::dynamic_pointer_cast<Cell>(head)->GetFirst())->GetName()));
            } else if (!std::dynamic_pointer_cast<Cell>(head)->GetFirst() ||
                       std::dynamic_pointer_cast<Cell>(head)->GetFirst()->IsCell()) {
                vect.push_back(std::make_shared<List>(std::dynamic_pointer_cast<Cell>(head)
                                                              ->GetFirst()));
            } else {
                throw SyntaxError("Что-то не то после quote");
            }
            head = std::dynamic_pointer_cast<Cell>(head)->GetSecond();
        } else {
            vect.push_back(std::dynamic_pointer_cast<Cell>(head)->GetFirst());
            head = std::dynamic_pointer_cast<Cell>(head)->GetSecond();
        }
    }
    return vect;
}

/***********************************************************
 ***********************************************************
 ***********************************************************/

std::shared_ptr<Object> SetCar::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() != 2 || !args[0] || !args[0]->IsSymbol() || !args[1]) {
        throw SyntaxError("Аргументы  set-car некорректны");
    }

    EvalArgs(args, scope);
    if (!args[0] ) {
        throw RuntimeError("Первый аргумент set-car пуст");
    }

    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }
    if (!args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Аргумент set-car не list/pair");
    }

    std::dynamic_pointer_cast<List>(args[0])->SetCar(args[1]);
    return nullptr;
}


std::shared_ptr<Object> SetCdr::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() != 2 || !args[0] || !args[0]->IsSymbol() || !args[1]) {
        throw SyntaxError("Аргументы  set-car некорректны");
    }

    EvalArgs(args, scope);
    if (!args[0] ) {
        throw RuntimeError("Первый аргумент set-car пуст");
    }

    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }
    if (!args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Аргумент set-car не list/pair");
    }

    std::dynamic_pointer_cast<List>(args[0])->SetCdr(args[1]);
    return nullptr;
}

std::shared_ptr<Object> Car::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 1 || !args[0]) {
        throw SyntaxError("Аргумент car некорректен");
    }
    EvalArgs(args, scope);
    if (!args[0]) {
        throw RuntimeError("Аргумент car пуст");
    }
    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }
    if (!args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Аргумент car не list/pair");
    }
    return std::dynamic_pointer_cast<List>(args[0])->Car();
}

std::shared_ptr<Object> Cdr::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 1 || !args[0]) {
        throw SyntaxError("Аргумент car некорректен");
    }
    EvalArgs(args, scope);
    if (!args[0]) {
        throw RuntimeError("Аргумент cdr пуст");
    }
    if (args[0]->IsCell()) {
        args[0] = std::make_shared<List>(args[0]);
    }
    if (!args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Аргумент cdr не list/pair");
    }
    return std::dynamic_pointer_cast<List>(args[0])->Cdr();
}

std::shared_ptr<Object> Cons::Apply(std::vector<std::shared_ptr<Object>> &args,
                                    std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw SyntaxError("");
    }
    EvalArgs(args, scope);
    auto cell = std::make_shared<Cell>(args[0], args[1]);
    return cell;
}

std::shared_ptr<Object> NewList::Apply(std::vector<std::shared_ptr<Object>> &args,
                                       std::shared_ptr<Scope> scope) {
    if (args.empty()) {
        return nullptr;
    }
    EvalArgs(args, scope);
    std::shared_ptr<Cell> head = std::make_shared<Cell>();
    std::shared_ptr<Cell> current = head;

    for (size_t i = 0; i < args.size(); ++i) {
        current->SetFirst(args[i]);
        if (i + 1 != args.size()) {
            current->SetSecond(std::make_shared<Cell>());
            current = std::dynamic_pointer_cast<Cell>(current->GetSecond());
        }
    }

    return head;
}

std::shared_ptr<Object> ListTail::Apply(std::vector<std::shared_ptr<Object>> &args,
                                        std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("Неверное число аргументов list-tail");
    }
    EvalArgs(args, scope);

    if (!std::dynamic_pointer_cast<Number>(args[1])) {
        throw RuntimeError("Второй аргумент у list-ref должен быть индексом");
    }
    auto number = std::dynamic_pointer_cast<Number>(args[1])->GetValue();

    if (!args[0]->IsCell() && !args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Неверные аргументы у list-tail");
    }
    std::shared_ptr<Cell> cur;
    if (args[0]->IsList() || args[0]->IsPair()) {
        cur = std::dynamic_pointer_cast<List>(args[0])->Head();
    } else {
        cur = std::dynamic_pointer_cast<Cell>(args[0]);
    }

    for (int i = 0; i < number; ++i) {
        if (!cur) {
            throw RuntimeError("Index out of range");
        }
        cur = std::dynamic_pointer_cast<Cell>(cur->GetSecond());
    }
    return cur;
}

std::shared_ptr<Object> ListRef::Apply(std::vector<std::shared_ptr<Object>> &args,
                                       std::shared_ptr<Scope> scope) {
    if (args.size() != 2) {
        throw RuntimeError("Неверное число аргументов list-tail");
    }
    EvalArgs(args, scope);

    if (!std::dynamic_pointer_cast<Number>(args[1])) {
        throw RuntimeError("Второй аргумент у list-ref должен быть индексом");
    }
    auto number = std::dynamic_pointer_cast<Number>(args[1])->GetValue();

    if (!args[0]->IsCell() && !args[0]->IsList() && !args[0]->IsPair()) {
        throw RuntimeError("Неверные аргументы у list-tail");
    }
    std::shared_ptr<Cell> cur;
    if (args[0]->IsList() || args[0]->IsPair()) {
        cur = std::dynamic_pointer_cast<List>(args[0])->Head();
    } else {
        cur = std::dynamic_pointer_cast<Cell>(args[0]);
    }

    for (int i = 0; i < number; ++i) {
        if (!cur) {
            throw RuntimeError("Index out of range");
        }
        cur = std::dynamic_pointer_cast<Cell>(cur->GetSecond());
    }
    if (!cur) {
        throw RuntimeError("Index out of range");
    }
    return cur->GetFirst();
}

Lambda::Lambda(const std::vector<std::string> &variables,
               const std::vector<std::shared_ptr<Object>> &body,
               std::shared_ptr<Scope> old_scope) {
    defined_variables_ = variables;
    body_of_function_ = body;
    my_scope_ = std::make_shared<Scope>(*old_scope);
}

void Lambda::InsertMeToScope(const std::string &var) {
    my_scope_->OverrideVariable(var, shared_from_this());
}

bool Lambda::IsLambda() {
    return true;
}

std::shared_ptr<Object> Lambda::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() != defined_variables_.size()) {
        throw SyntaxError("Неверное число аргументов у lambda функции");
    }
    EvalArgs(args, scope);
    int i = 0;
    for (auto &var : defined_variables_) {
        my_scope_->OverrideVariable(var, args[i++]);
    }
    std::shared_ptr<Object> result = nullptr;
    for (auto &step : body_of_function_) {
        result = step->Eval(my_scope_);
    }
    return result;
}


std::shared_ptr<Object> CreateLambda::Apply(std::vector<std::shared_ptr<Object>> &args,
                                            std::shared_ptr<Scope> scope) {
    if (args.size() < 2) {
        throw SyntaxError("Недостаточно аргументов у lambda");
    }
    std::vector<std::string> defined_variables;
    std::vector<std::shared_ptr<Object>> body_of_function;

    auto head = args[0];
    while (head) {
        if (!std::dynamic_pointer_cast<Cell>(head)->GetFirst()->IsSymbol()) {
            throw RuntimeError("После lambda должен быть список переменных");
        }
        auto first = std::dynamic_pointer_cast<Cell>(head)->GetFirst();
        defined_variables.push_back(std::dynamic_pointer_cast<Symbol>(first)->GetName());
        head = std::dynamic_pointer_cast<Cell>(head)->GetSecond();
    }
    for (size_t i = 1; i < args.size(); ++i) {
        body_of_function.push_back(args[i]);
    }
    return std::make_shared<Lambda>(defined_variables, body_of_function, scope);
}

std::shared_ptr<Object> Define::Apply(std::vector<std::shared_ptr<Object>> &args,
                                      std::shared_ptr<Scope> scope) {
    if (args.size() != 2 || !args[0] || !args[1] || !(args[0]->IsSymbol() || args[0]->IsCell())) {
        throw SyntaxError("Неверные аргументы для define");
    }
    if (args[0]->IsCell()) {
        auto head = std::dynamic_pointer_cast<Cell>(args[0]);
        if (!head->GetFirst() || !head->GetFirst()->IsSymbol()) {
            throw SyntaxError("Неверные аргументы для define lambda-sugar");
        }

        std::string name = std::dynamic_pointer_cast<Symbol>(head->GetFirst())->GetName();

        head = std::dynamic_pointer_cast<Cell>(head->GetSecond());

        std::vector<std::string> defined_variables;
        std::vector<std::shared_ptr<Object>> body_of_function;

        while (head) {
            if (!std::dynamic_pointer_cast<Cell>(head)->GetFirst()->IsSymbol()) {
                throw RuntimeError("После lambda должен быть список переменных");
            }
            auto first = std::dynamic_pointer_cast<Cell>(head)->GetFirst();
            defined_variables.push_back(std::dynamic_pointer_cast<Symbol>(first)->GetName());
            head = std::dynamic_pointer_cast<Cell>(std::dynamic_pointer_cast<Cell>(head)->
                    GetSecond());
        }
        for (size_t i = 1; i < args.size(); ++i) {
            body_of_function.push_back(args[i]);
        }
        std::shared_ptr<Object> lambda = std::make_shared<Lambda>(defined_variables,
                body_of_function, scope);
        std::dynamic_pointer_cast<Lambda>(lambda)->InsertMeToScope(name);
        scope->OverrideVariable(name, lambda);
        return nullptr;
    }
    args[1] = args[1]->Eval(scope);
    std::string name = std::dynamic_pointer_cast<Symbol>(args[0])->GetName();
    if (args[1]->IsLambda()) {
        std::dynamic_pointer_cast<Lambda>(args[1])->InsertMeToScope(name);
    }
    scope->OverrideVariable(name, args[1]);
    return nullptr;
}

std::shared_ptr<Object> If::Apply(std::vector<std::shared_ptr<Object>> &args,
                                  std::shared_ptr<Scope> scope) {
    if (args.size() < 2 || args.size() > 3 || !args[0]) {
        throw SyntaxError("");
    }
    args[0] = args[0]->Eval(scope);
    if (!args[0]->IsSymbol() || !(std::dynamic_pointer_cast<Symbol>(args[0])->GetName() == "#t"
                                  || std::dynamic_pointer_cast<Symbol>(args[0])->GetName() ==
                                     "#f")) {
        throw SyntaxError("");
    }
    if (std::dynamic_pointer_cast<Symbol>(args[0])->GetName() == "#t") {
        return args[1]->Eval(scope);
    }
    if (args.size() == 3) {
        return args[2]->Eval(scope);
    }
    return std::shared_ptr<List>(nullptr);
}

std::shared_ptr<Object> Set::Apply(std::vector<std::shared_ptr<Object>> &args,
                                   std::shared_ptr<Scope> scope) {
    if (args.size() != 2 || !args[0] || !args[1] || !args[0]->IsSymbol()) {
        throw SyntaxError("");
    }
    args[1] = args[1]->Eval(scope);
    scope->ChangeVariable(std::dynamic_pointer_cast<Symbol>(args[0])->GetName(),
                          args[1]);
    return nullptr;
}

Scope::Scope() : variables_(std::unordered_map<std::string, std::shared_ptr<Object>>()) {
    variables_["+"] = std::make_shared<Add>();
    variables_["-"] = std::make_shared<Subtract>();
    variables_["/"] = std::make_shared<Divide>();
    variables_["*"] = std::make_shared<Multiply>();

    variables_["and"] = std::make_shared<And>();
    variables_["or"] = std::make_shared<Or>();
    variables_["if"] = std::make_shared<If>();
    variables_["define"] = std::make_shared<Define>();

    variables_["list-tail"] = std::make_shared<ListTail>();
    variables_["list-ref"] = std::make_shared<ListRef>();
    variables_["list"] = std::make_shared<NewList>();
    variables_["cons"] = std::make_shared<Cons>();

    variables_["cdr"] = std::make_shared<Cdr>();
    variables_["car"] = std::make_shared<Car>();
    variables_["set-cdr!"] = std::make_shared<SetCdr>();
    variables_["set-car!"] = std::make_shared<SetCar>();

    variables_["+"] = std::make_shared<Add>();
    variables_["-"] = std::make_shared<Subtract>();
    variables_["/"] = std::make_shared<Divide>();
    variables_["*"] = std::make_shared<Multiply>();

    variables_["lambda"] = std::make_shared<CreateLambda>();
    variables_["set!"] = std::make_shared<Set>();
    variables_["not"] = std::make_shared<Not>();
    variables_["max"] = std::make_shared<Max>();
    variables_["min"] = std::make_shared<Min>();
    variables_["abs"] = std::make_shared<Abs>();

    variables_["<"] = std::make_shared<Less>();
    variables_["<="] = std::make_shared<LessEq>();
    variables_[">="] = std::make_shared<MoreEq>();
    variables_[">"] = std::make_shared<More>();
    variables_["="] = std::make_shared<Eq>();

    variables_["null?"] = std::make_shared<QNull>();
    variables_["pair?"] = std::make_shared<QPair>();
    variables_["list?"] = std::make_shared<QList>();
    variables_["boolean?"] = std::make_shared<QBoolean>();
    variables_["symbol?"] = std::make_shared<QSymbol>();
    variables_["number?"] = std::make_shared<QNumber>();
    variables_["quote"] = std::make_shared<Quote>();
}

Scope::Scope(const Scope &rhs) {
    variables_ = rhs.variables_;
}

std::shared_ptr<Object> Symbol::Eval(std::shared_ptr<Scope> scope) {
    if (name_ == "#f" || name_ == "#t") {
        return std::make_shared<Symbol>(name_);
    }
    if (!name_.empty() && name_[0] == '\'') {
        return std::make_shared<Symbol>(name_.substr(1));
    }
    return scope->LookUp(name_);
}

std::string PrintTo(const std::shared_ptr<Object> &obj) {
    if (!obj) {
        return "()";
    }
    if (obj->IsSymbol()) {
        return std::dynamic_pointer_cast<Symbol>(obj)->GetName();
    }
    if (obj->IsNumber()) {
        return std::to_string(std::dynamic_pointer_cast<Number>(obj)->GetValue());
    }
    std::string result;
    if (obj->IsList() || obj->IsCell()) {
        std::shared_ptr<Cell> cell;
        if (obj->IsList()) {
            cell = std::dynamic_pointer_cast<Cell>(std::dynamic_pointer_cast<List>(obj)->Head());
        } else {
            cell = std::dynamic_pointer_cast<Cell>(obj);
        }
        while (cell) {
            if (!result.empty()) {
                result += " ";
            }
            result += PrintTo(cell->GetFirst());
            if (cell->GetSecond() && !cell->GetSecond()->IsCell()) {
                result += " . " + PrintTo(cell->GetSecond());
                return "(" + result + ")";
            }
            cell = std::dynamic_pointer_cast<Cell>(cell->GetSecond());
        }
        return "(" + result + ")";
    }
    return result;
}