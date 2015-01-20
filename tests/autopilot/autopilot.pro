TEMPLATE=aux

#show all files in the QtC project tree
OTHER_FILES += $$system(find $${PWD} -name *.qml)
OTHER_FILES += $$system(find $${PWD} -name *.py)
