#grep tr\( * | sed 's/.*"\(.*\)"[^"]*$/\1/' | sort | uniq
grep tr\( * | sed 's/.*i18n\.tr("\(.*\)").*/___\1___/' | grep \_\_\_ | sort -u | sed 's/___\(.*\)___/msgid "\1"\nmsgstr ""\n/'
