import random

def generate_valid_formula(depth):
    if depth <= 0:
        var1 = generate_variable()
        var2 = generate_variable()

        return f"( {var1} in {var2} )"

    operators = ["exists", "forall", "vee", "wedge", "to", "leftrightarrow"]
    operator = random.choice(operators)

    if operator in ["exists", "forall"]:
        variable = generate_variable()
        return f"( {operator} {variable} {generate_valid_formula(depth - 1)} )"

    elif operator in ["vee", "wedge", "to", "leftrightarrow"]:
        left_subformula = generate_valid_formula(depth - 1)
        right_subformula = generate_valid_formula(depth - 1)
        return f"( {left_subformula} {operator} {right_subformula} )"

    elif operator == "neg":
        sub_formula = generate_valid_formula(depth - 1)
        return f"( neg {sub_formula} )"

def generate_variable():
    allowed_letters = "abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    allowed_letters = [letter for letter in allowed_letters if letter not in {'e', 'E', 'I'}]
    base_letter = random.choice(allowed_letters)
    return base_letter

# Example usage
with open("valid_formulas.txt", "w") as formulas_file:
    for index in range(1000):
        formula = generate_valid_formula(random.randint(1, 6))
        formulas_file.write(f"{formula}\n")
