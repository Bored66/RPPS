// Description: Файл описания строковых литералов
#ifndef OSU_LITERALS_H
#define OSU_LITERALS_H


#include <QMap>
#include "Transport.h"

namespace OSULiterals {

using String = QString ;

#ifndef UTF8
#define UTF8( string ) QString::fromUtf8( string )
#endif //UTF8

#define INIT_GENERATE_STRINGS_HERE

#ifdef INIT_GENERATE_STRINGS_HERE
#define CSTR(name,chars)\
const String name = chars;
#else
#define CSTR(name,chars)\
extern const String name;
#endif
    enum class StatusType : int
    {
        Undefined               = 0xFF,
        Ok                      = TransportRequestStatus::REQUEST_SEND_SUCCESS,
        TransportUnavailable    = TransportRequestStatus::REQUEST_TRANSPORT_UNAVAILABLE,
        TransportUndefined      = TransportRequestStatus::REQUEST_TRANSPORT_UNDEFINED,
        TransportFailed         = TransportRequestStatus::REQUEST_TRANSPORT_FAILED,
        SendUnknownError        = TransportRequestStatus::REQUEST_SEND_UNKNOWN_ERROR,
        CRCError                = TransportRequestStatus::REQUEST_CRC_ERROR,
        NeedMoreData            = TransportRequestStatus::REQUEST_NEED_MORE_DATA,
        RespUnknownError        = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR,
        UnknownError            = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 100,
        DispatcherUndefined     = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 101,
        AdapterUndefined        = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 102,
        TransportNoActive       = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 103,
        Inactive                = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 104,
        ParameterValueError     = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 150,
        ClientUndefined         = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 200,
        ServerUndefined         = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 201,
        ThreadIncomplete        = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 400,
        ThreadWaitTimeout       = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 401,
        ThreadTerminateTimeout  = TransportRequestStatus::REQUEST_RESPONSE_UNKNOWN_ERROR + 402
    };

    CSTR( BASE_TYPE, "uchar")

    CSTR(STATUS_UNDEF, "Неопределённфй статус")

    static QMap< StatusType, QString> InitStatusInfo()
    {
            QMap< StatusType, QString> retval;
            retval.insert( StatusType::Ok, QString("Нормальное состояние") );
            retval.insert( StatusType::TransportUnavailable,  QString("Компонент \"ТРАНСПОРТ\" недоступен") );
            retval.insert( StatusType::TransportUndefined,    QString("Компонент \"ТРАНСПОРТ\" не определён") );
            retval.insert( StatusType::TransportFailed,       QString("Ошибка приёма/передачи данных компонента \"ТРАНСПОРТ\"") );
            retval.insert( StatusType::SendUnknownError,      QString("Неопределённая ошибка при Отправке команды") );
            retval.insert( StatusType::CRCError,              QString("Ошибка значения контрольной суммы") );
            retval.insert( StatusType::NeedMoreData,          QString("Не хватает данных") );
            retval.insert( StatusType::RespUnknownError,      QString("Неопределённая ошибка Ответа на Отправку команды") );
            retval.insert( StatusType::UnknownError,          QString("Неопределённая ошибка") );
            retval.insert( StatusType::DispatcherUndefined,   QString("Компонент \"ДИСПЕТЧЕР\" не определён") );
            retval.insert( StatusType::AdapterUndefined,      QString("Компонент \"АДАПТЕР\" не определён") );
            retval.insert( StatusType::TransportNoActive,     QString("Компонент \"ТРАНСПОРТ\" неактивен") );
            retval.insert( StatusType::Inactive,              QString("Неактивное состояние") );
            retval.insert( StatusType::ParameterValueError,   QString("Ошибка значения пкараметра функции") );
            retval.insert( StatusType::ClientUndefined,       QString("Компонент \"КЛИЕНТ\" не определён") );
            retval.insert( StatusType::ServerUndefined,       QString("Компонент \"СЕРВЕР\" не определён") );
            retval.insert( StatusType::ThreadIncomplete,      QString("Поток завершен как недопустимый к запуску.") );
            retval.insert( StatusType::ThreadWaitTimeout,     QString("Время ожидания завершения потока истекло.") );
            retval.insert( StatusType::ThreadTerminateTimeout,QString("Время аварийного завершения потока истекло.") );
            return retval;
    }

