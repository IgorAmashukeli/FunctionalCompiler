#pragma once
#include <algorithm>
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

enum class FormulaType {
  ATOM = 0,
  NEG = 1,
  DISJ = 2,
  CONJ = 3,
  IMPL = 4,
  EXISTS = 5,
  FORALL = 6
};

enum { ENGLISH_ALPHABIT_SIZE = 26 };

class WrongVariable : public std::exception {
public:
  WrongVariable(const std::string &errorMessage);
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

  std::string error_message_;
};

class WrongAtom : public std::exception {
public:
  WrongAtom(const std::string &errorMessage);
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

private:
  std::string error_message_;
};

class WrongFormula : public std::exception {
public:
  WrongFormula(const std::string &errorMessage);
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;

private:
  std::string error_message_;
};

// Variable class
class Variable {
public:
  Variable();
  explicit Variable(const std::string &var);

  Variable(const Variable &atom) = default;
  Variable &operator=(const Variable &other) = default;
  Variable(Variable &&other) = default;
  Variable &operator=(Variable &&other) = default;

  bool is_variable(const std::string &str) const;

  std::string to_string() const;

  friend std::ostream &operator<<(std::ostream &os, const Variable &variable);

private:
  std::string var_;
};

// Atom class
class Atom {
public:
  Atom();
  explicit Atom(const std::vector<std::string> &words, size_t start, size_t end,
                bool first = false);

  Atom(const Atom &atom) = default;
  Atom &operator=(const Atom &other) = default;
  Atom(Atom &&other) = default;
  Atom &operator=(Atom &&other) = default;

  explicit Atom(const std::string &str);

  explicit Atom(std::shared_ptr<Variable> &var);
  explicit Atom(std::shared_ptr<Variable> &left,
                std::shared_ptr<Variable> &right);

  bool is_variable(const std::string &str) const;

  std::string to_string() const;

  friend std::ostream &operator<<(std::ostream &os, const Atom &atom);

private:
  std::shared_ptr<Variable> left_;
  std::shared_ptr<Variable> right_;
};

class Formula {

public:
  Formula();

  // parcer for the formula
  explicit Formula(const std::vector<std::string> &words, size_t start,
                   size_t end, bool first = false);

  Formula(const Formula &atom) = default;
  Formula &operator=(const Formula &other) = default;
  Formula(Formula &&other) = default;
  Formula &operator=(Formula &&other) = default;

  // if you want to get formula from the string, use vector
  explicit Formula(const std::string &str);

  // constructor to create atom formula
  explicit Formula(std::shared_ptr<Atom> &atom);

  // constructor to create (A V B), (A ^ B), (A -> B)
  explicit Formula(std::shared_ptr<Formula> &left,
                   std::shared_ptr<Formula> &right, FormulaType formulaType);

  // constructor to create (¬ A)
  explicit Formula(std::shared_ptr<Formula> &formula, FormulaType formulaType);

  // constructor to create (∀ x A), (∃ x A)
  explicit Formula(std::shared_ptr<Formula> &formula,
                   std::shared_ptr<Variable> &quantifier_var,
                   FormulaType formulaType);

  bool is_variable(const std::string &str) const;

  std::string to_string() const;

  friend std::ostream &operator<<(std::ostream &os, const Formula &formula);

private:
  FormulaType formulaType_;
  std::shared_ptr<Atom> atom_;
  std::shared_ptr<Formula> left_;
  std::shared_ptr<Formula> right_;
  std::shared_ptr<Variable> quantifier_var_;
};