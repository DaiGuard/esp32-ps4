#ifndef PS4_H
#define PS4_H


/********************************************************************************/
/*                                  T Y P E S                                   */
/********************************************************************************/

/********************/
/*    A N A L O G   */
/********************/

typedef struct {
    // 11byte
    int8_t lx;
    // 12byte
    int8_t ly;
    // 13byte
    int8_t rx;
    // 14byte
    int8_t ry;
} ps4_analog_stick_t;

typedef struct {
    // 18byte
    uint8_t l2;
    // 19byte
    uint8_t r2;
} ps4_analog_button_t;

typedef struct {
    ps4_analog_stick_t stick;
    ps4_analog_button_t button;
} ps4_analog_t;


/*********************/
/*   B U T T O N S   */
/*********************/

typedef struct {
    // 15byte
    uint8_t up       : 1;
    uint8_t right    : 1;
    uint8_t down     : 1;
    uint8_t left     : 1;

    uint8_t square   : 1;
    uint8_t cross    : 1;
    uint8_t circle   : 1;
    uint8_t triangle : 1;

    // 16byte
    uint8_t l1       : 1;
    uint8_t r1       : 1;
    uint8_t l2       : 1;
    uint8_t r2       : 1;

    uint8_t share    : 1;
    uint8_t option   : 1;
    uint8_t l3       : 1;
    uint8_t r3       : 1;

    // 17byte
    uint8_t ps       : 1;
    uint8_t touch    : 1;
} ps4_button_t;


/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/

enum ps4_status_battery {
    ps4_status_battery_shutdown = 0x01,
    ps4_status_battery_dying    = 0x02,
    ps4_status_battery_low      = 0x03,
    ps4_status_battery_high     = 0x04,
    ps4_status_battery_full     = 0x05,
    ps4_status_battery_charging = 0xEE
};

enum ps4_status_connection {
    ps4_status_connection_usb,
    ps4_status_connection_bluetooth
};

typedef struct {
    enum ps4_status_battery battery;
    enum ps4_status_connection connection;
    uint8_t charging : 1;
    uint8_t rumbling : 1;
} ps4_status_t;


/********************/
/*   S E N S O R S  */
/********************/

typedef struct {
    int16_t z;
} ps4_sensor_gyroscope_t;

typedef struct {
    int16_t x;
    int16_t y;
    int16_t z;
} ps4_sensor_accelerometer_t;

typedef struct {
    ps4_sensor_accelerometer_t accelerometer;
    ps4_sensor_gyroscope_t gyroscope;
} ps4_sensor_t;


/*******************/
/*    O T H E R    */
/*******************/

typedef struct {
    /* Rumble control */
    uint8_t rumble_right_duration;
    uint8_t rumble_right_intensity;
    uint8_t rumble_left_duration;
    uint8_t rumble_left_intensity;

    /* LED control */
    uint8_t led1 : 1;
    uint8_t led2 : 1;
    uint8_t led3 : 1;
    uint8_t led4 : 1;
} ps4_cmd_t;

typedef struct {
    ps4_button_t button_down;
    ps4_button_t button_up;
    ps4_analog_t analog_changed;
} ps4_event_t;

typedef struct {
    ps4_analog_t analog;
    ps4_button_t button;
    ps4_status_t status;
    ps4_sensor_t sensor;
} ps4_t;


/***************************/
/*    C A L L B A C K S    */
/***************************/

typedef void(*ps4_connection_callback_t)( uint8_t is_connected );
typedef void(*ps4_connection_object_callback_t)( void *object, uint8_t is_connected );

typedef void(*ps4_event_callback_t)( ps4_t ps4, ps4_event_t event );
typedef void(*ps4_event_object_callback_t)( void *object, ps4_t ps4, ps4_event_t event );


/********************************************************************************/
/*                             F U N C T I O N S                                */
/********************************************************************************/

bool ps4IsConnected();
void ps4Init();
void ps4Deinit();
void ps4Enable();
void ps4Cmd( ps4_cmd_t ps4_cmd );
void ps4SetConnectionCallback( ps4_connection_callback_t cb );
void ps4SetConnectionObjectCallback( void *object, ps4_connection_object_callback_t cb );
void ps4SetEventCallback( ps4_event_callback_t cb );
void ps4SetEventObjectCallback( void *object, ps4_event_object_callback_t cb );
void ps4SetLed( uint8_t player );
void ps4SetLedCmd( ps4_cmd_t *cmd, uint8_t player );
void ps4SetBluetoothMacAddress( const uint8_t *mac );


#endif
