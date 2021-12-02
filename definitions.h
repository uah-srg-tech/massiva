/**
 * \file	definitions.h
 * \brief	global definitions
 *
 * \author	Aaron Montalvo, <amontalvo@srg.aut.uah.es>
 *
 * \internal
 * Created:		26/03/2012 at 10:32:26
 * Compiler:	LabWindows / CVI 10.0.0.0
 * Company:		Space Research Group, Universidad de Alcalá.
 * Copyright:	Copyright (c) 2012, Aaron Montalvo
 *
 * For further information, please visit http://srg.aut.uah.es
 *
 */
#ifndef __definitions_H__
#define __definitions_H__

#include <stdint.h>
#define UNUSED(x) (void)(x)

#if(defined _WIN32 || __CYGWIN__)
#include <psdk_inc/_ip_types.h>
#elif(defined __linux__)
#include <netinet/in.h>
#endif

/*!
 * Maximum size for packets
 */
#define MAX_PACKET_SIZE                                 4200
/*!
 * Maximum log message string length
 */
#define MAX_MSG_SIZE                                     300
/*!
 * Maximum field name length
 */
#define MAX_FIELD_NAME_LEN                                30
/*!
 * Maximum export filename length
 */
#define MAX_EXPORT_FILENAME_LEN                          130

/*!
 * Maximum number of interfaces
 */
#define MAX_INTERFACES                                     8
/*!
 * Maximum number of levels
 */
#define MAX_LEVELS                                         4
/*!
 * Maximum number of fields
 */
#define MAX_FIELDS                                        50
/*!
 * Maximum number of DIC
 */
#define MAX_DICS                                          10
/*!
 * Maximum number of export fields
 */
#define MAX_EXPORT_FIELDS                                230

/*!
 * Maximum number of bool vars
 */
#define MAX_BOOL_VARS                                    256
/*!
 * Maximum number of filters
 */
#define MAX_FILTERS                                       10

/*!
 * Maximum number of special packets
 */
#define MAX_SPECIAL_PACKETS                               17
/*!
 * Maximum number of periodic TCs
 */
#define MAX_PERIODIC_TCS                                  10
/*!
 * Maximum number of global vars
 */
#define MAX_GLOBALVARS                                    20
/*!
 * Maximum number of monitors
 */
#define MAX_MONITORINGS                                   15
 
/*!
 * Maximum procedures name length
 */
#define MAX_PROC_NAME_LEN                                 70
/*!
 * Maximum procedures filename length
 */
#define MAX_PROC_FILE_LEN                                255

/*!
 * Maximum number of procedures
 */
#define MAX_PROCEDURES                                   150

/*!
 * Maximum error message string length
 */
#define MAX_ERR_LEN                                      120

/*!
 * Maximum bool var operand lengths
 */
#define MAX_STR_OPERAND_LEN                             4200
#define MAX_NUM_OPERAND_LEN                                8

/*!
 * Maximumalarm msg length
 */
#define MAX_ALARM_MSG_LEN                                 16

/*!
 * Maximum number of alarms
 */
#define MAX_NUMBER_OF_ALARM_VALS                           3

/*!
 * Maximum default timeout
 */
#define DEFAULT_TIMEOUT_MS                              1000.0

/*!
 * Maximum read ports (for logic SpW ports)
 */
#define MAX_READ_PORTS                                     2

/*!
 * Maximum port name length
 */
#define MAX_PORT_NAME_LEN                                 20

#define MAX_DEV_NAME_LEN                                  22

#define MAX_DEV_INFO_LEN                                  40

#define MAX_NUMBER_ACTIONS                                 1

#define MAX_PROTOCOL_PACKETS                              15

#define MAX_PROTOCOL_PACKET_LEVELS                         2

#define MAX_STEP_NAME_LEN                                 60

#define MAX_ACTION_MSG_LEN                                90

#define MAX_PROTOCOLS                                      2

#define MAX_PHY_HDR                                        6

#define MAX_DATE_VERSION_SIZE                             11

#define MAX_CHARTS                                        10

#define MAX_PLOTS_PER_CHART                               12

#define TEST_ERRORS                                        4

