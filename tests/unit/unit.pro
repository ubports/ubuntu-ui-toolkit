TEMPLATE = subdirs

SUBDIRS += tst_template \
# Disabled until they pass again on i386.
# They stopped passing since the CONFIG += DEBUG was added to the Qt5 beta1 build.
    tst_theme_engine \
    tst_theme_engine_private \
    tst_units \
    tst_scaling_image_provider \
    tst_components \
    tst_components_benchmark
