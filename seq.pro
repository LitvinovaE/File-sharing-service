QT += network widgets core
CONFIG += c++11

HEADERS       = sslclient.h
SOURCES       = sslclient.cpp \
                main.cpp

# install
target.path = $$[QT_INSTALL_EXAMPLES]/network/fortuneclient
INSTALLS += target
