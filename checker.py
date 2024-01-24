import random

def generate_variable(level):
    letters = "abcdfghjklmnopqrstuvwxyzABCDFGHJKLMNOPQRSTUVWXYZ"
    base_letter = random.choice(letters)
    return ''.join([base_letter for _ in range(level)])

def generate_formula(depth):
    if depth <= 0:
        return f"( {generate_variable(random.randint(1, 8))} in {generate_variable(random.randint(1, 8))} )"

    operators = ["exists", "forall", "vee", "wedge", "in", "neg", "to"]
    operator = random.choice(operators)

    if operator in ["exists", "forall"]:
        variable = generate_variable(random.randint(1, 8))
        sub_formula = generate_formula(depth - 1)
        return f"( {operator} {variable} {sub_formula} )"
    elif operator in ["vee", "wedge", "to"]:
        left_subformula = generate_formula(depth - 1)
        right_subformula = generate_formula(depth - 1)
        return f"( {left_subformula} {operator} {right_subformula} )"
    elif operator == "in":
        left_variable = generate_variable(random.randint(1, 4))
        right_variable = generate_variable(random.randint(1, 4))
        sub_formula = f"{left_variable} {operator} {right_variable}"
        return f"( {sub_formula} )"
    elif operator == "neg":
        sub_formula = generate_formula(depth - 1)
        return f"( neg {sub_formula} )"

with open("formulas.txt", "w") as file:
    formulas = [generate_formula(random.randint(1, 8)) for _ in range(10000)]
    for formula in formulas:
        file.write(f" {formula} \n")
