# MathProver

# To run first test:

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
