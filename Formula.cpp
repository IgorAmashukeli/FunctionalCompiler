#include "Formula.hpp"
#include <assert.h>
#include <cstddef>
#include <cwchar>
#include <exception>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <ostream>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#define CHANGE_END()                                                           \
  if (first) {                                                                 \
    end = words.size();                                                        \
  }

#define CHECK_RANGE1(range, exception)                                         \
  if (end != start + range) {                                                  \
    throw exception("Wrong atom length");                                      \
  }

#define CHECK_RANGE2(range, exception)                                         \
  if (end < start + range) {                                                   \
    throw exception("Wrong formula length");                                   \
  }

#define CHECK_BRACKETS(words, left, right, exception)                          \
  if ((words[left] != "(") || (words[right] != ")")) {                         \
    throw exception("No first open or no last close bracket");                 \
  }

#define CHECK_ATOM(words, start)                                               \
  if ((words[start + 2] != "in")) {                                            \
    throw WrongAtom("No \"in\" in atom!");                                     \
  }

#define CREATE_SUB_VARIABLE(name, args, exp)                                   \
  try {                                                                        \
    name = make_shared<Variable>(args);                                        \
  } catch (const std::exception &e) {                                          \
    throw exp(e.what());                                                       \
  }

#define CREATE_SUB(name, args1, args2, args3, cls, exp)                        \
  try {                                                                        \
    name = make_shared<cls>(args1, args2, args3);                              \
  } catch (const std::exception &e) {                                          \
    throw exp(e.what());                                                       \
  }

#define CHECK_FORMULA_ELEMENTS()                                               \
  for (size_t i = start; i < end; ++i) {                                       \
    if ((!logic_elements.contains(words[i]) && !brackets.contains(words[i]) && \
         !check_variable(words[i]))) {                                         \
      throw WrongFormula("Wrong words in formula");                            \
    } else if (brackets_number < 0) {                                          \
      throw WrongFormula("Negative balance in the middle");                    \
    } else if (words[i] == "(") {                                              \
      brackets_number += 1;                                                    \
    } else if (words[i] == ")") {                                              \
      brackets_number -= 1;                                                    \
    }                                                                          \
  }

#define CHECK_BALANCE()                                                        \
  if (brackets_number != 0) {                                                  \
    throw WrongFormula("Wrong final balance");                                 \
  }

#define FIND_MIDDLE_WORD()                                                     \
  for (size_t i = start; i < end; ++i) {                                       \
    if (logic_elements.contains(words[i]) && brackets_number == 1) {           \
      middle_word = words[i];                                                  \
      index = i;                                                               \
    }                                                                          \
    if (words[i] == "(") {                                                     \
      brackets_number += 1;                                                    \
    } else if (words[i] == ")") {                                              \
      brackets_number -= 1;                                                    \
    }                                                                          \
  }

#define CHECK_MIDDLE_WORD()                                                    \
  if (middle_word == "") {                                                     \
    throw WrongFormula("Wrong operation");                                     \
  }

#define CHECK_FORMULA_RANGE_CONDITION(cond)                                    \
  if (cond) {                                                                  \
    throw WrongFormula("Wrong length of subformula");                          \
  }

#define ASSIGN_FIELDS(left_val, right_val, atom_val, quant_val, type_val)      \
  left_ = left_val;                                                            \
  right_ = right_val;                                                          \
  atom_ = atom_val;                                                            \
  quantifier_var_ = quant_val;                                                 \
  formulaType_ = type_val;

#define COPY(other, field, cls)                                                \
  if (other.field == nullptr) {                                                \
    this->field = nullptr;                                                     \
  } else {                                                                     \
    this->field = std::make_shared<cls>(*other.field);                         \
  }

// split string into vector of strings by space
std::vector<std::string> split(const std::string &str) {

  std::istringstream iss(str);
  std::vector<std::string> words;

  do {
    std::string word;
    iss >> word;
    if (!word.empty()) {
      words.push_back(word);
    }
  } while (iss);

  return words;
}

WrongVariable::WrongVariable(const std::string &errorMessage)
    : error_message_(errorMessage) {}

// Variable exception
const char *WrongVariable::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return error_message_.c_str();
}

WrongAtom::WrongAtom(const std::string &errorMessage)
    : error_message_(errorMessage) {}

// Atom exception
const char *WrongAtom::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return error_message_.c_str();
}

WrongFormula::WrongFormula(const std::string &errorMessage)
    : error_message_(errorMessage) {}

// Formula exception
const char *WrongFormula::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return error_message_.c_str();
}