#define NUMBER_OF_LOGS                                     3

#define PTCL_BUFFER_SIZE                             0x10000

typedef enum{
    DISABLED,
    ENABLED
}enabled_status;

typedef enum{
    MAIN_LOG = 0,
    PORT_LOGS = 1,
    RAW_LOG = 2,
}gss_options_logs;

typedef enum{
    TX_ERRORS = 0,
    NOT_EXPECTED_PACKETS = 1,
    FILTERS_KO = 2,
    VALID_TIMES_KO = 3
}testErrorsIndex;

typedef enum {
    IN_PROGRESS = 0,
    FINISHED = 1,
    CANCELED = 2
} processStatus;
/*!
 * Enumeration of the different port options
 */
typedef enum {
    NO_PORT,		/**< port not configured */
    SPW_USB_PORT,	/**< SpW-USB brick */
    SPW_USB_MK2_PORT,   /**< SpW-USB MK2 brick */
    SPW_PCI_PORT,	/**< SpW PCI-2 board */
    SPW_TC_PORT,	/**< SpW dummy port for timecodes */
    SPW_ERROR_PORT,	/**< SpW dummy port for errors */
    UART_PORT,		/**< Serial - UART */
    SOCKET_SRV_PORT,    /**< Socket - Server */
    SOCKET_CLI_PORT,    /**< Socket - Client */
    DUMMY_PORT		/**< dummy port */
}portTypes;

typedef enum {
    IN_PORT,            /**< only input port */
    OUT_PORT,           /**< only output port */
    INOUT_PORT          /**< input / output port */
} ioTypes;
/*!
 * Enumeration of the different level type errors
 */
typedef enum {
    TC_FORMAT, 		/**< Telecommand packet format */
    TM_FORMAT,		/**< Telemetry packet format */
    FILTER,		/**< Bool var or filter */
    EXPORT,		/**< Export */
    IMPORT,		/**< Import */
    TP_INPUT,		/**< TP file: Input */
    TP_OUTPUT,		/**< TP file: Output */
    TP_STEP,		/**< TP file: Step */
    CONFIG_OTHER, 	/**< Config file / TP file: Other */
    PERIOD,
    INTERVAL
}levelTypeError;

typedef struct
{
    unsigned int typeOffset;
    int subtypeOffset;
    unsigned int typeLevel;
    char typeName[MAX_FIELD_NAME_LEN];
    char subtypeName[MAX_FIELD_NAME_LEN];
}portProtocol;

typedef struct
{
    uint64_t ** numbersDef, ** numbersExt;
    uint8_t *** stringsDef, *** stringsExt;
    short * defaultResult, * extraResult;
}filterResults;

typedef struct {
    unsigned char data[PTCL_BUFFER_SIZE];
    unsigned int head;
    unsigned int tail;
    unsigned int minLenBytes;
    int rxErrors;
}circularBuffer_t;

/*!
 * A structure to contain information for a protocol in a byte-oriented port 
 */
typedef struct
{
    unsigned int sizeFieldOffsetInBits;
    unsigned int sizeFieldTrimInBits;
    int constSizeInBits;
    unsigned int refUnit;
    enum {
        PTCL_POWER_NONE,
        PTCL_POWER_BASE_2,
        PTCL_POWER_BASE_2_WITH_0
    } refPower;
    unsigned char syncPattern[MAX_PHY_HDR];
    unsigned int syncPatternLength;
    unsigned char portValid;
    unsigned char portConnected;
    circularBuffer_t buffer;
}protocolConfig;

/*!
 * A structure to contain all necessary information for a SpW port
 */
typedef struct
{
    void * ptr;                 /**< Handle (brick, board) */
    unsigned int channel;       /**< Channel (MK2 brick) */
    unsigned char link;		/**< Tx/Rx link of the port (phy) */
    unsigned char protocolID;	/**< Protocol ID (logical SpW info) */
    unsigned char writingPort;	/**< Writing index of the port (phy) */
    unsigned int numberOfReadingPorts;	/**< Reading index of the port (phy) */
    unsigned char readingPorts[MAX_READ_PORTS];
    unsigned int deviceId;	/**< deviceId (MK2 brick) */
}spwConfig;

