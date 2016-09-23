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
    layouts \
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
    mainview \
    i18n \
    arguments \
    argument \
    alarms \
    theme \
    quickutils \
    tree
