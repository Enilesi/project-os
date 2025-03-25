

input="$1"
output="$2"

> "$output"

while IFS= read -r line; do
    if echo "$line" | grep -E -q '^[a-zA-Z]+[a-zA-Z0-9.-]*@[a-zA-Z0-9]+\.[a-zA-Z]{2,};([0-9]{1,3}\.){3}[0-9]{1,3}$'; then
        echo "OK" >> "$output"
    else
        echo "ERROR" >> "$output"
    fi
done < "$input"
