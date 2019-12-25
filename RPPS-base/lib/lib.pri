#-------------------------------------------------------------------------------
# Description: Файл подключения библиотеки
#-------------------------------------------------------------------------------
DS_IMPOSTER_FOLDER_NAME = devImposter
include($$PWD/rpps_config.pri)
DS_LIB_NAME = RPPTDevSupport$${QT_MAJOR_VERSION}$${DS_LIB_SUFFIX}

#-------------------------------------------------------------------------------
IMPOSTER_HEADERS = \
    $$DS_COMMON_INC/*.h \
    $$DS_TRANSPORT_INC/*.h \
    $$DS_BUFFER_INC/*.h \
    $$DS_CONCRETE_INC/*.h \
    $$DS_IMPOSTER_INC/*.h

#!contains(HEADERS,${IMPOSTER_HEADERS}) HEADERS += $${IMPOSTER_HEADERS}

#-------------------------------------------------------------------------------
INCLUDEPATH += \
    $${DS_INC} \
#    $${DS_EXTERNAL_INC} \
    $${DS_COMMON_INC} \
    $${DS_IMPOSTER_INC} \
    $${DS_TRANSPORT_INC} \
    $${DS_BUFFER_INC} \
    $${DS_CONCRETE_INC}

DEPENDPATH  += \
    $${DS_INC} \
#    $${DS_EXTERNAL_INC} \
    $${DS_COMMON_INC} \
    $${DS_IMPOSTER_INC} \
    $${DS_TRANSPORT_INC} \
    $${DS_BUFFER_INC} \
    $${DS_CONCRETE_INC}

DEPENDPATH  += $${DS_INSTALL_LIBS} #$${DS_EXTERNAL_LIB}

#-------------------------------------------------------------------------------
!contains(QT, network): QT += network
!contains(QT,gui): QT += gui

greaterThan(QT_MAJOR_VERSION,4): {
    !contains(QT, serialport): QT += serialport
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++0x
    CONFIG += serialport
    LIBS += -lQtSerialPort #-L$${DS_EXTERNAL_LIB}
}

!contains(LIBS, $${DS_LIB_NAME} ): LIBS += -L$${DS_INSTALL_LIBS} -l$${DS_LIB_NAME}

