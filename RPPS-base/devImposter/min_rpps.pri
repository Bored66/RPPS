DEFINES += EXCLUDE_SERIAL

Transport = $$PWD_TR/src/Transport
ConcreteTransport = $$PWD_TR/src/Transport/Concrete
Reader = $$PWD_TR/src
Buffer = $$PWD_TR/src/Transport/Buffer
Common = $$PWD_TR/src/common
message($$Transport)
exists($$Transport/Transport.cpp) {

SOURCES += \
            $$Transport/Transport.cpp \
            $$Transport/TransportFactory.cpp \
            $$Transport/QueuedTransport.cpp \
#            $$ConcreteTransport/TerminalTransport.cpp \
#            $$ConcreteTransport/SerialTransport.cpp \
            $$ConcreteTransport/TcpClientTransport.cpp \
            $$ConcreteTransport/TcpServerTransport.cpp \
            $$ConcreteTransport/HttpTransport.cpp \
            $$ConcreteTransport/UdpTransport.cpp \
            $$Transport/TransportReader.cpp \
            $$Reader/XmlReader.cpp \
            $$Reader/AppSettings.cpp \
            $$Reader/CalcCRC.cpp \
            $$Reader/crc.cpp \
            $$Reader/FieldDescriptor.cpp \
            $$Reader/PacketDispatcher.cpp \
            $$Reader/Utils.cpp \
            $$Common/CommonUtils.cpp \
            $$Buffer/Transform.cpp \
            $$Buffer/TransportTransform.cpp

Transport = $$PWD_TR/include/Transport
ConcreteTransport = $$PWD_TR/include/Transport/Concrete
Buffer = $$PWD_TR/include/Transport/Buffer
Reader = $$PWD_TR/include
Common = $$PWD_TR/include/common

HEADERS  += \
            $$Transport/Transport.h \
            $$Transport/TransportFactory.h \
            $$Transport/QueuedTransport.h \
#            $$ConcreteTransport/TerminalTransport.h \
#            $$ConcreteTransport/SerialTransport.h \
            $$ConcreteTransport/TcpClientTransport.h \
            $$ConcreteTransport/TcpServerTransport.h \
            $$Transport/TransportReader.h \
            $$ConcreteTransport/HttpTransport.h \
            $$ConcreteTransport/UdpTransport.h \
            $$Reader/XmlReader.h \
            $$Reader/AppSettings.h \
            $$Reader/CalcCRC.h \
            $$Reader/crc.h \
            $$Reader/FieldDescriptor.h \
            $$Reader/PacketDispatcher.h \
            $$Reader/Utils.h \
            $$Common/CommonUtils.h \
            $$Buffer/Transform.h \
            $$Buffer/TransportTransform.h

INCLUDEPATH += $$Transport \
                $$ConcreteTransport \
                $$Reader \
                $$Common \
                $$Buffer
} else {
warning("Unable to find RPPS source files")

}