/*!
 * A structure to contain all necessary information for a uart port
 */
typedef struct
{
    int socketHdl;
    unsigned int portNum;
    char ipAddress[16];
    struct sockaddr_in address;
}socketConfig;

/*!
 * A structure to contain all necessary information for a socket port
 */
typedef struct
{
    int portNum;
    char portName[MAX_DEV_NAME_LEN];
}uartConfig;

/*!
 * A structure to contain all necessary information for a port (interface)
 */
typedef struct portConfig_tag
{
    portTypes portType; 	/**< Type of the port to be configured */
    portTypes portTypeAux; 	/**< Type of the port for SpW TC / SpW error */
    ioTypes ioType;                 /**< I/O type of the port to be configured */
    union {
        spwConfig spw;
        uartConfig uart;
        socketConfig socket;
    }config;
    protocolConfig ptcl;
    char name[MAX_PORT_NAME_LEN];
    int protocolID;
    char deviceInfo[MAX_DEV_INFO_LEN];
    unsigned int portPhyHeaderOffsetTM;
    unsigned int portPhyHeaderOffsetTC;
}portConfig;

/*!
 * A structure to contain all necessary information for a configure a serial port
 */
typedef enum {
    HW,
    RTS_CTS,
    XON_OFF,
    DTR_DSR
} flowControl;
    
typedef struct serialConfig_tag
{
    int parity;
    int dataBits;
    int stopBits;
    long baudRate;
    flowControl control;
}serialConfig;

/*!
 * A structure to contain specific information of a variable field
 */
typedef struct variableField_Tag
{
    int constSizeInBits;	/**< Constant size (bits) (can be negative) */
    unsigned int fieldRef;	/**< Field which containts variable size */
    unsigned int refUnit;	/**< Value for references sizes (multiplier) */
    enum {
        NO_POWER,                   /**< No power */
        BASE_2,                     /**< 2 to the base of ref */
        BASE_2_WITH_0               /**< 2 to the base of ref, but 0 is 0 not 1 */
    } refPower;			/**< Whether reference must be powered */
    unsigned int maxSizeInBits;	/**< Max size of the field (bits) */
} variableField;

/*!
 * A structure to contain specific information of an array field
 */
typedef struct arrayField_Tag
{
    unsigned int fieldRef;	/**< Field which containts number of elements */
    unsigned int maxItems;	/**< Maximum number of items */
    unsigned int sizeOfItem;    /**< Size of one item */
} arrayField;

/*!
 * A structure to contain specific information of an array item field
 */
typedef struct arrayItem_Tag
{
    unsigned int arrayRef;	/**< Array to which the field belongs */
    unsigned int sizeInBits;	/**< Size of the item (bits) */
} arrayItem;

/*!
 * A structure to contain specific information of a floating DIC field  
 */
typedef struct checkRef_Tag
{
    unsigned int fieldRef;	/**< Field which is before the floating field */
    unsigned int sizeInBits;	/**< Size (bits) */
    enum {
        CRC_16,
        CHECKSUM_16,
    } type;
} checkRef;

/*!
 * A structure to contain specific information of a Constant Formula field  
 */
typedef struct formulaField_Tag
{
    unsigned int sizeInBits;	/**< Size (bits) */
    double slope;
    double intercept;
} formulaField;

typedef enum {
    CSFIELD,   			/**< Constant size field */
    CSFORMULAFIELD,             /**< Constant size field with formula */
    VSFIELD,   			/**< Virtual size field */
    FDICFIELD, 			/**< Floating DIC field */
    VRFIELDSIZE,                /**< Virtual field size */
    AFIELD,                     /**< Array field */
    AIFIELD,                     /**< Array item field */
    NOFIELD
} formatType;

/*!
 * A structure to contain all necessary information for any field
 */		
