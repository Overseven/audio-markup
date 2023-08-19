QT += gui printsupport qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/utils.cpp \
    helpers/audio_view_cache.cpp \
    helpers/processing_result_cache.cpp \
    providers/dir_provider.cpp \
    providers/js_functions_provider.cpp \
    providers/js_global_variable_provider.cpp \
    processing/markup_list_model.cpp \
    providers/js_script_provider.cpp \
    providers/markup_provider.cpp \
    providers/processing_result_provider.cpp \
    providers/samples_provider.cpp \
    ui/widgets/audio_file_multi_selector.cpp \
    ui/widgets/audio_view_mode_selector.cpp \
    ui/widgets/js_script_multi_selector.cpp \
    ui/widgets/js_script_selector.cpp \
    ui/windows/js_script_window.cpp \
    ui/windows/main_window.cpp \
    ui/windows/markup_window.cpp \
    ui/windows/result_window.cpp \
    ui/widgets/audio_file_selector.cpp \
    ui/widgets/graph_controls.cpp \
    ui/qcustomplot/qcustomplot.cpp \
    processing/executor.cpp \
    main.cpp \
    ui/windows/stats_window.cpp

HEADERS += \
    common/audio_file.h \
    common/types.h \
    common/utils.h \
    helpers/audio_view_cache.h \
    helpers/processing_result_cache.h \
    interfaces/i_js_script_provider.h \
    interfaces/i_markup_provider.h \
    interfaces/i_processing_result_provider.h \
    interfaces/i_samples_provider.h \
    providers/dir_provider.h \
    providers/js_functions_provider.h \
    providers/js_global_variable_provider.h \
    processing/markup_list_model.h \
    providers/js_script_provider.h \
    providers/markup_provider.h \
    providers/processing_result_provider.h \
    providers/samples_provider.h \
    ui/widgets/audio_file_multi_selector.h \
    ui/widgets/audio_view_mode_selector.h \
    ui/widgets/js_script_multi_selector.h \
    ui/widgets/js_script_selector.h \
    ui/windows/js_script_window.h \
    ui/windows/main_window.h \
    ui/windows/markup_window.h \
    ui/windows/result_window.h \
    ui/widgets/audio_file_selector.h \
    ui/widgets/graph_controls.h \
    ui/qcustomplot/qcustomplot.h \
    common/settings.h \
    processing/common.h \
    processing/executor.h \
    ui/windows/stats_window.h

FORMS += \
    ui/widgets/audio_file_multi_selector.ui \
    ui/widgets/audio_view_mode_selector.ui \
    ui/widgets/js_script_multi_selector.ui \
    ui/widgets/js_script_selector.ui \
    ui/windows/js_script_window.ui \
    ui/windows/main_window.ui \
    ui/windows/markup_window.ui \
    ui/windows/result_window.ui \
    ui/widgets/audio_file_selector.ui \
    ui/widgets/graph_controls.ui \
    ui/windows/stats_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    js/functions/utils/debug_ranges.js
