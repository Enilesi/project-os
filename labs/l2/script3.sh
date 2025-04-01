
time=$(date +"%H")
current_user=$(whoami)

if [ "$time" -ge 6 ] && [ "$time" -lt 12 ]; then
    echo "Good morning, $current_user"
elif [ "$time" -ge 12 ] && [ "$time" -lt 18 ]; then
    echo "Good afternoon, $current_user"
elif [ "$time" -ge 18 ] && [ "$time" -lt 22 ]; then
    echo "Good evening, $current_user"
else
    echo "Good night, $current_user"
fi
