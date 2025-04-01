maxi=0
for arg in "$@"
do
    if [ "$arg" -gt "$maxi" ]
    then
        maxi=$arg
    fi
done

echo $maxi
