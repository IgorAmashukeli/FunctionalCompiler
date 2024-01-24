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
