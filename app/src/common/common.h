#ifndef COMMON_H
#define COMMON_H

#ifndef QTTEST_OBJECT   // test用
#define QTTEST_OBJECT   // これがないとPrivate領域のテストが行えない
#endif

#include <QtCore>
#include <QObject>
#include <QMap>

// アプリケーション情報
// アプリケーション名
#define APP_NAME                QString("FABOOLDesktop")
#define APP_NAME_TRANSLATE      QString("fabool_desktop_")


//-----Alias-------------------------------//
//Machine Type Number
#define MACHINE_TYPE_NONE                       0
#define MACHINE_TYPE_MINI_1_6W                  1
#define MACHINE_TYPE_MINI_3_5W                  2
#define MACHINE_TYPE_CO2                        3
#define MACHINE_TYPE_DS                         4

//Machine Type Name
#define MACHINE_TYPE_MINI_1_6W_STR              QString("FaboolLaserMini(1.6W)")
#define MACHINE_TYPE_MINI_3_5W_STR              QString("FaboolLaserMini(3.5W)")
#define MACHINE_TYPE_CO2_STR                    QString("FaboolLaserCO2")
#define MACHINE_TYPE_DS_STR                     QString("FaboolLaserDS")

//file suffix
#define FILE_SUFFIX_PROJECT                     QString(".fds")
#define FILE_SUFFIX_PROJECT_BROWSER             QString(".fsd")
#define FILE_SUFFIX_IMG_PRESET                  QString(".fdsi")
#define FILE_SUFFIX_PRM_PRESET                  QString(".fdsp")

//Picture Type
#define PROCESSING_PICTURE_TYPE_RASTER          QString("Raster")
#define PROCESSING_PICTURE_TYPE_VECTOR          QString("Vector")

//System Parameter
#define SYS_PARA_WORK_AREA_WIDTH_MIN            300
#define SYS_PARA_WORK_AREA_WIDTH_MAX            3000
#define SYS_PARA_WORK_AREA_HEIGHT_MIN           230
#define SYS_PARA_WORK_AREA_HEIGHT_MAX           3000
#define SYS_PARA_WORK_AREA_WIDTH_CO2            600
#define SYS_PARA_WORK_AREA_HEIGHT_CO2           440
#define SYS_PARA_WORK_AREA_WIDTH_DS             1050
#define SYS_PARA_WORK_AREA_HEIGHT_DS            630

#define SYS_PARA_DRIVER_POWER_MIN               300
#define SYS_PARA_DRIVER_POWER_MAX               1200
#define SYS_PARA_DRIVER_POWER_DEFAULT_LOW       800
#define SYS_PARA_DRIVER_POWER_DEFAULT_HIGH      980
#define SYS_PARA_GRAPHIC_MIN_SIZE               5
#define SYS_PARA_RASTER_DEAL_F_S_METHOD         0
#define SYS_PARA_RASTER_DEAL_BINARIZATION       1

//Path Parameter
#define PATH_PARAMETER_SPEED_MIN                50
#define PATH_PARAMETER_SPEED_MAX                8000
#define PATH_PARAMETER_LASER_MIN                0
#define PATH_PARAMETER_LASER_MAX                100
#define PATH_PARAMETER_PROCESS_MIN              0
#define PATH_PARAMETER_PROCESS_MAX              30

//INI File Group Name
#define INI_FILE_NAME                           QString("Configuration")
#define INI_GROUP_SETTINGS_PREFIX               QString("Setting")
#define INI_PARAM_MACHINE_TYPE_PREFIX           QString("Mch_Type")
#define INI_PARAM_COM_PREFIX                    QString("COM")
#define INI_PARAM_USED_LANG_PREFIX              QString("Language")

//Graphics Item Modify Type
#define ITEM_MODIFIED_TYPE_NONE                 QString("None")
#define ITEM_MODIFIED_TYPE_COPIED               QString("Copied")
#define ITEM_MODIFIED_TYPE_PASTED               QString("Pasted")
#define ITEM_MODIFIED_TYPE_DELETED              QString("Deleted")
#define ITEM_MODIFIED_TYPE_SHAPE_CHANGED        QString("ShapeChanged")

//Project Format
#define PROJECT_FORMAT_VER_BETA             0.1

