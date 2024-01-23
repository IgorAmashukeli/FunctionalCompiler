#include "Formula.hpp"
#include <assert.h>
#include <cstddef>
#include <cwchar>
#include <exception>
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
  if (end - start != range) {                                                  \
    throw exception();                                                         \
  }

#define CHECK_RANGE2(range, exception)                                         \
  if (end - start < range) {                                                   \
    throw exception();                                                         \
  }

#define CHECK_BRACKETS(words, left, right, exception)                          \
  if ((words[left] != "(") || (words[right] != ")")) {                         \
    throw exception();                                                         \
  }

#define CHECK_ATOM(words, start)                                               \
  if ((words[start + 2] != "in")) {                                            \
    throw WrongAtom();                                                         \
  }

#define CREATE_SUB_VARIABLE(name, args, exp, is_del, to_del)                   \
  try {                                                                        \
    name = new Variable(args);                                                 \
  } catch (...) {                                                              \
    if (is_del) {                                                              \
      delete to_del;                                                           \
    }                                                                          \
    throw exp();                                                               \
  }

#define CREATE_SUB(name, args1, args2, args3, cls, exp, is_del, to_del)        \
  try {                                                                        \
    name = new cls(args1, args2, args3);                                       \
  } catch (...) {                                                              \
    if (is_del) {                                                              \
      delete to_del;                                                           \
    }                                                                          \
    throw exp();                                                               \
  }

#define CHECK_FORMULA_ELEMENTS()                                               \
  for (size_t i = start; i < end; ++i) {                                       \
    if ((!logic_elements.contains(words[i]) && !brackets.contains(words[i]) && \
         !check_variable(words[i])) ||                                         \
        brackets_number < 0) {                                                 \
      throw WrongFormula();                                                    \
    } else if (words[i] == "(") {                                              \
      brackets_number += 1;                                                    \
    } else if (words[i] == ")") {                                              \
      brackets_number -= 1;                                                    \
    }                                                                          \
  }

#define CHECK_BALANCE()                                                        \
  if (brackets_number != 0) {                                                  \
    throw WrongFormula();                                                      \
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
    throw WrongFormula();                                                      \
  }

#define CHECK_FORMULA_RANGE_CONDITION(cond)                                    \
  if (cond) {                                                                  \
    throw WrongFormula();                                                      \
  }

#define ASSIGN_FIELDS(left_val, right_val, atom_val, quant_val, del_val,       \
                      type_val)                                                \
  left_ = left_val;                                                            \
  right_ = right_val;                                                          \
  atom_ = atom_val;                                                            \
  quantifier_var_ = quant_val;                                                 \
  to_delete_ = del_val;                                                        \
  formulaType_ = type_val;

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

// Variable exception
const char *WrongVariable::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return "Wrong variable name";
}

// Atom exception
const char *WrongAtom::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return "Wrong atom name";
}

// Formula exception
const char *WrongFormula::what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW {
  return "Wrong formula name";
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

Variable::Variable(const std::string &var) {

  // if the variable is not correct, throw an exception
  if (!check_variable(var)) {
    throw WrongVariable();
  }

  // if the variable is correct, create the variable
  var_ = var;
}

std::string Variable::to_string() const { return var_; }

std::ostream &operator<<(std::ostream &os, const Variable &variable) {
  os << variable.to_string();
  return os;
}

Atom::Atom(const std::vector<std::string> &words, size_t start, size_t end,
           bool first) {

  CHANGE_END();

  CHECK_RANGE1(5, WrongAtom);

  CHECK_BRACKETS(words, start, start + 4, WrongAtom);

  CHECK_ATOM(words, start);

  CREATE_SUB_VARIABLE(left_, words[start + 1], WrongAtom, false, left_);

  CREATE_SUB_VARIABLE(right_, words[start + 3], WrongAtom, true, left_);

  to_delete_ = true;
}

Atom::Atom(const std::string &str) : Atom(split(str), 0, 0, true) {}

Atom::Atom(Variable &var) : left_(&var), right_(&var) {}
Atom::Atom(Variable &left, Variable &right) : left_(&left), right_(&right) {}

std::string Atom::to_string() const {
  return ("( " + left_->to_string() + " in " + right_->to_string() + " )");
}

Atom::~Atom() {
  if (to_delete_) {
    delete left_;
    delete right_;
  }
}

std::ostream &operator<<(std::ostream &os, const Atom &atom) {
  os << atom.to_string();
  return os;
}

// class Formula
Formula::Formula(const std::vector<std::string> &words, size_t start,
                 size_t end, bool first) {

  CHANGE_END();

  CHECK_RANGE2(3, WrongFormula);

  CHECK_BRACKETS(words, start, end - 1, WrongFormula);

  std::unordered_set<std::string> logic_elements = {
      "forall", "exists", "in", "wedge", "vee", "neg", "to"};
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

    CREATE_SUB(atom_, words, start, end, Atom, WrongFormula, false, atom_);
    ASSIGN_FIELDS(NULL, NULL, atom_, NULL, true, FormulaType::ATOM);

  } else if ((middle_word == "neg")) {

    CHECK_FORMULA_RANGE_CONDITION((index + 1 > end || end < 1));

    CREATE_SUB(left_, words, index + 1, end - 1, Formula, WrongFormula, false,
               left_);

    ASSIGN_FIELDS(left_, left_, NULL, NULL, true, FormulaType::NEG);

  } else if ((middle_word == "wedge") || (middle_word == "vee") ||
             (middle_word == "to")) {

    CHECK_FORMULA_RANGE_CONDITION(
        (index + 1 > end || end < 1 || start + 1 > index));

    CREATE_SUB(left_, words, start + 1, index, Formula, WrongFormula, false,
               left_);

    CREATE_SUB(right_, words, index + 1, end - 1, Formula, WrongFormula, true,
               left_);

    if (middle_word == "wedge") {
      ASSIGN_FIELDS(left_, right_, NULL, NULL, true, FormulaType::CONJ);
    } else if (middle_word == "vee") {
      ASSIGN_FIELDS(left_, right_, NULL, NULL, true, FormulaType::DISJ);
    } else if (middle_word == "to") {
      ASSIGN_FIELDS(left_, right_, NULL, NULL, true, FormulaType::IMPL);
    }

  } else if ((middle_word == "forall") || (middle_word == "exists")) {

    CHECK_FORMULA_RANGE_CONDITION(
        (index + 2 > end || end < 1 || start + 1 > index));

    CREATE_SUB_VARIABLE(quantifier_var_, words[index + 1], WrongFormula, false,
                        quantifier_var_);

    CREATE_SUB(left_, words, index + 2, end - 1, Formula, WrongFormula, true,
               quantifier_var_);

    if (middle_word == "forall") {
      ASSIGN_FIELDS(left_, left_, NULL, quantifier_var_, true,
                    FormulaType::FORALL);
    } else {
      ASSIGN_FIELDS(left_, left_, NULL, quantifier_var_, true,
                    FormulaType::EXISTS);
    }
  }
}

