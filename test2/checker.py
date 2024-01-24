with open("variables_checker.txt", "w") as variables_file:
    for i in range(100):
        formula = input()
        words = formula.split()

        specific = ["forall", "exists", "in", "vee", "wedge", "neg", "to"]

        for word in set(words):
            if word != "(" and word != ")" and (not (word in specific)):
                variables_file.write(f"{word}\n")