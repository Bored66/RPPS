// Description: Файл описания строк
#ifndef DI_LITERALS_H
#define DI_LITERALS_H

#include <QObject>
#include <QString>
#include "OSULiterals.h"

namespace Generate
{
using String = QString ;

#define STR_FROM_UTF8_TR(CHAR_POINTER)\
    QString::fromUtf8(QString(CHAR_POINTER).toStdString().c_str())//QObject::trUtf8

//------------------------------------------------------------------------------
///
/// \name Литералы работы с проектами
/// \{
CSTR( MSG_STRUCT, "_msgStruct" )

CSTR(TITLE_PRJ_OPEN, STR_FROM_UTF8_TR("Откройте или создайте новый проект."))
CSTR(TITLE_PRJ_FILE, STR_FROM_UTF8_TR("Файл проекта: \"%1\"."))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы диалоговых окон
/// \{

CSTR(MSGBOX_QUIT_CAPTION   , STR_FROM_UTF8_TR("Программа завершает работу"))
CSTR(MSGBOX_QUIT_INFO      , STR_FROM_UTF8_TR("Вы уверены, что хотите завершить работу?"))
CSTR(MSGBOX_QUIT_OK        , STR_FROM_UTF8_TR("Применить"))
CSTR(MSGBOX_QUIT_CANCEL    , STR_FROM_UTF8_TR("Отмена"))
CSTR(MSGBOX_QUIT_YES       , STR_FROM_UTF8_TR("Да"))
CSTR(MSGBOX_QUIT_NO        , STR_FROM_UTF8_TR("Нет"))

CSTR(CAPT_LABEL_FILENAME   , STR_FROM_UTF8_TR("Файл"))
CSTR(CAPT_LABEL_FILEPATH   , STR_FROM_UTF8_TR("Путь к Файлу"))
CSTR(CAPT_LABEL_FILEPRJ   , STR_FROM_UTF8_TR("Файл проекта"))
CSTR(CAPT_PBUTTON_TYPE_NONE   , STR_FROM_UTF8_TR("Не определён"))
CSTR(CAPT_PBUTTON_TYPE_XML   , STR_FROM_UTF8_TR("XML"))

CSTR(DLG_OPEN_PRJ_PATH     , STR_FROM_UTF8_TR("Укажите каталог проекта"))
CSTR(DLG_OPEN_PRJ_CAPTION  , STR_FROM_UTF8_TR("Открыть проект"))
CSTR(DLG_PRJ_FILE_EXT      , STR_FROM_UTF8_TR("XML Files (*.xml);;All Files (*.*)"))

CSTR(DLG_CAPT_DIALOG_EDIT  , STR_FROM_UTF8_TR("Редактор. Объект \"%1\"."))
CSTR(DLG_INFO_TRANSF_EDIT  , STR_FROM_UTF8_TR("Вы уверены, что хотите удалить преобразование \"%1\"?"))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы окон форматорв
/// \{

CSTR(UI_CAPT_NAME,      STR_FROM_UTF8_TR("Название"))
CSTR(UI_CAPT_TRANSFORM, STR_FROM_UTF8_TR("Трансформация"))
CSTR(UI_CAPT_COMMENT,   STR_FROM_UTF8_TR("Примечания"))
CSTR(UI_CAPT_EMPTY,     STR_FROM_UTF8_TR("< Нет значения >"))
CSTR(UI_CAPT_DEF_BYTE,  STR_FROM_UTF8_TR("Байт по-умолчанию"))
CSTR(UI_CAPT_FILE_NAME, STR_FROM_UTF8_TR("Файл"))

CSTR(UI_CAPT_APPLY, STR_FROM_UTF8_TR("Применить"))
CSTR(UI_CAPT_ADD, STR_FROM_UTF8_TR("Добавить"))
CSTR(UI_CAPT_EDIT, STR_FROM_UTF8_TR("Правка"))
CSTR(UI_CAPT_DEL, STR_FROM_UTF8_TR("Удалить"))
CSTR(UI_CAPT_CLR, STR_FROM_UTF8_TR("Очистить"))
CSTR(UI_CAPT_VAL, STR_FROM_UTF8_TR("Значение"))
CSTR(UI_CAPT_SIZEOF, STR_FROM_UTF8_TR("SizeOf field = "))

CSTR(UI_CAPT_TRANSFORM_LIST,    STR_FROM_UTF8_TR(" Список трансформаций "))
CSTR(UI_CAPT_FORMATS_LIST,      STR_FROM_UTF8_TR(" Список Форматов "))
CSTR(UI_CAPT_FORMATS_USAGE,     STR_FROM_UTF8_TR(" Назначен Форматам "))

CSTR(UI_CAPT_TRANSFORM_TITLE, STR_FROM_UTF8_TR(" Трансформация: "))
CSTR(UI_CAPT_TRANSFORM_TADD, STR_FROM_UTF8_TR("Добавить"))
CSTR(UI_CAPT_TRANSFORM_REPL, STR_FROM_UTF8_TR("Заменить"))
CSTR(UI_CAPT_TRANSFORM_TDEL, STR_FROM_UTF8_TR("Удалить"))
CSTR(UI_CAPT_TRANSFORM_FMT, STR_FROM_UTF8_TR("Назначить Формату"))
CSTR(UI_CAPT_PAIR_TITLE, STR_FROM_UTF8_TR(" Пара значений: "))
CSTR(UI_VAL_PAIR_FIRST, STR_FROM_UTF8_TR("Последовательности байт:"))
CSTR(UI_VAL_PAIR_SECOND, STR_FROM_UTF8_TR("Трансформация:"))

CSTR(UI_CAPT_CONST_GSIMPLE  , STR_FROM_UTF8_TR(" Простая константа "))
CSTR(UI_CAPT_CONST_GLIST    , STR_FROM_UTF8_TR(" Список полей комплексной константы "))
CSTR(UI_CAPT_SIMPLE        , STR_FROM_UTF8_TR(" Простая "))
CSTR(UI_CAPT_COMPLEX       , STR_FROM_UTF8_TR(" Комплексная "))
CSTR(UI_CAPT_CONST_NAME    , STR_FROM_UTF8_TR(" Название "))

CSTR(UI_CAPT_TYPE_GFLDS  , STR_FROM_UTF8_TR(" Поля типа "))
CSTR(UI_CAPT_TYPE_GAFLD  , STR_FROM_UTF8_TR(" Атрибуты поля "))
CSTR(UI_CAPT_TYPE_FLD_T  , STR_FROM_UTF8_TR("Тип атрибута"))
CSTR(UI_CAPT_TYPE_FLD_N  , STR_FROM_UTF8_TR("Тип"))
CSTR(UI_CAPT_TYPE_NAME   , STR_FROM_UTF8_TR("Название"))
CSTR(UI_CAPT_TYPE_SIZE   , STR_FROM_UTF8_TR("Размер"))
CSTR(UI_CAPT_TYPE_DEFAULT, STR_FROM_UTF8_TR("Пустой"))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы названий элементов QAction
/// \{
CSTR(CAPT_ACTION_QUIT        , STR_FROM_UTF8_TR("Выход"))
CSTR(CAPT_ACTION_NEW_PRJ     , STR_FROM_UTF8_TR("Создать"))
CSTR(CAPT_ACTION_OPEN_PRJ    , STR_FROM_UTF8_TR("Открыть"))
CSTR(CAPT_ACTION_VIEW_PRJ    , STR_FROM_UTF8_TR("Просмотр"))
CSTR(CAPT_ACTION_SHOW_ERR    , STR_FROM_UTF8_TR("Проблемы"))
CSTR(CAPT_ACTION_SHOW_LOG    , STR_FROM_UTF8_TR("Журнал работы"))

CSTR(CAPT_ACTION_EDITMODE     , STR_FROM_UTF8_TR("Режим\nРедактирования"))
CSTR(CAPT_ACTION_ADD           , STR_FROM_UTF8_TR("Добавть"))
CSTR(CAPT_ACTION_EDIT          , STR_FROM_UTF8_TR("Редактировать"))
CSTR(CAPT_ACTION_DEL           , STR_FROM_UTF8_TR("Удалить"))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы названий классов
/// \{

CSTR(NAME_MODEL_ROOT        , STR_FROM_UTF8_TR("Семантическая модель"))
CSTR(NAME_GENERATOR         , STR_FROM_UTF8_TR("Генератор"))
CSTR(NAME_TRANSFORMS        , STR_FROM_UTF8_TR("Преобразования"))
CSTR(NAME_PAIRS,            STR_FROM_UTF8_TR("Пары значений"))
CSTR(NAME_FORMATS           , STR_FROM_UTF8_TR("Форматы"))
CSTR(NAME_DEVICES           , STR_FROM_UTF8_TR("Команды"))
CSTR(NAME_ANSWER            , STR_FROM_UTF8_TR("Ответ"))

/// \}

///
/// \name Литералы типов объектов
/// \{

CSTR(TITLE_UNDEFINED       , STR_FROM_UTF8_TR("<Укажите название объекта>"))
CSTR(TITLE_GENERATOR       , STR_FROM_UTF8_TR("Генератор"))
CSTR(TITLE_COLLECTION      , STR_FROM_UTF8_TR("Набор объектов"))
CSTR(TITLE_FMT_COLLECTION  , STR_FROM_UTF8_TR("Набор форматов"))
CSTR(TITLE_TRN_COLLECTION  , STR_FROM_UTF8_TR("Набор преобразований"))
CSTR(TITLE_TYPE_COLLECTION , STR_FROM_UTF8_TR("Набор типов форматов"))
CSTR(TITLE_CONST_SCOLLECTION, STR_FROM_UTF8_TR("Набор простых констант"))
CSTR(TITLE_CONST_ECOLLECTION, STR_FROM_UTF8_TR("Набор констант с полями"))
CSTR(TITLE_MSG_COLLECTION  , STR_FROM_UTF8_TR("Набор сообщений"))
CSTR(TITLE_FIELD_COLLECTION, STR_FROM_UTF8_TR("Набор полей"))
CSTR(TITLE_DEV_COLLECTION  , STR_FROM_UTF8_TR("Набор устройств"))
CSTR(TITLE_CMD_COLLECTION  , STR_FROM_UTF8_TR("Набор команд"))
CSTR(TITLE_REACT_COLLECTION, STR_FROM_UTF8_TR("Набор реакций"))
CSTR(TITLE_EXPR_COLLECTION,  STR_FROM_UTF8_TR("Формирует выражения"))
CSTR(TITLE_TRANSFORM       , STR_FROM_UTF8_TR("Преобразование"))
CSTR(TITLE_FORMAT          , STR_FROM_UTF8_TR("Формат"))
CSTR(TITLE_CONST           , STR_FROM_UTF8_TR("Константа"))
CSTR(TITLE_TYPEDEF         , STR_FROM_UTF8_TR("Определение"))
CSTR(TITLE_MESSAGE         , STR_FROM_UTF8_TR("Сообщение"))
CSTR(TITLE_MSG_FIELD       , STR_FROM_UTF8_TR("Поле"))
CSTR(TITLE_OPTION          , STR_FROM_UTF8_TR("Опция"))
CSTR(TITLE_PARAM           , STR_FROM_UTF8_TR("Параметр"))

CSTR(TITLE_DEVICE          , STR_FROM_UTF8_TR("Набор команд"))
CSTR(TITLE_ANSWER          , STR_FROM_UTF8_TR("Результат"))
CSTR(TITLE_REQUEST         , STR_FROM_UTF8_TR("Запрос"))
CSTR(TITLE_REACTION        , STR_FROM_UTF8_TR("Набор реакций"))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы названия свойств каждого объекта
/// \{

CSTR(TITLE_INC_FIELDS      , STR_FROM_UTF8_TR("Атрибуты"))
CSTR(TITLE_INC_FORMATS     , STR_FROM_UTF8_TR("Форматы"))
CSTR(TITLE_INC_FILES       , STR_FROM_UTF8_TR("Файлы"))
CSTR(TITLE_INC_REQUESTS    , STR_FROM_UTF8_TR("Запросы"))
CSTR(TITLE_INC_TRANSFORMS  , STR_FROM_UTF8_TR("Преобразования"))
CSTR(TITLE_INC_MSGS        , STR_FROM_UTF8_TR("Сообщения"))
CSTR(TITLE_INC_TYPEDEFS    , STR_FROM_UTF8_TR("Определения"))
CSTR(TITLE_INC_CONASTS     , STR_FROM_UTF8_TR("Константы"))
CSTR(TITLE_INC_MSGFIELDS   , STR_FROM_UTF8_TR("Атрибуты"))
CSTR(TITLE_INC_OPTIONS     , STR_FROM_UTF8_TR("Опции"))
CSTR(TITLE_INC_PARAMS      , STR_FROM_UTF8_TR("Параметры"))

CSTR(TITLE_PROP_ANSWER     , STR_FROM_UTF8_TR("Ответ"))
CSTR(TITLE_PROP_VALUE      , STR_FROM_UTF8_TR("Значение"))
CSTR(TITLE_PROP_SIMPLE     , STR_FROM_UTF8_TR("Тип"))
CSTR(TITLE_PROP_FORMAT     , STR_FROM_UTF8_TR("Формат"))
CSTR(TITLE_PROP_FIELD_TYPE , STR_FROM_UTF8_TR("Тип поля"))
CSTR(TITLE_PROP_REF        , STR_FROM_UTF8_TR("Ссылка"))
CSTR(TITLE_PROP_LINK       , STR_FROM_UTF8_TR("Поле"))
CSTR(TITLE_PROP_SIZE       , STR_FROM_UTF8_TR("Размер"))
CSTR(TITLE_PROP_DEFVAL     , STR_FROM_UTF8_TR("Значение"))
CSTR(TITLE_PROP_BYTE_POS   , STR_FROM_UTF8_TR("Позиция байта"))
CSTR(TITLE_PROP_FUNC       , STR_FROM_UTF8_TR("Параметр"))
CSTR(TITLE_PROP_TEMPL      , STR_FROM_UTF8_TR("Шаблон"))
CSTR(TITLE_PROP_PAIR       , STR_FROM_UTF8_TR("Пара"))

/// \}

//------------------------------------------------------------------------------
///
/// \name Литералы наименований свойств (Q_PROPERTY) для объектов различныйх
/// типов \sa TModelType семантической модели.
/// \{

CSTR(PROP_NAME_FIELD_TYPE   ,   STR_FROM_UTF8_TR("FIELD_TYPE"))
CSTR(PROP_NAME_FIELD_SREF   ,   STR_FROM_UTF8_TR("FIELD_SIZE_REFERENCE"))
CSTR(PROP_NAME_FIELD_SIZE   ,   STR_FROM_UTF8_TR("FIELD_SIZE"))
CSTR(PROP_NAME_FIELD_DEFAULT,   STR_FROM_UTF8_TR("FIELD_DEFAULT"))

CSTR(PROP_NAME_MSG_TEMPLATET,   STR_FROM_UTF8_TR("MSG_TEMPLATE_NAME"))

CSTR(PROP_NAME_CONST_VALUE,     STR_FROM_UTF8_TR("CONST_VALUE"))
CSTR(PROP_NAME_CONST_SIMPLE,    STR_FROM_UTF8_TR("CONST_IS_SIMPLE"))

CSTR(PROP_NAME_FMT_TRANSFORM,   STR_FROM_UTF8_TR("FORMAT_TRANSFORM_NAME"))
CSTR(PROP_NAME_FMT_SRC_FILE,    STR_FROM_UTF8_TR("FORMAT_SRC_FILE"))
CSTR(PROP_NAME_FMT_DEF_BYTE_VAL,STR_FROM_UTF8_TR("FORMAT_DEFAULT_BYTE_VALUE"))

CSTR(PROP_NAME_PARAM_BYTE,      STR_FROM_UTF8_TR("PARAM_BYTE"))
CSTR(PROP_NAME_PARAM_VALUE,     STR_FROM_UTF8_TR("PARAM_VALUE"))
CSTR(PROP_NAME_PARAM_FUNC,      STR_FROM_UTF8_TR("PARAM_FUNC"))
CSTR(PROP_NAME_PARAM_FILL_FROM, STR_FROM_UTF8_TR("PARAM_FILL_FROM"))
CSTR(PROP_NAME_PARAM_RANGE,     STR_FROM_UTF8_TR("PARAM_RANGE"))

CSTR(PROP_NAME_OPTION_VALUE,    STR_FROM_UTF8_TR("OPTION_VALUE"))
CSTR(PROP_NAME_OPTION_PROBAB,   STR_FROM_UTF8_TR("OPTION_PROBABILITY"))

CSTR(PROP_NAME_REACT_PARAMS,   STR_FROM_UTF8_TR("REACTION_PARAMS"))

CSTR(PROP_NAME_ANSWER_TYPE,     STR_FROM_UTF8_TR("ANSWER_TYPE"))
CSTR(PROP_NAME_ANSWER_PARAMS,   STR_FROM_UTF8_TR("ANSWER_PARAMS"))
CSTR(PROP_NAME_ANSWER_REACTION, STR_FROM_UTF8_TR("ANSWER_REACTION"))

CSTR(PROP_NAME_REQUEST_TYPE,     STR_FROM_UTF8_TR("REQUEST_TYPE"))
CSTR(PROP_NAME_REQUEST_FORMAT,   STR_FROM_UTF8_TR("REQUEST_FORMAT_NAME"))
CSTR(PROP_NAME_REQUEST_PARAMS,   STR_FROM_UTF8_TR("REQUEST_FORMAT_PARAMS"))
CSTR(PROP_NAME_REQUEST_TEMPLATE, STR_FROM_UTF8_TR("REQUEST_FORMAT_TEMPLATE"))
CSTR(PROP_NAME_REQUEST_ANSWER,  STR_FROM_UTF8_TR("REQUEST_FORMAT_ANSWER"))

CSTR(PROP_NAME_DEVICE_FORMAT,   STR_FROM_UTF8_TR("DEVICE_FORMAT"))
CSTR(PROP_NAME_DEVICE_FILE,     STR_FROM_UTF8_TR("DEVICE_SOURCE_FILE"))

/// \}

//------------------------------------------------------------------------------
///
/// \name .
/// \{

CSTR(TOOL_ACTIONS,              STR_FROM_UTF8_TR("Отображение"))
CSTR(TOOL_ACTION_PROJECT,       STR_FROM_UTF8_TR("Проекты"))
CSTR(TOOL_ACTION_PRJ_CATEGORY,  STR_FROM_UTF8_TR("Категории"))
CSTR(TOOL_ACTION_XML,           STR_FROM_UTF8_TR("XML-вид"))
CSTR(TOOL_ACTION_DEL,           STR_FROM_UTF8_TR("Удалить"))

/// \}

CSTR(PROJECT_NAME,                      STR_FROM_UTF8_TR("Проект \"%1\""))
CSTR(PROJECT_NODE_ROOT_MODEL,           STR_FROM_UTF8_TR("Семантическая модель"))
CSTR(PROJECT_NODE_ROOT_FILES,           STR_FROM_UTF8_TR("Файловая cтруктура"))
CSTR(PROJECT_NODE_MAINFILE,             STR_FROM_UTF8_TR("Файл структуры"))
CSTR(PROJECT_NODE_FORMATS,              STR_FROM_UTF8_TR("Форматы"))
CSTR(PROJECT_NODE_COMMANDS,             STR_FROM_UTF8_TR("Команды"))
CSTR(PROJECT_NODE_INCLUDES,             STR_FROM_UTF8_TR("Заголовочные"))
CSTR(PROJECT_NODE_LIBRARIES,            STR_FROM_UTF8_TR("Библиотеки"))
CSTR(PROJECT_NODE_FILES,                STR_FROM_UTF8_TR("Другие файлы"))

}// namespace Generate

#endif // DI_LITERALS_H

