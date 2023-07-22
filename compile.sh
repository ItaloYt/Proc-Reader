mapfile -d '' src < <(find src/ -name "*.c" -print0)
obj=()

for file in ${src[@]}; do
  obj+=(${file/[.]c/.o})
  clang -c -o ${obj[-1]} $file -Iinclude -O3 -Wall -Wextra -Wpedantic -Werror
done

clang -o proc-reader ${obj[@]} -O3 -Wall -Wextra -Wpedanticn -Werror

rm ${obj[@]} -rf
