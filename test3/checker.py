import random

def generate_valid_formula(depth):
    if depth <= 0:
        var1 = generate_variable(random.randint(1, 4))
        var2 = generate_variable(random.randint(1, 4))
        return f"( {var1} in {var2} )", {var1, var2}

    operators = ["exists", "forall", "vee", "wedge", "to"]
    operator = random.choice(operators)

    if operator in ["exists", "forall"]:
        variable = generate_variable(random.randint(1, 4))
        sub_formula, sub_formula_free_vars = generate_valid_formula(depth - 1)
        sub_formula_free_vars.discard(variable)
        return f"( {operator} {variable} {sub_formula} )", sub_formula_free_vars
    elif operator in ["vee", "wedge", "to"]:
        left_subformula, free_vars_left_subformula = generate_valid_formula(depth - 1)
        right_subformula, free_vars_right_subformula = generate_valid_formula(depth - 1)
        return f"( {left_subformula} {operator} {right_subformula} )", free_vars_left_subformula.union(free_vars_right_subformula)
    elif operator == "neg":
        sub_formula, sub_formula_free_vars = generate_valid_formula(depth - 1)
        return f"( neg {sub_formula} )", sub_formula_free_vars

def generate_variable(level):
    letters = "abcdfghjklmnopqrstuvwxyzABCDFGHJKLMNOPQRSTUVWXYZ"
    base_letter = random.choice(letters)
    return ''.join([base_letter for _ in range(level)])

with open("valid_formulas.txt", "w") as formulas_file:
    for index in range(100):
        _, free_vars = generate_valid_formula(random.randint(1, 8))
        formulas_file.write(f"{_}\n")

        with open(f"file{index + 1}.txt", "w") as file:
            for free_var in sorted(free_vars):
                file.write(f"{free_var}\n")
