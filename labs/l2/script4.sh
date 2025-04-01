directory=$1
permission=$2

cd "$directory" || { echo "Failed to change directory"; exit 1; }

find . -name '*.txt' -exec chmod "$permission" {} +
