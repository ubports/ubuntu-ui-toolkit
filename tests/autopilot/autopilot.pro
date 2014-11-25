TEMPLATE=aux

#show all files in the QtC project tree
SOURCES += $$system(find $${PWD} -name *.qml)
SOURCES += $$system(find $${PWD} -name *.py)
