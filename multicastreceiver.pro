QT = core

CONFIG += c++17 cmdline

QT += network
requires(qtConfig(udpsocket))

HEADERS       = receiver.h
SOURCES       = receiver.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/multicastreceiver
INSTALLS += target