typedef struct formatField_Tag
{
    formatType type;                /**< Type of field */
    unsigned int pfid;              /**< Parent fid (fid will be the index) */
    char name[MAX_FIELD_NAME_LEN];  /**< Field name */
    char descr[MAX_FIELD_NAME_LEN]; /**< Field description */
    union {
        unsigned int sizeInBits;        /**< Size (bits) for CSFIELDs */
        variableField variable;     	/**< Struct with VFIELD information */
        checkRef check;			/**< Struct with FDIC information */
        unsigned long long virtualSizeValue;	/**< Current value of virtualSize */
        arrayField array;		/**< Struct with AFIELD information */
        arrayItem item;			/**< Struct with AIFIELD information */
        formulaField formula;           /**< Struct with CSFORMULAFIELD information */
    } info;                         /**< Field information */
    int totalSizeInBits;            /**< Total size (bits) */
    int offsetInBits;               /**< Offset (bits) */
    unsigned char exported;		/**< Field to export information */
} formatField;

/*!
 * A structure to contain all necessary information for export a field
 */
typedef struct export_Tag
{
    enum {
        EX_CONST_SIZE_BITS,	/**< Export a constant field size */
        EX_VBLE_SIZE_BITS, 	/**< Export a variable field size */
        EX_FIELD,		/**< Export a whole field */
        EX_CONSTANT,		/**< Export a constant value */
        EX_CONSTANT_HEX,	/**< Export a constant hex value defined in XML */
        EX_FILE,		/**< Export from file */
        EX_FILE_LINE,		/**< Export from file line */
        EX_ARRAY_CONST,		/**< Export to AI from const */
        EX_ARRAY_FILE           /**< Export to AI from file */
    } type; 			/**< Type of the export */
    union {
        unsigned long long constOrSizeInBits;/**< Integer (constant or size) */
        struct
        {
            unsigned char data[MAX_STR_OPERAND_LEN];
            unsigned int size;
        }file;
        struct {
            unsigned int sizeRef;       /**< Ref of vble field to export its size */
            int addSize;                /**< Extra size to be added */
        } vbleSizeInBits;           /**< Non-def number (variable size) */
        unsigned int sourceRef;     /**< Source field reference (for fields) */
    } data;			/**< Export information */ 
    unsigned int index;		/**< Index of the array to copy in */
    unsigned int targetFieldRef;		/**< Field to export information */
} levelExport;

/*!
 * A structure to contain all necessary information for a level's input
 */
typedef struct levelIn_Tag
{
    formatField * TCFields; 		/**< Telecommand fields */
    unsigned int numberOfTCFields;	/**< Number of telecommand fields */
    unsigned int numberOfFDICTCFields;	/**< Number of FDIC fields */
    unsigned int ** crcTCFieldRefs;	/**< CRC TC fields references */
    unsigned int * numberOfcrcTCFields;	/**< Number of CRC TC fields refs */
    levelExport * exportFields;  	/**< Fields to export */
    unsigned int numberOfExportFields;	/**< Number of fields to export */
    unsigned int * ActiveDICs;
    unsigned int numberOfActiveDICs;
} levelIn;

/*!
 * A structure to contain all necessary information for a bool var
 */		
typedef struct boolVar_Tag
{
    char field[MAX_FIELD_NAME_LEN]; 	/**< Field to be checked */
    enum {
        NORMAL,                             /**< BoolVar */
        FROM_ARRAY,                         /**< BoolVarFromArrayItem */
        FROM_GROUP_ARRAY,                   /**< BoolVarFromGroupedArrayItem */
        FDIC_BV                             /**< FDICBoolVar */
    } type;                             /**< Type of BoolVar */
    enum {
        EQUAL,                              /**< == */
        DIFFERENT,                          /**< != */
        BIGGER_THAN,                        /**< > */
        SMALLER_THAN,                       /**< < */
        BIGGER_OR_EQUAL,                    /**< >= */
        SMALLER_OR_EQUAL                    /**< <= */
    } operation;                        /**< Operation to be performed */
    enum {
        DEC_BASE,                           /**< Decimal base */
        HEX_BASE,                           /**< Hexadecimal base */
        BIN_BASE,                           /**< Binary base */
        STRING_CHAR,                        /**< Character */
        STRING                              /**< String */
    } valueType;                        /**< Type of operand */		
    int indexGroup;			/**< Index or Group */
    uint64_t number; 
    uint64_t mask;		/**< Mask for hex types */
    uint8_t string[MAX_STR_OPERAND_LEN];	/**< Character or string */
    uint8_t result; 		/**< Bool var result (0 or 1) */
} boolVar;