// checks that the char is a letter
bool is_inside_range(char chr, char start) {
  return (chr - start >= 0 && chr - start < ENGLISH_ALPHABIT_SIZE);
}

// checks that the char is not 'e', 'i', 'E', 'I' and is a letter
// 'e', 'i', 'E', 'I' will be used later for other needs
bool is_correct_letter(char chr) {
  return (chr != 'e') && (chr != 'E') && (chr != 'i') && (chr != 'I') &&
         (is_inside_range(chr, 'a') || is_inside_range(chr, 'A'));
}

// checks if the variable is correct
bool check_variable(const std::string &str) {
  // letter is not empty
  if (str.empty()) {
    return false;
  }

  for (size_t i = 0; i < str.size(); ++i) {

    if (
        // variable string should be homogeneous
        ((i + 1 < str.size() && str[i] != str[i + 1]))

        // variable should contain letters only
        || (!is_correct_letter(str[i]))) {
      return false;
    }
  }

  return true;
}

Variable::Variable() { var_ = ""; }

Variable::Variable(const std::string &var) {

  // if the variable is not correct, throw an exception
  if (!check_variable(var)) {
    throw WrongVariable("Wrong variable name");
  }

  // if the variable is correct, create the variable
  var_ = var;
}

bool Variable::is_variable(const std::string &str) const {
  return (str == this->var_);
}

std::string Variable::to_string() const { return var_; }

// change variable
void Variable::substitute(const std::string &old_str,
                          const std::string &new_str) {
  if (!check_variable(new_str)) {
    throw WrongVariable("Wrong name to substitute");
  }

  if (old_str == this->var_) {
    this->var_ = new_str;
  }

  // else this->var_ = this->var_ (nothing to do)
}

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
  os << variable.to_string();
  return os;
}

Atom::Atom() {
  left_ = nullptr;
  right_ = nullptr;
}

Atom::Atom(const Atom &other) {
  this->left_ = std::make_shared<Variable>(*other.left_);
  this->right_ = std::make_shared<Variable>(*other.right_);
}

Atom &Atom::operator=(const Atom &other) {
  this->left_ = std::make_shared<Variable>(*other.left_);
  this->right_ = std::make_shared<Variable>(*other.right_);
  return *this;
}

Atom::Atom(const std::vector<std::string> &words, size_t start, size_t end,
           bool first) {

  CHANGE_END();

  CHECK_RANGE1(5, WrongAtom);

  CHECK_BRACKETS(words, start, start + 4, WrongAtom);

  CHECK_ATOM(words, start);

  CREATE_SUB_VARIABLE(left_, words[start + 1], WrongAtom);

  CREATE_SUB_VARIABLE(right_, words[start + 3], WrongAtom);
}

Atom::Atom(const std::string &str) : Atom(split(str), 0, 0, true) {}

Atom::Atom(std::shared_ptr<Variable> &var) : left_(var), right_(var) {
  if (var == nullptr) {
    throw WrongAtom("nullptr variable provided");
  }
}
Atom::Atom(std::shared_ptr<Variable> &left, std::shared_ptr<Variable> &right)
    : left_(left), right_(right) {
  if (left == nullptr || right == nullptr) {
    throw WrongAtom("nullptr variable provided");
  }
}

bool Atom::is_variable(const std::string &str) const {
  return this->left_->is_variable(str) || this->right_->is_variable(str);
}

std::string Atom::to_string() const {
  if (left_ == nullptr || right_ == nullptr) {
    throw WrongAtom("Atom was moved, now it is empty, you can't use it");
  }
  return ("( " + left_->to_string() + " in " + right_->to_string() + " )");
}

// change variable
void Atom::substitute(const std::string &old_str, const std::string &new_str) {
  if (!check_variable(new_str)) {
    throw WrongVariable("Wrong name to substitute");
  }

  // substitute to both sides of "in"
  left_->substitute(old_str, new_str);
  right_->substitute(old_str, new_str);
}

std::ostream &operator<<(std::ostream &os, const Atom &atom) {
  os << atom.to_string();
  return os;
}

Formula::Formula() {
  ASSIGN_FIELDS(nullptr, nullptr, nullptr, nullptr, FormulaType::ATOM);
}

