TEMPLATE = subdirs

SUBDIRS += testparser

#######################################
# these tests are failing in armhf/qemu
# can not create QQuickView
# tested with Qt 5 beta 1 and beta 2
#
# https://bugs.launchpad.net/qemu-linaro/+bug/1084148
#######################################
#!contains(QMAKE_HOST.arch,armv7l) {
    SUBDIRS += components \
        components_benchmark
#}

SUBDIRS += \
    visual \
    ubuntu_shape \
    page \
    test \
    iconprovider \
    inversemousearea \
    recreateview \
    statesaver \
    deprecated_theme_engine \
    orientation \
#    layouts \ # FIXME: Breaks on Yakkety. See bug #1625137.
    mousefilters \
    animator \
    serviceproperties \
    subtheming \
    swipearea \
    touchregistry \
    bottomedge \
    asyncloader \
    custom_qpa \
    units \
    scaling_image_provider \
    qquick_image_extension \
    performance \
    mainview11 \
    mainview13 \
#    i18n \ FIXME: breaks xenial
    arguments \
    argument \
    alarms \
    theme \
    quickutils \
    tree
