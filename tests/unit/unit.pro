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
    SUBDIRS += tst_theme_engine \
        tst_theme_engine_private \
        tst_components \
        tst_components_benchmark
#}

SUBDIRS += tst_units \
    tst_scaling_image_provider \
    tst_qquick_image_extension \
    tst_theme_engine_selector \
    tst_theme_engine_stylecache \
    tst_theme_engine_style \
    tst_inversemousearea \
    tst_performance \
    tst_ubuntu_shape \
    tst_arguments \
    tst_argument
