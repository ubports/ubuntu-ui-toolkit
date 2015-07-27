TEMPLATE=aux
TARGET=documentation

load(qt_build_config)
load(qt_functions)

#find the path to qdoc and qhelpgenerator
qtPrepareTool(QDOC, qdoc)
qtPrepareTool(QHELPGENERATOR, qhelpgenerator)

DOC_SRC = $$ROOT_SOURCE_DIR
equals(QT_MAJOR_VERSION, 5):lessThan(QT_MINOR_VERSION, 2) {
    DOC_SRC = $$ROOT_SOURCE_DIR/documentation
}

DOC_PATH=$$shadowed($$ROOT_SOURCE_DIR)/documentation
generate_docs.commands = cd $$ROOT_SOURCE_DIR; SRC=$$ROOT_SOURCE_DIR/documentation BLD=$$ROOT_BUILD_DIR/documentation $$ROOT_SOURCE_DIR/documentation/docs.sh \'$$QDOC\' \'$$QHELPGENERATOR\' $$DOC_PATH

#install the online docs only when building outside of Qt
force_independent: {
    install_docs.files = $$shadowed($$ROOT_SOURCE_DIR)/documentation/html
    install_docs.path = /usr/share/ubuntu-ui-toolkit/doc
    install_docs.CONFIG += no_check_exist directory no_build
    install_docs.depends = docs
    INSTALLS += install_docs
}

install_qch.files = $$shadowed($$ROOT_SOURCE_DIR)/documentation/ubuntuuserinterfacetoolkit.qch
install_qch.path =  $$[QT_INSTALL_DOCS]
install_qch.CONFIG += no_check_exist no_build
install_qch.depends = docs
INSTALLS += install_qch

OTHER_FILES += *.qdocconf \
               docs.sh

