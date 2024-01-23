#pragma once
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
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class WrongAtom : public std::exception {
public:
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class WrongFormula : public std::exception {
public:
  const char *what() const _GLIBCXX_TXN_SAFE_DYN _GLIBCXX_NOTHROW override;
};

class Variable {
public:
  Variable();
  explicit Variable(const std::string &var);

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

  explicit Atom(const std::string &str);

  explicit Atom(Variable *var);
  explicit Atom(Variable *left, Variable *right);

  std::string to_string() const;

  friend std::ostream &operator<<(std::ostream &os, const Atom &atom);

  ~Atom();

private:
  Variable *left_;
  Variable *right_;
  bool to_delete_ = false;
};

class Formula {

public:
  Formula();

  // parcer for the formula
  explicit Formula(const std::vector<std::string> &words, size_t start,
                   size_t end, bool first = false);

  // if you want to get formula from the string, use vector
  explicit Formula(const std::string &str);

  // constructor to create atom formula
  explicit Formula(Atom *atom);

  // constructor to create (A V B), (A ^ B), (A -> B)
  explicit Formula(Formula *left, Formula *right, FormulaType formulaType);

  // constructor to create (¬ A)
  explicit Formula(Formula *formula, FormulaType formulaType);

  // constructor to create (∀ x A), (∃ x A)
  explicit Formula(Formula *formula, Variable *quantifier_var,
                   FormulaType formulaType);

  std::string to_string() const;

  ~Formula();

  friend std::ostream &operator<<(std::ostream &os, const Formula &formula);

private:
  FormulaType formulaType_;
  Atom *atom_;
  Formula *left_;
  Formula *right_;
  Variable *quantifier_var_;
  bool to_delete_ = false;
};