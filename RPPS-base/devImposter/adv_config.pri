# Description: Файл настройки проекта
CONFIG(debug,debug|release):DS_LIB_SUFFIX = d

DS_ROOT          = $$PWD
DS_INCLUDE       = include
DS_INC           = $${DS_ROOT}/$${DS_INCLUDE}
#DS_IMPOSTER_INC  = $${DS_INC}/$${DS_IMPOSTER_FOLDER_NAME}
DS_IMPOSTER_INC  = $${DS_INC}/$${DS_IMPOSTER_FOLDER_NAME}
#DS_COMMON_INC    = $${DS_INC}/Common
DS_COMMON_INC    = $${DS_IMPOSTER_INC}/common
DS_TRANSPORT_INC = $${DS_IMPOSTER_INC}/Transport
DS_CONCRETE_INC  = $${DS_TRANSPORT_INC}/Concrete
DS_BUFFER_INC    = $${DS_TRANSPORT_INC}/Buffer
DS_INSTALL_LIBS  = $${DS_ROOT}/lib

#DS_EXTERNAL_PATH       = $${DS_ROOT}/external
#DS_INSTALL_FEATURES    = $${DS_ROOT}/features
DS_EXTERNAL_INC        = $${DS_EXTERNAL_PATH}/include
#DS_EXTERNAL_LIB        = $${DS_EXTERNAL_PATH}/lib

!contains(QT, core): QT += core
!contains(QT, xml): QT += xml

greaterThan(QT_MAJOR_VERSION,4): {
    !contains(QT, widgets): QT += widgets
    CONFIG += c++11
} else {
    QMAKE_CXXFLAGS += -std=c++0x
}