// if you want to get formula from the string, use vector
Formula::Formula(const std::string &str) : Formula(split(str), 0, 0, true) {}

// constructor to create atom formula
Formula::Formula(Atom *atom) {
  if (atom == NULL) {
    throw WrongFormula();
  }
  ASSIGN_FIELDS(NULL, NULL, atom, NULL, false, FormulaType::ATOM)
}

// constructor to create (A V B), (A ^ B), (A -> B)
Formula::Formula(Formula *left, Formula *right, FormulaType formulaType) {
  if (((formulaType != FormulaType::CONJ) &&
       (formulaType != FormulaType::DISJ) &&
       (formulaType != FormulaType::IMPL)) ||
      (left == NULL) || (right == NULL)) {
    throw WrongFormula();
  }
  ASSIGN_FIELDS(left, right, NULL, NULL, false, formulaType)
}

// constructor to create (¬ A)
Formula::Formula(Formula *formula, FormulaType formulaType) {
  if ((formulaType != FormulaType::NEG) || formula == NULL) {
    throw WrongFormula();
  }

  ASSIGN_FIELDS(formula, formula, NULL, NULL, false, formulaType);
}

// constructor to create (∀ x A), (∃ x A)
Formula::Formula(Formula *formula, Variable *quantifier_var,
                 FormulaType formulaType) {
  if (((formulaType != FormulaType::FORALL) &&
       (formulaType != FormulaType::EXISTS)) ||
      (formula == NULL) || (quantifier_var == NULL)) {
    throw WrongFormula();
  }

  ASSIGN_FIELDS(formula, formula, NULL, quantifier_var, false, formulaType);
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

Formula::~Formula() {
  if (to_delete_) {
    bool to_delete_right = left_ != right_;
    if (formulaType_ != FormulaType::ATOM) {
      delete left_;

      if ((formulaType_ == FormulaType::FORALL) ||
          (formulaType_ == FormulaType::EXISTS)) {
        delete quantifier_var_;
      } else if ((formulaType_ != FormulaType::NEG) && to_delete_right) {
        delete right_;
      }
    }
  }
}

std::ostream &operator<<(std::ostream &os, const Formula &formula) {
  os << formula.to_string();
  return os;
}

void check(const Formula &formula) {
  std::string formula_str = formula.to_string();
  Formula formula2 = Formula(formula_str);
  std::string formula2_str = formula2.to_string();
  assert(formula_str == formula2_str);
}

int main() {
  Variable left = Variable("a");
  Variable right = Variable("b");
  Atom atom = Atom("( mm in cc )");
  Variable var = Variable("aa");
  Variable var2 = Variable("bb");
  Formula formula = Formula(&atom);
  Formula formula2 = Formula(&formula, &formula, FormulaType::CONJ);
  Formula formula3 = Formula(&formula, &formula2, FormulaType::DISJ);
  Formula formula4 = Formula(&formula2, &formula3, FormulaType::IMPL);
  Formula formula5 = Formula(&formula3, &var, FormulaType::FORALL);
  Formula formula6 = Formula(&formula4, &var, FormulaType::EXISTS);
  Formula formula7 = Formula(&formula6, FormulaType::NEG);
  Formula formula8 = Formula(&formula5, &formula7, FormulaType::DISJ);
  Formula formula9 = Formula(&formula8, &var2, FormulaType::EXISTS);
  Formula formula10 = Formula(&formula9, &formula9, FormulaType::IMPL);
  check(formula);
  check(formula2);
  check(formula3);
  check(formula4);
  check(formula5);
  check(formula6);
  check(formula7);
  check(formula8);
  check(formula9);
  check(formula10);
}