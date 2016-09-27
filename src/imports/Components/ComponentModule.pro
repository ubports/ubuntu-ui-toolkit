CXX_MODULE = qml
TARGET  = UbuntuComponents
TARGETPATH = Ubuntu/Components
IMPORT_VERSION = 0.1

include(plugin/plugin.pri)

#artwork files
ARTWORK_FILES = $$system(find artwork -type f)

QML_FILES += $$ARTWORK_FILES

#1.0
QML_FILES += 1.0/Button.qml \
             1.0/Icon.qml \
             1.0/ProgressBar.qml \
             1.0/UbuntuColors.qml

#1.1
QML_FILES += 1.1/Button.qml \
             1.1/Haptics.qml \
             1.1/Icon.qml \
             1.1/ProgressBar.qml \
             1.1/UbuntuColors.qml

#1.2
QML_FILES += 1.2/AbstractButton.qml \
             1.2/ActionList.qml \
             1.2/ActivityIndicator.qml \
             1.2/AnimatedItem.qml \
             1.2/AppHeader.qml \
             1.2/Captions.qml \
             1.2/CheckBox.qml \
             1.2/ComboButton.qml \
             1.2/CrossFadeImage10.qml \
             1.2/CrossFadeImage11.qml \
             1.2/dateUtils.js \
             1.2/DraggingArea.qml \
             1.2/Header.qml \
             1.2/InputHandler.qml \
             1.2/Label.qml \
             1.2/MainView12.qml \
             1.2/MainViewBase.qml \
             1.2/MainView.qml \
             1.2/Object.qml \
             1.2/OptionSelectorDelegate.qml \
             1.2/OptionSelector.qml \
             1.2/OrientationHelper.qml \
             1.2/Page10.qml \
             1.2/Page11.qml \
             1.2/PageHeadConfiguration.qml \
             1.2/PageHeadSections.qml \
             1.2/PageHeadState.qml \
             1.2/PageStack.qml \
             1.2/PageTreeNode.qml \
             1.2/PageWrapper.qml \
             1.2/PageWrapperUtils.js \
             1.2/Panel.qml \
             1.2/PullToRefresh.qml \
             1.2/Scrollbar.qml \
             1.2/Slider.qml \
             1.2/sliderUtils.js \
             1.2/stack.js \
             1.2/Switch.qml \
             1.2/TabBar.qml \
             1.2/Tab.qml \
             1.2/Tabs.qml \
             1.2/TextArea.qml \
             1.2/TextCursor.qml \
             1.2/TextField.qml \
             1.2/TextInputPopover.qml \
             1.2/ToolbarButton.qml \
             1.2/ToolbarItems.qml \
             1.2/Toolbar.qml \
             1.2/UbuntuListView11.qml \
             1.2/UbuntuListView.qml \
             1.2/UbuntuNumberAnimation.qml

#1.3
QML_FILES += 1.3/ActionBar.qml \
             1.3/ActivityIndicator.qml \
	     1.3/AdaptivePageLayout.qml \
             1.3/AnimatedItem.qml \
             1.3/AppHeader.qml \
             1.3/Button.qml \
             1.3/Captions.qml \
             1.3/CheckBox.qml \
             1.3/ComboButton.qml \
             1.3/CrossFadeImage.qml \
             1.3/dateUtils.js \
             1.3/DraggingArea.qml \
             1.3/InputHandler.qml \
             1.3/MainView.qml \
             1.3/OptionSelectorDelegate.qml \
             1.3/OptionSelector.qml \
             1.3/OrientationHelper.qml \
             1.3/PageHeadConfiguration.qml \
             1.3/PageHeadSections.qml \
             1.3/PageHeadState.qml \
             1.3/Page.qml \
             1.3/PageStack.qml \
             1.3/pageUtils.js \
             1.3/Panel.qml \
             1.3/ProgressBar.qml \
             1.3/PullToRefresh.qml \
             1.3/Scrollbar.qml \
             1.3/Sections.qml \
             1.3/Slider.qml \
             1.3/Switch.qml \
             1.3/TabBar.qml \
             1.3/Tab.qml \
             1.3/Tabs.qml \
             1.3/TextArea.qml \
             1.3/TextCursor.qml \
             1.3/TextField.qml \
             1.3/TextInputPopover.qml \
             1.3/Toolbar.qml \
             1.3/ToolbarButton.qml \
             1.3/ToolbarItems.qml \
             1.3/UbuntuColors.qml \
             1.3/UbuntuListView.qml \
             1.3/UbuntuNumberAnimation.qml \
             1.3/ListItemPopover.qml \
             1.3/PageColumn.qml \
             1.3/PageColumnsLayout.qml \
             1.3/ProgressionSlot.qml \
             1.3/ScrollView.qml \
             1.3/PageHeader.qml \
             1.3/Icon.qml

OTHER_FILES+= qmldir \
             1.3/CrossFadeImage.qdoc \
             1.3/PageHeadConfiguration.qdoc \
             1.3/MainView.qdoc \
             1.3/Icon.qdoc

load(ubuntu_qml_plugin)
