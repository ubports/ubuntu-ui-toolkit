TEMPLATE = subdirs

OTHER_FILES = $$system(ls *.qml) $$system(ls *.png) $$system(*.svg) $$system(*.js)

other_files.path = /usr/lib/ubuntu-demos/uitk/demos
other_files.files = $$OTHER_FILES

desktop_file.path = /usr/share/applications
desktop_file.files = qt-components-ubuntu-demos.desktop

INSTALLS += other_files desktop_file