// class Formula
Formula::Formula(const std::vector<std::string> &words, size_t start,
                 size_t end, bool first) {

  CHANGE_END();

  CHECK_RANGE2(3, WrongFormula);

  CHECK_BRACKETS(words, start, end - 1, WrongFormula);

  std::unordered_set<std::string> logic_elements = {
      "forall", "exists", "in", "wedge", "vee", "neg", "to", "leftrightarrow"};
  std::unordered_set<std::string> brackets = {"(", ")"};

  int brackets_number = 0;
  CHECK_FORMULA_ELEMENTS();

  CHECK_BALANCE();

  brackets_number = 0;
  std::string middle_word = "";
  size_t index = 0;

  FIND_MIDDLE_WORD();

  CHECK_MIDDLE_WORD();

  if (middle_word == "in") {

    CREATE_SUB(atom_, words, start, end, Atom, WrongFormula);
    ASSIGN_FIELDS(nullptr, nullptr, atom_, nullptr, FormulaType::ATOM);

  } else if ((middle_word == "neg")) {

    CHECK_FORMULA_RANGE_CONDITION((index + 1 > end || end < 1));

    CREATE_SUB(left_, words, index + 1, end - 1, Formula, WrongFormula);

    ASSIGN_FIELDS(left_, left_, nullptr, nullptr, FormulaType::NEG);

  } else if ((middle_word == "wedge") || (middle_word == "vee") ||
             (middle_word == "to") || (middle_word == "leftrightarrow")) {

    CHECK_FORMULA_RANGE_CONDITION(
        (index + 1 > end || end < 1 || start + 1 > index));

    CREATE_SUB(left_, words, start + 1, index, Formula, WrongFormula);

    CREATE_SUB(right_, words, index + 1, end - 1, Formula, WrongFormula);

    if (middle_word == "wedge") {
      ASSIGN_FIELDS(left_, right_, nullptr, nullptr, FormulaType::CONJ);
    } else if (middle_word == "vee") {
      ASSIGN_FIELDS(left_, right_, nullptr, nullptr, FormulaType::DISJ);
    } else if (middle_word == "to") {
      ASSIGN_FIELDS(left_, right_, nullptr, nullptr, FormulaType::IMPL);
    } else if (middle_word == "leftrightarrow") {
      ASSIGN_FIELDS(left_, right_, nullptr, nullptr, FormulaType::EQUIV);
    }

  } else if ((middle_word == "forall") || (middle_word == "exists")) {

    CHECK_FORMULA_RANGE_CONDITION(
        (index + 2 > end || end < 1 || start + 1 > index));

    CREATE_SUB_VARIABLE(quantifier_var_, words[index + 1], WrongFormula);

    CREATE_SUB(left_, words, index + 2, end - 1, Formula, WrongFormula);

    if (middle_word == "forall") {
      ASSIGN_FIELDS(left_, left_, nullptr, quantifier_var_,
                    FormulaType::FORALL);
    } else {
      ASSIGN_FIELDS(left_, left_, nullptr, quantifier_var_,
                    FormulaType::EXISTS);
    }
  }
}

// if you want to get formula from the string, use vector
Formula::Formula(const std::string &str) : Formula(split(str), 0, 0, true) {}

// constructor to create atom formula
Formula::Formula(std::shared_ptr<Atom> &atom) : atom_(atom) {
  if (atom == nullptr) {
    throw WrongFormula("nullptr atom provided");
  }
  ASSIGN_FIELDS(nullptr, nullptr, atom_, nullptr, FormulaType::ATOM)
}

// constructor to create (A V B), (A ^ B), (A -> B), (A <-> B)
Formula::Formula(std::shared_ptr<Formula> &left,
                 std::shared_ptr<Formula> &right, FormulaType formulaType)
    : left_(left), right_(right) {
  if (((formulaType != FormulaType::CONJ) &&
       (formulaType != FormulaType::DISJ) &&
       (formulaType != FormulaType::IMPL) &&
       (formulaType != FormulaType::EQUIV))) {
    throw WrongFormula("Wrong Formula type provided");
  }
  if ((left == nullptr) || (right == nullptr)) {
    throw WrongFormula("nullptr formula provided");
  }
  ASSIGN_FIELDS(left_, right_, nullptr, nullptr, formulaType)
}

// constructor to create (¬ A)
Formula::Formula(std::shared_ptr<Formula> &formula, FormulaType formulaType)
    : left_(formula), right_(formula) {
  if ((formulaType != FormulaType::NEG)) {
    throw WrongFormula("Wrong Formula type provided");
  }

  if (formula == nullptr) {
    throw WrongFormula("nullptr formula provided");
  }

  ASSIGN_FIELDS(left_, right_, nullptr, nullptr, formulaType);
}

