QT += core gui network
# Удаляем модуль core5compat, так как он не установлен и не требуется
# QT += core5compat
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    client/clientsingleton.cpp \
    dialogs/authdialog.cpp \
    dialogs/cryptodialog.cpp \
    dialogs/chordmethoddialog.cpp \
    dialogs/steganographydialog.cpp

HEADERS += \
    mainwindow.h \
    client/clientsingleton.h \
    dialogs/authdialog.h \
    dialogs/cryptodialog.h \
    dialogs/chordmethoddialog.h \
    dialogs/steganographydialog.h

FORMS += \
    mainwindow.ui \
    dialogs/authdialog.ui \
    dialogs/cryptodialog.ui \
    dialogs/chordmethoddialog.ui \
    dialogs/steganographydialog.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

# Include common library
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../common/release/ -lcommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../common/debug/ -lcommon
else:unix: LIBS += -L$$OUT_PWD/../common/ -lcommon

INCLUDEPATH += $$PWD/../common
DEPENDPATH += $$PWD/../common

RESOURCES += \
    resources.qrc 