/*!
 * A structure to contain all necessary information for a filter
 */		
typedef struct filter_Tag
{
    unsigned int * boolVarRef;		/**< References to bool vars */
    unsigned int numberOfBoolVarRefs;   /**< Number of bool vars references */
    unsigned short result; 		/**< Filter result (0 or 1) */
} filter;

/*!
 * A structure to contain all necessary information for import a virtual field
 */   
typedef struct importVirtual_tag
{
    unsigned int sourceRef;		/**< Source field reference */
    unsigned int targetRef;		/**< Target field reference  */
    int addSizeInBits;			/**< Extra size (if needed) (bits) */
}importVirtual;

typedef enum {
    MINTERM,
    MAXTERM,
    FILTER_NOT_DEF
} filterTypes;

/*!
 * A structure to contain all necessary information for a level's output
 */
typedef struct levelOut_Tag
{
    formatField * TMFields;		/**< Telemetry fields */
    unsigned int numberOfTMFields;	/**< Number of telemetry fields */
    unsigned int numberOfFDICTMFields;	/**< Number of FDIC fields */
    unsigned int ** crcTMFieldRefs;	/**< CRC TM fields references */
    unsigned int * numberOfcrcTMFields;	/**< Number of CRC TM fields refs */
    boolVar * boolVars;			/**< Bool variables */
    unsigned int numberOfBoolVars;	/**< Number of bool variables */
    filter * filters;	 	 	/**< Filters */
    unsigned int numberOfFilters;	/**< Number of filters */
    filterTypes typeOfFilters; 	/**< Type of filters (MIN / MAXTERM) */
    unsigned int defaultFilter;		/**< Apply default filter? (0 or 1) */
    unsigned int importInBytes; 	/**< Import offset (bytes - position) */
    importVirtual * virtualFields;  	/**< Virtual import fields */
    int numberOfVirtualFields;  	/**< Number of virtual import fields */
} levelOut;

/*!
 * A structure to contain all necessary information for a level
 */
typedef struct level_Tag
{
    levelIn in;  			/**< Input of the level */
    levelOut out; 			/**< Output of the level */
} level;

/*!
 * A structure to contain all necessary information for a step's input
 */
typedef struct input_tag
{
    unsigned int ifRef; 		/**< Port (interface) index */
    unsigned int delayInMs;		/**< Delay between inputs (ms) */
    levelIn * level;			/**< Pointer to input levels configured */
    unsigned int numberOfLevels;	/**< Number of levels configured */
}input;

/*!
 * A structure to contain all necessary information for a step's output
 */
typedef struct output_tag 
{
    unsigned int ifRef;			/**< Port (interface) index */
    levelOut * level;			/**< Pointer to output levels configured */
    unsigned int numberOfLevels;        /**< Number of levels configured */
    unsigned char received;		/**< Mark when packets have been received */
    unsigned char optional;		/**< Mark whether it is an optional packet */
    unsigned int type;
    unsigned int subtype;
}output;


/*!
 * A structure to contain all necessary information for a special packet
 */
typedef struct special_packet_info_tag
{
    char name[MAX_FIELD_NAME_LEN];	/**< Special packet name */
    enum {
        SPECIAL_NORMAL,
        SPECIAL_PERIODIC,
        SPECIAL_INTERVAL
    } type;
    unsigned char print_enabled;
    int struct_show;
    int struct_show_field;
    double minValueMs;
    unsigned int timesUnderValue;
    double maxValueMs;
    unsigned int timesOverValue;
    double lastTimeMs;
    unsigned char outOfMinMax;
    double intervalTimeMs;
    unsigned int currentIntervalValue;
    unsigned int counter;
    unsigned char enabled;
    unsigned char period_enabled;
}special_packet_info;

/*!
 * A structure to contain all necessary information enabling / disabling special packets
 */