// constructor to create (∀ x A), (∃ x A)
Formula::Formula(std::shared_ptr<Formula> &formula,
                 std::shared_ptr<Variable> &quantifier_var,
                 FormulaType formulaType)
    : left_(formula), right_(formula), quantifier_var_(quantifier_var) {
  if (((formulaType != FormulaType::FORALL) &&
       (formulaType != FormulaType::EXISTS))) {
    throw WrongFormula("Wrong Formula type provided");
  }
  if ((formula == nullptr) || (quantifier_var == nullptr)) {
    throw WrongFormula("nullptr formula provided");
  }

  ASSIGN_FIELDS(left_, right_, nullptr, quantifier_var_, formulaType);
}

Formula::Formula(const Formula &other) {
  this->formulaType_ = other.formulaType_;

  COPY(other, left_, Formula);
  COPY(other, right_, Formula);
  COPY(other, atom_, Atom);
  COPY(other, quantifier_var_, Variable);
}

Formula &Formula::operator=(const Formula &other) {
  this->formulaType_ = other.formulaType_;

  COPY(other, left_, Formula);
  COPY(other, right_, Formula);
  COPY(other, atom_, Atom);
  COPY(other, quantifier_var_, Variable);

  return *this;
}

bool Formula::is_variable(const std::string &str) const {
  if (this->formulaType_ == FormulaType::ATOM) {
    return this->atom_->is_variable(str);
  } else if (this->formulaType_ == FormulaType::CONJ ||
             this->formulaType_ == FormulaType::DISJ ||
             this->formulaType_ == FormulaType::IMPL ||
             this->formulaType_ == FormulaType::EQUIV) {
    return (this->left_->is_variable(str) || this->right_->is_variable(str));
  } else if (this->formulaType_ == FormulaType::NEG) {
    return (this->left_->is_variable(str));
  } else {
    return (this->left_->is_variable(str)) ||
           (quantifier_var_->is_variable(str));
  }
}

bool Formula::is_parameter(const std::string &str) const {
  if (formulaType_ == FormulaType::ATOM) {
    return this->atom_->is_variable(str);
  } else if (formulaType_ == FormulaType::NEG) {
    return this->left_->is_parameter(str);
  } else if ((formulaType_ == FormulaType::CONJ) ||
             (formulaType_ == FormulaType::DISJ) ||
             (formulaType_ == FormulaType::IMPL) ||
             (formulaType_ == FormulaType::EQUIV)) {
    return this->left_->is_parameter(str) || this->right_->is_parameter(str);
  } else {

    // variable under quantifier is not free
    if (this->quantifier_var_->is_variable(str)) {
      return false;
    }

    return this->left_->is_parameter(str);
  }
}

std::set<std::string> Formula::find_all_variables() const {
  std::set<std::string> variables = {};
  std::string formula_str = this->to_string();
  std::vector<std::string> words = split(formula_str);
  for (const auto &word : words) {
    if (check_variable(word)) {
      variables.insert(word);
    }
  }

  return variables;
}

std::set<std::string> Formula::find_all_parameters() const {
  std::set<std::string> parameters = {};
  std::string formula_str = this->to_string();
  std::vector<std::string> words = split(formula_str);
  for (const auto &word : words) {
    if (is_parameter(word)) {
      parameters.insert(word);
    }
  }

  return parameters;
}

std::string Formula::new_variable() const {
  std::set<std::string> variables = find_all_variables();
  if (variables.empty()) {
    throw WrongFormula("No variable in formula!");
  }

  std::string last_variable = *(--variables.end());
  if (last_variable.empty()) {
    throw WrongVariable("Wrong (empty) variable in formula");
  }

  if (last_variable[0] != 'z' && last_variable[0] != 'Z' &&
      last_variable[0] != 'd' && last_variable[0] != 'D' &&
      last_variable[0] != 'h' && last_variable[0] != 'H') {
    for (size_t i = 0; i < last_variable.size(); ++i) {
      last_variable[i] = last_variable[i] + 1;
    }
  } else if (last_variable[0] == 'd' || last_variable[0] == 'D' ||
             last_variable[0] == 'h' || last_variable[0] == 'H') {
    for (size_t i = 0; i < last_variable.size(); ++i) {
      last_variable[i] = last_variable[i] + 2;
    }
  } else if (last_variable[0] == 'z') {
    for (size_t i = 0; i < last_variable.size(); ++i) {
      last_variable[i] = 'a';
    }
    last_variable += 'a';
  } else {
    for (size_t i = 0; i < last_variable.size(); ++i) {
      last_variable[i] = 'A';
    }
    last_variable += 'A';
  }

  return last_variable;
}

