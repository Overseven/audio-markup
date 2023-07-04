QT += gui printsupport qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    processing/dir_provider.cpp \
    processing/js_functions_provider.cpp \
    processing/js_global_variable_provider.cpp \
    processing/markup_list_model.cpp \
    processing/markup_provider.cpp \
    processing/samples_provider.cpp \
    widgets/graph_controls.cpp \
    widgets/js_case_window.cpp \
    widgets/markup_window.cpp \
    common/markup.cpp \
    processing/executor.cpp \
    main.cpp \
    mainwindow.cpp \
    qcustomplot/qcustomplot.cpp \
    widgets/result_window.cpp

HEADERS += \
    common/audio_file.h \
    common/types.h \
    interfaces/i_js_case_provider.h \
    interfaces/i_markup_provider.h \
    interfaces/i_samples_provider.h \
    processing/dir_provider.h \
    processing/js_functions_provider.h \
    processing/js_global_variable_provider.h \
    processing/markup_list_model.h \
    processing/markup_provider.h \
    processing/samples_provider.h \
    widgets/graph_controls.h \
    widgets/js_case_window.h \
    widgets/markup_window.h \
    common/markup.h \
    common/settings.h \
    processing/common.h \
    processing/executor.h \
    mainwindow.h \
    qcustomplot/qcustomplot.h \
    widgets/result_window.h

FORMS += \
    mainwindow.ui \
    widgets/graph_controls.ui \
    widgets/js_case_window.ui \
    widgets/markup_window.ui \
    widgets/result_window.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
