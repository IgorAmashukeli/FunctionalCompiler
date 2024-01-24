python3 checker.py && cd .. && ./a.out < test1/mixed_formulas.txt > test1/mixed_indices2.txt && cd test1 && diff mixed_indices.txt mixed_indices2.txt
if [ $? -eq 0 ]; then
    echo "OK!"
else
    echo "Fail!"
fi