//JSON Element Name for Project
#define JSON_TAG_PROJ_NAME                  QString("ProjectName")
#define JSON_TAG_FORMAT_VER                 QString("FormatVersion")
#define JSON_TAG_IMAGE                      QString("Image")
#define JSON_TAG_IMG_TYPE                   QString("ImageType")
#define JSON_TAG_IMG_EXTENTION              QString("ImageExtention")
#define JSON_TAG_IMG_NAME                   QString("ImageName")
#define JSON_TAG_IMG_DATA                   QString("ImageData")
#define JSON_TAG_POS_X                      QString("PosX")
#define JSON_TAG_POS_Y                      QString("PosY")
#define JSON_TAG_WIDTH                      QString("Width")
#define JSON_TAG_HEIGHT                     QString("Height")
#define JSON_TAG_ROTATE                     QString("Rotation")
#define JSON_TAG_GENERIC_PATH               QString("GenericPAth")
#define JSON_TAG_DETAIL_PATH                QString("DetailPAth")
#define JSON_TAG_PATH_SPEED                 QString("Speed")
#define JSON_TAG_PATH_LASER                 QString("LaserPower")
#define JSON_TAG_PATH_TIMES                 QString("ProccessTimes")
#define JSON_TAG_PATH_COLOR                 QString("Color")
#define JSON_TAG_PATH_USE_TYPE              QString("UseType")
#define JSON_TAG_SVG_EFFECTIVE_ATTR         QString("EffectiveAttribute")
#define JSON_TAG_SVG_PATH_LIST              QString("PathList")
#define JSON_TAG_SVG_COLOR_MAP              QString("ColorMap")
#define JSON_TAG_SVG_COLOR                  QString("Color")
#define JSON_TAG_SVG_MADE_BY                QString("MadeBy")
#define JSON_TAG_SVG_DPI                    QString("Dpi")
#define JSON_TAG_SVG_WIDTH                  QString("Width")
#define JSON_TAG_SVG_HEIGHT                 QString("Height")
#define JSON_TAG_SVG_UNIT                   QString("Unit")
#define JSON_TAG_SVG_VIEW_BOX               QString("ViewBox")
#define JSON_TAG_SVG_VIEW_BOX_X             QString("ViewBox_X")
#define JSON_TAG_SVG_VIEW_BOX_Y             QString("ViewBox_Y")
#define JSON_TAG_SVG_VIEW_BOX_W             QString("ViewBox_W")
#define JSON_TAG_SVG_VIEW_BOX_H             QString("ViewBox_H")
#define JSON_TAG_SVG_KEEP_ASPTECT           QString("KeepAspectRatio")
#define JSON_TAG_SVG_COMMON_SETTING         QString("Common")
#define JSON_TAG_SVG_COMMON_FILL            QString("Common_Fill")
#define JSON_TAG_SVG_COMMON_STROKE          QString("Common_Stroke")
#define JSON_TAG_SVG_COMMON_STROKE_WIDTH    QString("Common_StrokeWidth")
#define JSON_TAG_SVG_COMMON_STROKE_DASH     QString("Common_StrokeDash")
#define JSON_TAG_SVG_COMMON_TRANSFORM       QString("Common_Transform")
#define JSON_TAG_SVG_COMMON_DISPLAY         QString("Common_Display")
#define JSON_TAG_SVG_TRANSFORM_0            QString("Transform_0")
#define JSON_TAG_SVG_TRANSFORM_1            QString("Transform_1")
#define JSON_TAG_SVG_TRANSFORM_2            QString("Transform_2")
#define JSON_TAG_SVG_TRANSFORM_3            QString("Transform_3")
#define JSON_TAG_SVG_TRANSFORM_4            QString("Transform_4")
#define JSON_TAG_SVG_TRANSFORM_5            QString("Transform_5")
#define JSON_TAG_SVG_PATH_ELEM_LIST         QString("PathElementList")
#define JSON_TAG_SVG_PATH_ELEM              QString("PathElement")
#define JSON_TAG_SVG_PATH_ELEM_TYPE         QString("PathElement_Type")
#define JSON_TAG_SVG_PATH_ELEM_POINT        QString("PathElement_Point")
#define JSON_TAG_SVG_PATH_ELEM_QUAD         QString("PathElement_Quadratic")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_1       QString("PathElement_Quadratic_1")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_2       QString("PathElement_Quadratic_2")
#define JSON_TAG_SVG_PATH_ELEM_QUAD_3       QString("PathElement_Quadratic_3")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER       QString("PathElement_Bezier")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_1     QString("PathElement_Bezier_1")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_2     QString("PathElement_Bezier_2")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_3     QString("PathElement_Bezier_3")
#define JSON_TAG_SVG_PATH_ELEM_BEZIER_4     QString("PathElement_Bezier_4")
#define JSON_TAG_SVG_PATH_ELEM_ARC          QString("PathElement_Arc")
#define JSON_TAG_SVG_POINT_X                QString("Point_X")
#define JSON_TAG_SVG_POINT_Y                QString("Point_Y")
#define JSON_TAG_SVG_POINT_NUM              QString("Point_No")

