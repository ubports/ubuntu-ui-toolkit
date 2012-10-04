grep tr\( * | sed 's/.*"\(.*\)"[^"]*$/\1/' | sort | uniq