    const QMap< StatusType, QString> STATUS_INFO = InitStatusInfo();

    CSTR(PROPERTY_TRANSPORT_PTR,  "TRANSPORT_INT_POINTER")

    CSTR(CSERIAL_TYPE,      "Serial")
    CSTR(CTERMINAL_TYPE,    "Terminal")
    CSTR(CCLIENT_TCP_TYPE,  "ClientTCP")
    CSTR(CSERVER_TCP_TYPE,  "ServerTCP")
    CSTR(CUDP_TYPE,         "UDP")
    CSTR(CHTTP_TYPE,        "HTTP")

    CSTR(CPORT_TYPE,        "Port")
    CSTR(CBAUDRATE_TYPE,    "BaudRate")
    CSTR(CDATABITS_TYPE,    "DataBits")
    CSTR(CPARITY_TYPE,      "Parity")
    CSTR(CSTOPBITS_TYPE,    "StopBits")
    CSTR(CDTR_TYPE,         "DTR")
    CSTR(CRTS_TYPE,         "RTS")
    CSTR(CFLOWCTRL_TYPE,    "FlowControl")
    CSTR(CBUFF_SIZE_TYPE,   "BufferThreshold")
    CSTR(CTIMEOUT_TYPE,     "DeviceWaitDelay")

    CSTR(CSERVER_IP_TYPE,   "ServerIP")
    CSTR(CIP_TYPE,          "IP")
    CSTR(CSEND_PORT_TYPE,   "SendPort")
    CSTR(CRESSIVE_PORT_TYPE,"ReceivePort")
    CSTR(CHOST_TYPE,        "Host")
    CSTR(CTCP_RECONNECT,    "Reconnect")
    CSTR(CHTTP_AUTORISATION,"Authorization")

    // Названия
    CSTR(CPROPERTY_TITLE,    UTF8("Характеристики"))

    CSTR(CCONNECTION_TYPE_TITLE,  UTF8("Тип подключения"))

    CSTR(CPORT_TITLE,        UTF8("Порт"))
    CSTR(CBAUDRATE_TITLE,    UTF8("Скорость"))
    CSTR(CDATABITS_TITLE,    UTF8("Длина"))
    CSTR(CPARITY_TITLE,      UTF8("Паритет"))
    CSTR(CSTOPBITS_TITLE,    UTF8("Стоповых бит"))
    CSTR(CDTR_TITLE,         UTF8("Терминал"))
    CSTR(CRTS_TITLE,         UTF8("RTS"))
    CSTR(CFLOWCTRL_TITLE,    UTF8("Чётность"))
    CSTR(CADDITIONAL_TITLE,  UTF8("Дополнительно"))
    CSTR(CBUFF_SIZE_TITLE,   UTF8("Длинна пакета (байт)"))
    CSTR(CTIMEOUT_MS_TITLE,  UTF8("Задержка получения данных (мс)"))

    CSTR(CIP_TITLE,                 UTF8("IP адрес"))
    CSTR(CSERVERIP_TITLE,           UTF8("Cервер"))
    CSTR(CSHOST_TITLE,              UTF8("Хост приёма"))
    CSTR(CSEND_PORT_TITLE,          UTF8("Порт отправки"))
    CSTR(CRESSIVE_PORT_TITLE,       UTF8("Порт приёма"))
    CSTR(CTCP_RECONNECT_TITLE,      UTF8("Переподключение"))
    CSTR(CHTTP_AUTORISATION_TITLE,  UTF8("Авторизация"))

    CSTR(INFO_STATUS,"Текущий статус: %1")
    CSTR(INFO_DISPATCH_DATA,"Данные ДИСПЕТЧЕРА: %1")
    CSTR(INFO_TRANSPORT_DATA,"Данные ТРАНСПОРТА: %1")

    CSTR(SIGNAL_NOT_FOUND, "Объект \"%1\": Сигнал \"%2\" не определён.")
    CSTR(MAP_ERR_INSERT, "Метод \"%1\". Опереция QMap::insert не выполнена (итератор на QMap::end()")
    CSTR(HASH_ERR_INSERT, "Метод \"%1\". Опереция QHash::insert не выполнена (итератор на QHash::end()")

}// namespace OSULiterals

#endif // OSU_LITERALS_H

