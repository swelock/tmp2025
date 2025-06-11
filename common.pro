QT += core
QT += gui
# Модуль core5compat недоступен в Docker-образе Ubuntu 20.04 с Qt 5.12
# QT += core5compat

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++17

SOURCES += \
    crypto/tripledes.cpp \
    crypto/sha384.cpp \
    math/chordmethod.cpp \
    steganography/steganography.cpp \
    network/protocol.cpp

HEADERS += \
    crypto/tripledes.h \
    crypto/sha384.h \
    math/chordmethod.h \
    steganography/steganography.h \
    network/protocol.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target 