#define JSON_TAG_FILE_TYPE                  QString("FileType")
#define FILE_TYPE_LIBRARY                   QString("Library")
#define FILE_TYPE_MATERIAL                  QString("Materials")
#define JSON_TAG_ATTRIBUTION                QString("AttributionTag")
#define JSON_TAG_MATERIAL_DATA              QString("MaterialData")
#define JSON_TAG_MATERIAL_NAME              QString("MaterialName")
#define JSON_TAG_LASER_TYPE                 QString("LaserType")
#define JSON_TAG_LASER_TYPE_1_6W            QString("1.6W")
#define JSON_TAG_LASER_TYPE_3_5W            QString("3.5W")
#define JSON_TAG_LASER_TYPE_CO2             QString("CO2")
#define JSON_TAG_LASER_TYPE_DS              QString("DS")
#define JSON_TAG_PROCESS_PARAM              QString("ProcessParameter")
#define JSON_TAG_THICK                      QString("Thick")
#define JSON_TAG_PARAMETER                  QString("Parameter")
#define JSON_TAG_USE_TYPE_ENGRAVE           QString("Engrave")
#define JSON_TAG_USE_TYPE_CUT               QString("Cut")

//serial
#ifndef Q_OS_WIN
#define CONTROL_BOARD "STM32 Virtual ComPort"
#else
#define CONTROL_BOARD "STMicroelectronics Virtual COM Port"
#endif

//paramTable
#define PARAM_TABLE_HEADER_COL_COLOR             0
#define PARAM_TABLE_HEADER_COL_USE_TYPE          1
#define PARAM_TABLE_HEADER_COL_HEAD_SPEED        2
#define PARAM_TABLE_HEADER_COL_LASER_POWER       3
#define PARAM_TABLE_HEADER_COL_PROCESS_TIME      4

#define PARAM_TABLE_HEADER_COL_COLOR_STR             "Color"
#define PARAM_TABLE_HEADER_COL_HEAD_SPEED_STR        "Head Speed"
#define PARAM_TABLE_HEADER_COL_LASER_POWER_STR       "Laser Power"
#define PARAM_TABLE_HEADER_COL_USE_TYPE_STR          "Use Type"
#define PARAM_TABLE_HEADER_COL_PROCESS_TIME_STR      "Times"
#define PARAM_TABLE_HEADER_COL_USE_TYPE_CUT          "Cut"
#define PARAM_TABLE_HEADER_COL_USE_TYPE_ENGRAVE      "Engrave"

#define PARAM_DENOMINATION_MM_PER_MIN                "(mm/min)"
#define PARAM_DENOMINATION_PERCENTAGE                "(%)"

//　パラメータのバーの色
#define COLOR_PARAM_HEADER_1             QString("#0D47A1") //"blue"             //
#define COLOR_PARAM_HEADER_2             QString("#01579B") //"lightblue"        //
#define COLOR_PARAM_HEADER_3             QString("#006064") //"cyan"             //
#define COLOR_PARAM_HEADER_4             QString("#004D40") //"teal"             //
#define COLOR_PARAM_HEADER_5             QString("#1B5E20") //"green"            //
#define COLOR_PARAM_HEADER_6             QString("#33691E") //"lightgreen"       //

// Gcode
#define GCODE_STOP_CYCLE                "!\n"
#define GCODE_RETURN_TO_ORIGIN          "~\nG30\n"
#define GCODE_RETURN_TO_ORIGIN_QUICK    "~\nG90\nM81\nG0X0Y0\n"
#define GCODE_PROCESS_START_HEADER      "G90\nM80\n"

//Main View
#define MAIN_VIEW_ZOOM_MIN 30

//Command Manager
#define COMMAND_UNDO_STACK_MAX 20

//-----ERR NUMBER-----------------------------//
//Serial Comm Error Number
//---------------------------------------------------------------------------
// 戻り値
// 戻り値はquint16で宣言する
// 最大65535
//---------------------------------------------------------------------------
#define SUCCESS 0   // 成功
#define ERR_SETTING_FILE_OPEN           10
#define ERR_SETTING_WORK_AREA_INVALID   11
#define ERR_SETTING_DRIVER_POW_INVALID  12
#define MODIFY_LASER_TYPE_CHANGED       20

// Reserved(100～)
// Logger(200～)
#define ERR_LOGGER_OPEN                 200     // Logファイルオープン失敗

#define ERR_COM_PORT_OPEN               401
#define ERR_COM_PORT_OPEN_YET           402
#define ERR_SERIAL_WRITE_TIMEOUT        403
#define ERR_SERIAL_READ_TIMEOUT         404
#define ERR_SERIAL_BUFFER_NOT_EMPTY     405
#define NEED_TO_SEND_READY              406

