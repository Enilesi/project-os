arg1=$1
arg=$2
arg2=$3

case "$arg" in
"/")  echo $((arg1 / arg2));;
"*")  echo $((arg1 * arg2));;
"+")  echo $((arg1 + arg2));;
"-")  echo $((arg1 - arg2));;
esac