typedef struct spec_in_step_tag {
    enum {
        ENABLE,
        DISABLE,
        ENABLE_PRINT,
        DISABLE_PRINT
    } mode;
    unsigned int id;
} spec_in_step;

typedef struct actionStruct_tag
{
    char actionMessage[MAX_ACTION_MSG_LEN];
    enum {
        INSTRUCTION_ACTION,             /**< Instruction action */
        TMTC_CHECKING_ACTION,           /**< TMTC checking action */
        CHECKING_ACTION,          /**< Checking action */
    } type;                         /**< action type */
    unsigned int delayInMs;
    unsigned int spanInMs;
}actionStruct;

/*!
 * A structure to contain all necessary information for a step
 */
typedef struct stepStruct_tag
{
    char name[MAX_STEP_NAME_LEN];
    int prevStepIdRef;              /**< Prev step which has to be checked */
    int outIdRefFromPrevStep;       /**< Prev output which has to be checked */
    enum {
        CONTINUOUS,                     /**< Continuous mode: not interaction needed */
        MANUAL,                         /**< Manual mode: User interaction needed */
        CONCURRENT,                     /**< Concurrent mode */
        ACTION,                     /**< Concurrent mode */
    } mode;                         /**< Step mode: continuous, manual */
    input * inputs;                 /**< Pointer to the step's inputs */
    unsigned int numberOfInputs;    /**< Number of step's inputs */
    actionStruct * action;          /**< Pointer to the step action */
    double intervalInMs;            /**< Interval validity (ms) */
    enum {
        ALL,                            /**< All packets must be received in order */ 
        ALLUNSORTED,                    /**< All packets must be received */
        ANY                             /**< Any packet could be received */
    } outputsCheckmode;             /**< How packets are received */
    output * outputs;               /**< Pointer to the step's outputs */
    unsigned int numberOfOutputs;   /**< Total numberof outputs in step */
    unsigned int outputsReceived;   /**< Number of step's outputs received */
    unsigned int numberOfOutputsAtPort[MAX_INTERFACES]; /**< optional don't count */
    unsigned int outputsReceivedAtPort[MAX_INTERFACES];
    int nextStepWithOutputsAtPort[MAX_INTERFACES];
    spec_in_step * specials;        /**< Special packets to enable/disable */
    unsigned int numberOfSpecials;  /**< Number of specials in step */
    struct {												
            unsigned int * list;        /**< Concurrent steps */
            unsigned int number;        /**< Number of concurrent steps */
            unsigned int next;          /**< Next step */	
            unsigned char nextIsNotConc;/**< Next step is not concurrent */
    }concurrent;
    unsigned int replays;           /** Number of replays of the step */
}stepStruct;

/*!
 * A structure to contain all necessary information for export a size
 */
typedef struct sizesType_tag
{
    enum {
        C_SIZE,                     /**< Constant size (from file or from const size field) */
        V_SIZE                      /**< Variable size (from variable size field) */
    } type;                     /**< Type of size */
    int sizeOrRef;		/**< Size (const) (bits) or field ref (variable) */
    int addSizeInBits;		/**< Extra size (if needed) (bits) */
}sizesType;


/*!
 * A structure to contain all necessary information for a global variable
 */
typedef struct globalVar_tag
{
    char name[MAX_FIELD_NAME_LEN];	/**< GlobalVar name */
    enum {
        GV_UINT,                       /**< Integer number */
        GV_FORMULA
    } dataType;                         /**< Type of data */
    double value;
    enum {
        GLOBAL_TC,
        GLOBAL_TM,
        GLOBAL_TC_PERIOD,
        GLOBAL_TM_SPECIAL
    } commandType;
    unsigned int level;
    unsigned int fieldRef;
    unsigned int Interface;
    int idRef;
    unsigned int offset;
    unsigned int size;
}globalVar;

typedef enum {
    GVR_FILTERED_TM,
    GVR_FILTERED_TC,
    GVR_PERIODIC
} typeOfGlobalVarRef;	/**< Type of global var ref */

enum alarmTypes {
    ALARM1,
    ALARM2,
    ALARM3,
};
enum modifyTypes {
    INCREMENT,
    INCREMENT_1_WRAP,
};

