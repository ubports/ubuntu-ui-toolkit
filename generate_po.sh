#grep tr\( * | sed 's/.*"\(.*\)"[^"]*$/\1/' | sort | uniq
grep tr\( * | sed 's/.*i18n\.tr("\(.*\)").*/___\1___/' | sort | uniq | grep \_\_\_ | sed 's/___\(.*\)___/msgid "\1"\nmsgstr "\1"\n/'
