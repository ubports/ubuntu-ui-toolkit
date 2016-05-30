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
    SUBDIRS += tst_components \
        tst_components_benchmark
#}

SUBDIRS += \
    tst_visual \
    tst_ubuntu_shape \
    tst_page \
    tst_test \
    tst_iconprovider \
    tst_inversemousearea \
    tst_recreateview \
    tst_statesaver \
    tst_deprecated_theme_engine \
    tst_orientation \
    tst_layouts \
    tst_mousefilters \
    tst_animator \
    tst_serviceproperties \
    tst_subtheming \
    tst_swipearea \
    tst_touchregistry \
    tst_bottomedge \
    tst_asyncloader \
    custom_qpa \
    tst_units \
    tst_scaling_image_provider \
    tst_qquick_image_extension \
    tst_performance \
    tst_mainview \
    tst_i18n \
    tst_arguments \
    tst_argument \
    tst_alarms \
    tst_theme \
    tst_quickutils \
    tst_tree
