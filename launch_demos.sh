export PATH=$PATH:/opt/qt5/bin
export UITK_THEME_PATH=.

if [ "$1" == "--phone" ]
then
	qmlscene -I modules demos/PhoneGallery.qml
else
	qmlscene -I modules demos/Gallery.qml
fi
