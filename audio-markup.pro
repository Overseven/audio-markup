QT += gui printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    common/markup.cpp \
    main.cpp \
    mainwindow.cpp \
    markup/graphcontrols.cpp \
    markup/markupwindow.cpp \
    qcustomplot/qcustomplot.cpp

HEADERS += \
    common/AudioFile.h \
    common/markup.h \
    common/settings.h \
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
