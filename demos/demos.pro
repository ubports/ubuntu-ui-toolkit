TEMPLATE = subdirs

SUBDIRS += po

OTHER_FILES = $$system(ls *.qml) $$system(ls *.png) $$system(*.svg) $$system(*.js)

OTHER_FILES += *.qmltheme

other_files.path = /usr/lib/ubuntu-demos/uitk/demos
other_files.files = $$OTHER_FILES

desktop_file.path = /usr/share/applications
desktop_file.files = qt-components-ubuntu-demos.desktop qt-components-ubuntu-phone-demo.desktop

INSTALLS += other_files desktop_file
