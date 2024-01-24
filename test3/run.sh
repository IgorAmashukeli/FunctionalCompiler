python checker.py && cd .. && g++ -g -fsanitize=address --std=c++20 Formula.cpp && ./a.out < test3/valid_formulas.txt && cd test3


for ((i=1; i<=100; i++)); do
     input_file="File${i}.txt";
     output_file="Sorted_File${i}.txt";
     sort "$input_file" > "$output_file";
done


for ((i=1; i<=100; i++)); do
     input_file="file${i}.txt";
     output_file="sorted_File${i}.txt";
     sort "$input_file" > "$output_file";
done


for ((i=1; i<=100; i++)); do
    original_file="sorted_File${i}.txt"
    sorted_file="Sorted_File${i}.txt"
    diff "$original_file" "$sorted_file"
done


if [ $? -eq 0 ]; then
    echo "OK!"
else
    echo "Fail!"
fi

