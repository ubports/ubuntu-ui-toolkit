TEMPLATE = subdirs

SUBDIRS += testparser

#######################################
# these tests are failing in armhf/qemu
# can not create QQuickView
# tested with Qt 5 beta 1 and beta 2
#######################################
!contains(QMAKE_HOST.arch,armv7l) {
    SUBDIRS += tst_theme_engine \
        tst_theme_engine_private \
        tst_components \
        tst_components_benchmark
}

SUBDIRS += tst_template \
    tst_units \
    tst_scaling_image_provider \
    tst_qquick_image_extension

