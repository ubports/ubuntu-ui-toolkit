TEMPLATE = subdirs

#######################################
# there tests are failing in armhf
# can not create QQuickView
!contains(QMAKE_HOST.arch,armv7l) {
    SUBDIRS += tst_theme_engine \
        tst_theme_engine_private \
        tst_components \
        tst_components_benchmark
}

SUBDIRS += tst_template \
    tst_units \
    tst_scaling_image_provider
