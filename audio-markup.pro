QT += gui printsupport qml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/markup.cpp \
    executor/executor.cpp \
    main.cpp \
    mainwindow.cpp \
    markup/graphcontrols.cpp \
    markup/markupwindow.cpp \
    qcustomplot/qcustomplot.cpp

HEADERS += \
    common/AudioFile.h \
    common/markup.h \
    common/settings.h \
    executor/executor.h \
    mainwindow.h \
    markup/graphcontrols.h \
    markup/markupwindow.h \
    qcustomplot/qcustomplot.h

FORMS += \
    mainwindow.ui \
    markup/graphcontrols.ui \
    markup/markupwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    functions/compression/limiter.js \
    functions/compression/normalizer.js \
    functions/convolution/attack_filter.js \
    functions/convolution/convolution.js \
    functions/convolution/derivative_fall.js \
    functions/convolution/derivative_filter.js \
    functions/convolution/derivative_raise.js \
    functions/convolution/high_pass_filter.js \
    functions/convolution/low_pass_filter.js \
    functions/convolution/mean.js \
    functions/gate/bi_dir_gate.js \
    functions/gate/simple_gate.js \
    functions/utils/gates_to_signal_form.js \
    functions/utils/merge_gates.js \
    functions/utils/derivative.js \
    functions/utils/abs.js \
    functions/utils/reduce_derivative_attack.js \
    functions/window/create_attack_window.js \
    functions/window/create_derivative_window.js \
    functions/window/create_fall_window.js \
    functions/window/create_high_pass_window.js \
    functions/window/create_low_pass_window.js \
    functions/window/create_mean_window.js \
    functions/window/create_raise_window.js
