python3 checker.py < correct_formulas.txt && cd .. && ./a.out < test2/correct_formulas.txt > test2/variables.txt && cd test2 && sort variables_checker.txt > variables_checker_sort.txt && sort variables_checker.txt > variables_checker_sort.txt && diff variables_sort.txt variables_checker_sort.txt
if [ $? -eq 0 ]; then
    echo "OK!"
else
    echo "Fail!"
fi