# MathProver

# To run the first test (checks Formula class constructors):

add this main in Formula.cpp:

int main() {
for (size_t i = 0; i < 10000; ++i) {
std::string input_str;
std::getline(std::cin, input_str);
std::shared_ptr<Formula> formula;
try {
formula = std::make_shared<Formula>(input_str);
std::cout << i + 1 << " valid\n";
} catch (...) {
std::cout << i + 1 << " invalid\n";
}
}
}

then

run run.sh in test1

# To run the second test (checks is_variable method):

add this main in Formula.cpp:

int main() {
for (size_t a = 0; a < 100; ++a) {
std::string str;
std::getline(std::cin, str);
std::shared_ptr<Formula> formula = std::make_shared<Formula>(str);
for (size_t i = 1; i < 10; ++i) {
for (size_t j = 0; j < ENGLISH_ALPHABIT_SIZE; ++j) {
char letter1 = 'a' + j;
char letter2 = 'A' + j;
std::string var1(i, letter1);
std::string var2(i, letter2);
if (formula->is_variable(var1)) {
std::cout << var1 << "\n";
}
if (formula->is_variable(var2)) {
std::cout << var2 << "\n";
}
}
}
}
}

then

run run.sh in test2

# To run the third test (checks is_parameter method):

add this main in Formula.cpp:

int main() {
for (size_t a = 0; a < 100; ++a) {
std::ofstream outputFile("test3/File" + std::to_string(a + 1) + ".txt");
std::string str;
std::getline(std::cin, str);
std::shared_ptr<Formula> formula = std::make_shared<Formula>(str);
for (size_t i = 1; i < 10; ++i) {
for (size_t j = 0; j < ENGLISH_ALPHABIT_SIZE; ++j) {
char letter1 = 'a' + j;
char letter2 = 'A' + j;
std::string var1(i, letter1);
std::string var2(i, letter2);
if (formula->is_parameter(var1)) {
outputFile << var1 << "\n";
}
if (formula->is_parameter(var2)) {
outputFile << var2 << "\n";
}
}
}
outputFile.close();
}
}

run run.sh in test3

# To run the fourth test (checks find_all_parameters + find_all_variables + substitute + new_variable)

int main() {
for (size_t i = 0; i < 100; ++i) {
std::string formula_str;
std::getline(std::cin, formula_str);

    std::shared_ptr formula = std::make_shared<Formula>(formula_str);
    std::string new_var = formula->new_variable();
    std::set<std::string> parameters = formula->find_all_parameters();

    if (parameters.empty()) {
      continue;
    }

    size_t j = rand() % (parameters.size());

    auto start = parameters.begin();
    for (size_t k = 0; k < j; ++k) {
      ++start;
    }

    std::string first_param = *start;

    assert(first_param != new_var);

    std::string formula_before_substitution = formula->to_string();
    formula->substitute(first_param, new_var);
    formula->substitute(new_var, first_param);
    std::string formula_after_substitution = formula->to_string();
    assert(formula_before_substitution == formula_after_substitution);

}
}

and run this in MathPorver directory:

g++ -g -fsanitize=address --std=c++20 Formula.cpp

./a.out < test4/valid_formulas.txt

# To run the fifth test (added equivalence, added can_substitute, changed copy constructors, and copy assignment operators to actually copy the underlying data)

add this to main:

int main() {
for (size_t i = 0; i < 1000; ++i) {
std::string formula_str;
std::getline(std::cin, formula_str);
std::shared_ptr<Formula> formula = std::make_shared<Formula>(formula_str);
std::shared_ptr<Formula> formula2 = std::make_shared<Formula>("( a in a )");
*formula2 = *formula;
std::set<std::string> variables = formula->find_all_variables();
assert(!variables.empty());
auto start = variables.begin();
size_t j = rand() % variables.size();
size_t k = rand() % variables.size();
std::string old_var = *std::next(start, j);
std::string new_var = *std::next(start, k);
std::string absolutely_new_var = formula->new_variable();
if (formula->can_substitute(old_var, new_var) &&
formula->can_substitute(new_var, old_var)) {
formula->substitute(old_var, new_var);
formula->substitute(new_var, absolutely_new_var);
formula2->substitute(new_var, old_var);
formula2->substitute(old_var, absolutely_new_var);
std::string formula_str = formula->to_string();
std::string formula2_str = formula2->to_string();
if (formula_str != formula2_str) {
std::cout << i + 1 << " Wrong\n";
} else {
std::cout << i + 1 << " OK!\n";
}

    } else {
      formula->can_substitute(new_var, old_var);
      formula->substitute(old_var, new_var);
      formula->substitute(new_var, absolutely_new_var);
      formula2->substitute(new_var, old_var);
      formula2->substitute(old_var, absolutely_new_var);
      std::string formula_str = formula->to_string();
      std::string formula2_str = formula2->to_string();
      if (formula_str == formula2_str) {
        std::cout << i + 1 << " Wrong\n";
      } else {
        std::cout << i + 1 << " OK!\n";
      }
    }

}
}

and run this:

g++ -g -fsanitize=address --std=c++20 Formula.cpp

./a.out < test5/valid_formulas.txt