std::string Formula::to_string() const {
  if (formulaType_ == FormulaType::ATOM) {
    return atom_->to_string();
  } else if (formulaType_ == FormulaType::CONJ) {
    return "( " + left_->to_string() + " wedge " + right_->to_string() + " )";
  } else if (formulaType_ == FormulaType::DISJ) {
    return "( " + left_->to_string() + " vee " + right_->to_string() + " )";
  } else if (formulaType_ == FormulaType::IMPL) {
    return "( " + left_->to_string() + " to " + right_->to_string() + " )";
  } else if (formulaType_ == FormulaType::EQUIV) {
    return "( " + left_->to_string() + " leftrightarrow " +
           right_->to_string() + " )";
  } else if (formulaType_ == FormulaType::NEG) {
    return "( neg " + left_->to_string() + " )";
  } else if (formulaType_ == FormulaType::EXISTS) {
    return "( exists " + quantifier_var_->to_string() + " " +
           left_->to_string() + " )";
  } else if (formulaType_ == FormulaType::FORALL) {
    return "( forall " + quantifier_var_->to_string() + " " +
           left_->to_string() + " )";
  }

  return "";
}

// substitute a variable
// formula(y/x)
void Formula::substitute(const std::string &old_str,
                         const std::string &new_str) {
  if (!check_variable(new_str)) {
    throw WrongVariable("Wrong name to substitute");
  }

  if (this->formulaType_ == FormulaType::ATOM) {

    // substitute to the Atom
    this->atom_->substitute(old_str, new_str);

  } else if (this->formulaType_ == FormulaType::NEG) {

    // substitute to the subformula under negation
    this->left_->substitute(old_str, new_str);

  } else if ((this->formulaType_ == FormulaType::CONJ) ||
             (this->formulaType_ == FormulaType::DISJ) ||
             (this->formulaType_ == FormulaType::IMPL) ||
             (this->formulaType_ == FormulaType::EQUIV)) {

    // substitute to the both sides of an operation
    this->left_->substitute(old_str, new_str);
    this->right_->substitute(old_str, new_str);

  } else {

    // if old variable is the one under quantifier (for example, x) ->
    // in ( Q x A ) no free variable x -> nothing to do
    // if formula is ( Q y A ), where y != x
    // => there can be free x in A => substitute in A
    if (!this->quantifier_var_->is_variable(old_str)) {

      this->left_->substitute(old_str, new_str);
    }
  }
}

// checks whether one can substitute a variable in another variable
// y - x - formula
bool Formula::can_substitute(const std::string &old_str,
                             const std::string &new_str) const {

  // quantifiers => no quantifiers with y variable
  if (this->formulaType_ == FormulaType::ATOM) {
    return true;

    // negation doesn't add quantifiers => ask left_
  } else if (this->formulaType_ == FormulaType::NEG) {
    return this->left_->can_substitute(old_str, new_str);

    // if something wrong in left or right => can't substitute
    // if everything ok both in left and right => can substitute
  } else if ((this->formulaType_ == FormulaType::CONJ) ||
             (this->formulaType_ == FormulaType::DISJ) ||
             (this->formulaType_ == FormulaType::IMPL) ||
             (this->formulaType_ == FormulaType::EQUIV)) {
    return this->left_->can_substitute(old_str, new_str) &&
           this->right_->can_substitute(old_str, new_str);

    // (Q z A)
  } else {

    // z == x (old_str)
    // => no free variables in (Q x A) => can substitute
    if (this->quantifier_var_->is_variable(old_str)) {
      return true;

      // z != x
      // => 2 possible cases:
      // a) can substitute in A (no y (new_str) quantifiers) + z quantifier is
      // not y quantifier =>
      // => can substitute in (Q x A) too
      //
      // b) x is not a parameter in A => can substitute (nothing will
      // change)
      //
      // if a is not true and b is not true => two possible cases:
      // a) x is a parameter in A + can't substitute in A => can't substitute in
      // (Q z A), because it consists A
      //
      // b) x is a parameter in A + z quantifier is actually y quantifier =>
      // => x is a parameter in (Q z A), because x != z => after possible
      // substitution there will be y variable in parameter place in A and it
      // will be quantified by y
      // => can't substitute
    } else {
      return ((!this->quantifier_var_->is_variable(new_str) &&
               this->left_->can_substitute(old_str, new_str)) ||
              (!this->is_parameter(old_str)));
    }
  }
}

std::ostream &operator<<(std::ostream &os, const Formula &formula) {
  os << formula.to_string();
  return os;
}

int main() {}