typedef struct chart_tag
{
    char name[MAX_FIELD_NAME_LEN];	/**< Chart name */
    double yMax;
    char yAxisUnit[MAX_FIELD_NAME_LEN];
    unsigned int numberOfPlots;
}chart;

/*!
 * A structure to contain all necessary information for a monitor
 */
typedef struct monitor_tag
{
    char name[MAX_FIELD_NAME_LEN];	/**< Monitor name */
    enum {
        PLOT,
        ALARM_MSG,
        MODIFY,
        ALARM_VAL
    } type;			/**< Type of monitor */
    union {
        enum alarmTypes alarmType;
        enum modifyTypes modifyType;
    } dataType;
    union {
        unsigned int chartRef;		/**< For modify */
        char msg[MAX_ALARM_MSG_LEN];    /**< For plot */
        unsigned int value;		/**< For modify */
        int controlIdx;                 /**< For alarm val */
    } data;
    unsigned int globarVarRef;
    typeOfGlobalVarRef typeOfRef;	/**< Type of global var ref */
    union {
        levelOut globalVarFilter;
        unsigned int periodInMs;
    } monitorRef;
}monitor;

/*!
 * A structure to contain all necessary information for a test procedures
 */
typedef struct test_proc_tag
{
    char name[MAX_PROC_NAME_LEN];	/**< Name of the procedure */
    char filename[MAX_PROC_FILE_LEN];   /**< Filename of the procedure */
    char prevMsg[MAX_PROC_NAME_LEN];	/**< Text command of the procedure */
    unsigned char prevAction;		/**< GSS action to be performed */
    char prevActionParam[MAX_PROC_NAME_LEN];/**< Parameter for the GSS action*/
    unsigned char enabled;		/**< Whether TP is enabled or not  */
}test_proc;

typedef struct protocolPacket_tag
{
    levelIn in[MAX_PROTOCOL_PACKET_LEVELS];
    unsigned int ifRef;
    unsigned int levelRef;
    char name[MAX_FIELD_NAME_LEN];
}protocolPacket;

/*!
 * A structure to contain all necessary information for the gss config
 */
typedef struct gss_config_tag
{
    unsigned int numberOfPorts;  		/**< Number of interfaces */
    int mainPort;				/**< Main port - debugging and logging */
    int mainPortAux;				/**< Port related to main*/
    level levels[MAX_INTERFACES][MAX_LEVELS];   /**< Pointer to levels */
    unsigned int numberOfLevels[MAX_INTERFACES];/**< Number of default levels */
    output * specialPackets;			/**< Pointer to special packets */
    special_packet_info * specialInfo;		/**< Pointer to special info */
    unsigned int numberOfSpecialPackets;	/**< Number of special packets */
    char ** periodicTCnames;
    input * periodicTCs;			/**< Pointer to periodic TCs */
    unsigned int numberOfPeriodicTCs;		/**< Number of periodic TCs */
    globalVar * globalVars;			/**< Pointer to global vars */
    unsigned int numberOfGlobalVars;		/**< Number of global vars */
    chart * charts;				/**< Pointer to charts */
    unsigned int numberOfCharts;		/**< Number of charts */
    monitor * monitors;				/**< Pointer to monitors */
    unsigned int numberOfMonitors;		/**< Number of monitors */
    test_proc procedures[MAX_PROCEDURES];
    unsigned int numberOfProcedures;		/**< Number of procedures */
    protocolPacket * protocolPackets;
    unsigned int numberOfProtocolPackets;
}gss_config;

typedef struct gss_options_tag
{
    char test_campaign[MAX_FIELD_NAME_LEN];
    char version[MAX_DATE_VERSION_SIZE];
    char date[MAX_DATE_VERSION_SIZE];
    char version_control_url[MAX_MSG_SIZE];
    enabled_status gss_info_print[NUMBER_OF_LOGS];
    enabled_status phy_header_print[NUMBER_OF_LOGS];
    enabled_status phy_header_gss_tabs;
    enabled_status discardErrorFlags[TEST_ERRORS];
}gss_options;

#endif  /* ndef __definitions_H__ */