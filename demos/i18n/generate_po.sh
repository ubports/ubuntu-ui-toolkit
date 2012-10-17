grep _\( ../*.qml | sed 's/.*_("\(.*\)").*/___\1___/' | grep \_\_\_ | sort -u | sed 's/___\(.*\)___/msgid "\1"\nmsgstr ""\n/'
