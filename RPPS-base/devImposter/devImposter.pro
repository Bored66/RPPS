#-------------------------------------------------
#
# Project created by QtCreator 2015-07-30T17:41:48
#
# Description: Файл проекта
#
#-------------------------------------------------
QT       -= gui

!exists( $$PWD/../devsupport_env.pri ): error( Cannot build project without $$PWD/../devsupport_env.pri file)
include( $$PWD/../devsupport_env.pri )

TARGET = $${DS_LIB_NAME}
TEMPLATE = lib

IMPOSTER_HEADERS = \
    $$DS_COMMON_INC/*.h \
    $$DS_TRANSPORT_INC/*.h \
    $$DS_BUFFER_INC/*.h \
    $$DS_CONCRETE_INC/*.h \
    $$DS_IMPOSTER_INC/*.h

IMPOSTER_SOURCES = \
    $$DS_SOURCES/*.cpp \
    $$DS_TRANSPORT_SRC/*.cpp \
    $$DS_BUFFER_SRC/*.cpp \
    $$DS_CONCRETE_SRC/*.cpp \
    $$DS_COMMON_SRC/*.cpp

!contains(HEADERS, IMPOSTER_HEADERS): HEADERS += $${IMPOSTER_HEADERS}
!contains(SOURCES, IMPOSTER_SOURCES): SOURCES += $${IMPOSTER_SOURCES}

OTHER_FILES += \
    $$PWD/adv_config.pri \
    $$PWD/min_rpps.pri \
    $$PWD/lib.pri

features.files = \
    $$PWD/adv_config.pri \
    $$PWD/min_rpps.pri \
    $$PWD/lib.pri

headers.files = $${DS_INC}/*
header_imposter.files = $${DS_IMPOSTER_INC}/*

features.path        = $${DS_INSTALL_PATH} #$${DS_INSTALL_FEATURES}
target.path          = $${DS_INSTALL_LIBS}
headers.path         = $${DS_INSTALL_HEADERS}/common
header_imposter.path = $${DS_INSTALL_HEADERS}/$${DS_IMPOSTER_FOLDER_NAME}

!android:{
INSTALLS += \
    target \
    headers \
    features \
    header_imposter
}

HEADERS += \
    include/function_view.h

