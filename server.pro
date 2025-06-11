QT += core network sql
QT += gui

CONFIG += c++17 console
CONFIG -= app_bundle

SOURCES += \
    main.cpp \
    server.cpp \
    database/databasesingleton.cpp \
    handlers/authhandler.cpp \
    handlers/requesthandler.cpp

HEADERS += \
    server.h \
    database/databasesingleton.h \
    handlers/authhandler.h \
    handlers/requesthandler.h

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