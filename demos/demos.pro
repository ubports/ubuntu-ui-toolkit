TEMPLATE = subdirs

OTHER_FILES = $$system(ls *.qml) $$system(ls *.png)

other_files.path = /opt/qt-components-ubuntu/demos
other_files.files = $$OTHER_FILES

desktop_file.path = /usr/share/applications
desktop_file.files = qt-components-ubuntu-demos.desktop

INSTALLS += other_files desktop_file
