Green='\033[0;32m'
Red='\033[0;31m'
NoColor='\033[0m' # No Color

make clean
make
if [[ $? -eq 0 ]]; then
    echo -e "${Green}Successfully compiled, ready to exec!${NoColor}"
    make exec
else 
    echo -e "${Red}Compilation error, have fun debugging!${NoColor}"
fi