#define INFO_PROJ_SAVE_ALREADY_SAVED     500        //　プロジェクト保存済み
#define ERR_PROJ_SAVE_FILE_NOT_EXIST     501        // プロジェクトファイルが存在しない
#define ERR_PROJ_SAVE_FILE_OPEN          502        // 保存先プロジェクトファイルが開けない
#define ERR_PROJ_SAVE_INVAILD_DATA       503        // 保存データに不正がある
#define ERR_PROJ_SAVE_NOT_EXIST_DATA     504        // データなし
#define ERR_PROJ_SAVE_SIZE_CHECK         505        // すべてのアイテムが枠内に収まっていない
#define ERR_PROJ_SAVE_CANCEL             506        // 保存キャンセル

#define ERR_PROJ_LOAD_FILE_NOT_EXIST     601        // ロード対象が存在しない
#define ERR_PROJ_LOAD_FILE_OPEN          602        // ロード対象が開けない
#define ERR_PROJ_LOAD_INVAILD_DATA       603        // ロードデータに不正がある
#define ERR_PROJ_LOAD_CANCEL             604        // ロードキャンセル

#define ERR_LIBRARY_OPEN                 700
#define ERR_MATERIAL_OPEN                701

#define ERR_LOAD_IMAGE                   801
#define ERR_LOAD_JSON                    802

// ui(1000~)
#define ERR_WORK_AREA_INVALID            1000

// Error Strings
#define SUCCESS_STR                         QString("Success.")   // 成功
#define UNKNOWN_ERROR_STR                   QString("There is unknown Error.")
#define ERR_SETTING_FILE_OPEN_STR           QString("Failed to open settings.")
#define ERR_SETTING_WORK_AREA_INVALID_STR   QString("Work area is invalid.")
#define ERR_SETTING_DRIVER_POW_INVALID_STR  QString("Driver power is invalid.")
#define ERR_SETTING_CONFIRMATION_STR        QString("\nNeed confirmtion your settings.")

// Reserved(100～)
// Logger(200～)
#define ERR_LOGGER_OPEN_STR                 ""     // Logファイルオープン失敗
// PythonApp(300～)
#define ERR_PYTHONAPP_START_UP_STR          ""    // Pythonアプリ起動失敗
#define ERR_PYTHONAPP_EXIT_STR              ""     // Pythonアプリ終了処理失敗
#define ERR_PYTHONAPP_ACCESS_TOKEN_STR      ""     // アクセストークン取得失敗
#define ERR_PYTHONAPP_ZIP_DIR_STR           ""    // フォルダ圧縮失敗

#define ERR_COM_PORT_OPEN_STR               ""
#define ERR_COM_PORT_OPEN_YET_STR           ""
#define ERR_SERIAL_WRITE_TIMEOUT_STR        ""
#define ERR_SERIAL_READ_TIMEOUT_STR         ""
#define ERR_SERIAL_BUFFER_NOT_EMPTY_STR     ""

#define INFO_PROJ_SAVE_ALREADY_SAVED_STR    ""        //　プロジェクト保存済み
#define ERR_PROJ_SAVE_FILE_NOT_EXIST_STR    "Cannot find project."        // プロジェクトファイルが存在しない
#define ERR_PROJ_SAVE_FILE_OPEN_STR         "Failed to open project."        // 保存先プロジェクトファイルが開けない
#define ERR_PROJ_SAVE_INVAILD_DATA_STR      "Existing invaild data."       // 保存データに不正がある
#define ERR_PROJ_SAVE_SIZE_CHECK_STR        "Some items are out of range."
#define ERR_PROJ_SAVE_CANCEL_STR            "Cancel save."        // 保存キャンセル

#define ERR_PROJ_LOAD_FILE_NOT_EXIST_STR    "Not exist target project."        // ロード対象が存在しない
#define ERR_PROJ_LOAD_FILE_OPEN_STR         "Failed to open project."       // ロード対象が開けない
#define ERR_PROJ_LOAD_INVAILD_DATA_STR      "Existing invalid data."       // ロードデータに不正がある
#define ERR_PROJ_LOAD_CANCEL_STR            "Cancel load."        // ロードキャンセル

extern QStringList RasterSuffixList;
extern QStringList VectorSuffixList;

//debug_log
#define DEBUG_STRING(text)      qDebug()    << QString(text)
#define INFO_STRING(text)       qInfo()     << QString(text)
#define WARNING_STRING(text)    qWarning()  << QString(text)
#define CRITICAL_STRING(text)   qCritical() << QString(text)
#define FATAL_STRING(text)      qFatal()    << QString(text)

#endif // COMMON_H


