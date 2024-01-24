import random


def generate_correct_variable(level):
    letters = "abcdfghjklmnopqrstuvwxyzABCDFGHJKLMNOPQRSTUVWXYZ"
    base_letter = random.choice(letters)
    return ''.join([base_letter for _ in range(level)])

def generate_formula(depth):
    if depth <= 0:
        return f"( {generate_correct_variable(random.randint(1, 8))} in {generate_correct_variable(random.randint(1, 8))} )"

    operators = ["exists", "forall", "vee", "wedge", "in", "neg", "to"]
    operator = random.choice(operators)

    if operator in ["exists", "forall"]:
        variable = generate_correct_variable(random.randint(1, 8))
        sub_formula = generate_formula(depth - 1)
        return f"( {operator} {variable} {sub_formula} )"
    elif operator in ["vee", "wedge", "to"]:
        left_subformula = generate_formula(depth - 1)
        right_subformula = generate_formula(depth - 1)
        return f"( {left_subformula} {operator} {right_subformula} )"
    elif operator == "in":
        left_variable = generate_correct_variable(random.randint(1, 4))
        right_variable = generate_correct_variable(random.randint(1, 4))
        sub_formula = f"{left_variable} {operator} {right_variable}"
        return f"( {sub_formula} )"
    elif operator == "neg":
        sub_formula = generate_formula(depth - 1)
        return f"( neg {sub_formula} )"

def generate_invalid_formula(depth):
    if depth <= 0:
        return ""

    operators = ["exists", "forall", "vee", "wedge", "in", "neg", "to"]
    operator = random.choice(operators)

    if operator in ["exists", "forall"]:
        variable = generate_variable(random.randint(1, 4))
        sub_formula = generate_invalid_formula(depth - 1)
        use_open_parenthesis = random.choice([True, False])
        use_close_parenthesis = random.choice([True, False])
        if use_open_parenthesis:
            operator = f"({operator}"
        if use_close_parenthesis:
            sub_formula = f"{sub_formula})"
        return f"{operator} {variable} {sub_formula}"
    elif operator in ["vee", "wedge", "to"]:
        left_subformula = generate_invalid_formula(depth - 1)
        right_subformula = generate_invalid_formula(depth - 1)
        
        use_open_parenthesis = random.choice([True, False])
        use_close_parenthesis = random.choice([True, False])

        if use_open_parenthesis:
            left_subformula = f"({left_subformula}"
        if use_close_parenthesis:
            right_subformula = f"{right_subformula})"
            
        return f"{left_subformula} {operator} {right_subformula}"
    elif operator == "in":
        left_variable = generate_variable(random.randint(1, 4))
        right_variable = generate_variable(random.randint(1, 4))

        use_open_parenthesis = random.choice([True, False])
        use_close_parenthesis = random.choice([True, False])

        if use_open_parenthesis:
            left_variable = f"({left_variable}"
        if use_close_parenthesis:
            right_variable = f"{right_variable})"
        
        return f"{left_variable} {operator} {right_variable}"
    elif operator == "neg":
        sub_formula = generate_invalid_formula(depth - 1)
        
        use_parentheses = random.choice([True, False])

        if use_parentheses:
            return f"neg({sub_formula})"
        else:
            return f"neg {sub_formula}"

def generate_variable(level):
    letters = "abcdefghjklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
    base_letter = random.choice(letters)
    return ''.join([base_letter for _ in range(level)])


with open("mixed_formulas.txt", "w") as formulas_file, open("mixed_indices.txt", "w") as indices_file:
    for index in range(10000):
        is_valid = random.choice([True, False])
        if is_valid:
            formula = generate_formula(random.randint(1, 4))
            formulas_file.write(f"{formula}\n")
            indices_file.write(f"{index + 1} valid\n")
        else:
            formula = generate_invalid_formula(random.randint(1, 4))
            formulas_file.write(f"{formula}\n")
            indices_file.write(f"{index + 1} invalid\n")
