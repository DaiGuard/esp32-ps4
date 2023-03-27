#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "include/ps4.h"
#include "include/ps4_int.h"
#include "esp_log.h"

#define  PS4_TAG "PS4_PARSER"


/********************************************************************************/
/*                            L O C A L    T Y P E S                            */
/********************************************************************************/

enum ps4_packet_index {

    ps4_packet_index_analog_stick_lx = 11,
    ps4_packet_index_analog_stick_ly = 12,
    ps4_packet_index_analog_stick_rx = 13,
    ps4_packet_index_analog_stick_ry = 14,

    ps4_packet_index_buttons_raw = 15,

    ps4_packet_index_analog_button_l2 = 18,
    ps4_packet_index_analog_button_r2 = 19,

    ps4_packet_index_status = 39,

    ps4_packet_index_sensor_accelerometer_x = 51,
    ps4_packet_index_sensor_accelerometer_y = 53,
    ps4_packet_index_sensor_accelerometer_z = 55,
    ps4_packet_index_sensor_gyroscope_z = 57
};

enum ps4_button_mask {

    ps4_button_mask_direct   = 0xf,

    ps4_button_mask_square   = 1 << 4,
    ps4_button_mask_cross    = 1 << 5,
    ps4_button_mask_circle   = 1 << 6,
    ps4_button_mask_triangle = 1 << 7,

    ps4_button_mask_l1       = 1 << 8,
    ps4_button_mask_r1       = 1 << 9,
    ps4_button_mask_l2       = 1 << 10,
    ps4_button_mask_r2       = 1 << 11,

    ps4_button_mask_share    = 1 << 12,
    ps4_button_mask_option   = 1 << 13,
    ps4_button_mask_l3       = 1 << 14,
    ps4_button_mask_r3       = 1 << 15,

    ps4_button_mask_ps       = 1 << 16,
    ps4_button_mask_touch    = 1 << 17,
};

enum ps4_status_mask {
    ps4_status_mask_rumbling = 0x02,
    ps4_status_mask_bluetooth = 0x04
};


/********************************************************************************/
/*              L O C A L    F U N C T I O N     P R O T O T Y P E S            */
/********************************************************************************/

ps4_sensor_t ps4_parse_packet_sensor( uint8_t *packet );
ps4_status_t ps4_parse_packet_status( uint8_t *packet );
ps4_analog_stick_t ps4_parse_packet_analog_stick( uint8_t *packet );
ps4_analog_button_t ps4_parse_packet_analog_button( uint8_t *packet );
ps4_button_t ps4_parse_packet_buttons( uint8_t *packet );
ps4_event_t ps4_parse_event( ps4_t prev, ps4_t cur );


/********************************************************************************/
/*                         L O C A L    V A R I A B L E S                       */
/********************************************************************************/

static ps4_t ps4;
static ps4_event_callback_t ps4_event_cb = NULL;


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/
void ps4_parser_set_event_cb( ps4_event_callback_t cb )
{
    ps4_event_cb = cb;
}

void ps4_parse_packet( uint8_t *packet)
{
    ps4_t prev_ps4 = ps4;

    ps4.button        = ps4_parse_packet_buttons(packet);
    ps4.analog.stick  = ps4_parse_packet_analog_stick(packet);
    ps4.analog.button = ps4_parse_packet_analog_button(packet);
    ps4.sensor        = ps4_parse_packet_sensor(packet);
    ps4.status        = ps4_parse_packet_status(packet);

    ps4_event_t ps4_event = ps4_parse_event( prev_ps4, ps4 );
    
    ps4_packet_event( ps4, ps4_event );
}


/********************************************************************************/
/*                      L O C A L    F U N C T I O N S                          */
/********************************************************************************/

/******************/
/*    E V E N T   */
/******************/
ps4_event_t ps4_parse_event( ps4_t prev, ps4_t cur )
{
    ps4_event_t ps4_event;

    /* Button down events */
    ps4_event.button_down.up       = !prev.button.up       && cur.button.up;
    ps4_event.button_down.right    = !prev.button.right    && cur.button.right;
    ps4_event.button_down.down     = !prev.button.down     && cur.button.down;
    ps4_event.button_down.left     = !prev.button.left     && cur.button.left;

    ps4_event.button_down.square   = !prev.button.square   && cur.button.square;
    ps4_event.button_down.cross    = !prev.button.cross    && cur.button.cross;
    ps4_event.button_down.circle   = !prev.button.circle   && cur.button.circle;
    ps4_event.button_down.triangle = !prev.button.triangle && cur.button.triangle;

    ps4_event.button_down.l1       = !prev.button.l1       && cur.button.l1;
    ps4_event.button_down.r1       = !prev.button.r1       && cur.button.r1;
    ps4_event.button_down.l2       = !prev.button.l2       && cur.button.l2;
    ps4_event.button_down.r2       = !prev.button.r2       && cur.button.r2;

    ps4_event.button_down.share    = !prev.button.share    && cur.button.share;
    ps4_event.button_down.option   = !prev.button.option   && cur.button.option;
    ps4_event.button_down.l3       = !prev.button.l3       && cur.button.l3;
    ps4_event.button_down.r3       = !prev.button.r3       && cur.button.r3;

    ps4_event.button_down.ps       = !prev.button.ps       && cur.button.ps;
    ps4_event.button_down.touch    = !prev.button.touch    && cur.button.touch;

    /* Button up events */
    ps4_event.button_up.up         = prev.button.up       && !cur.button.up;
    ps4_event.button_up.right      = prev.button.right    && !cur.button.right;
    ps4_event.button_up.down       = prev.button.down     && !cur.button.down;
    ps4_event.button_up.left       = prev.button.left     && !cur.button.left;

    ps4_event.button_up.square     = prev.button.square   && !cur.button.square;
    ps4_event.button_up.cross      = prev.button.cross    && !cur.button.cross;
    ps4_event.button_up.circle     = prev.button.circle   && !cur.button.circle;
    ps4_event.button_up.triangle   = prev.button.triangle && !cur.button.triangle;

    ps4_event.button_up.l1         = prev.button.l1       && !cur.button.l1;
    ps4_event.button_up.r1         = prev.button.r1       && !cur.button.r1;
    ps4_event.button_up.l2         = prev.button.l2       && !cur.button.l2;
    ps4_event.button_up.r2         = prev.button.r2       && !cur.button.r2;

    ps4_event.button_up.share      = prev.button.share    && !cur.button.share;
    ps4_event.button_up.option     = prev.button.option   && !cur.button.option;
    ps4_event.button_up.l3         = prev.button.l3       && !cur.button.l3;
    ps4_event.button_up.r3         = prev.button.r3       && !cur.button.r3;

    ps4_event.button_up.ps         = prev.button.ps       && !cur.button.ps;
    ps4_event.button_up.touch      = prev.button.touch    && !cur.button.touch;

    /* Analog events */
    ps4_event.analog_changed.stick.lx        = cur.analog.stick.lx - prev.analog.stick.lx;
    ps4_event.analog_changed.stick.ly        = cur.analog.stick.ly - prev.analog.stick.ly;
    ps4_event.analog_changed.stick.rx        = cur.analog.stick.rx - prev.analog.stick.rx;
    ps4_event.analog_changed.stick.ry        = cur.analog.stick.ry - prev.analog.stick.ry;

    ps4_event.analog_changed.button.l2       = cur.analog.button.l2 - prev.analog.button.l2;
    ps4_event.analog_changed.button.r2       = cur.analog.button.r2 - prev.analog.button.r2;

    return ps4_event;
}

/********************/
/*    A N A L O G   */
/********************/
ps4_analog_stick_t ps4_parse_packet_analog_stick( uint8_t *packet )
{
    ps4_analog_stick_t ps4_analog_stick;

    const uint8_t int_offset = 0x80;

    ps4_analog_stick.lx = (int16_t)packet[ps4_packet_index_analog_stick_lx] - int_offset;
    ps4_analog_stick.ly = (int16_t)packet[ps4_packet_index_analog_stick_ly] - int_offset;
    ps4_analog_stick.rx = (int16_t)packet[ps4_packet_index_analog_stick_rx] - int_offset;
    ps4_analog_stick.ry = (int16_t)packet[ps4_packet_index_analog_stick_ry] - int_offset;

    return ps4_analog_stick;
}

ps4_analog_button_t ps4_parse_packet_analog_button( uint8_t *packet )
{
    ps4_analog_button_t ps4_analog_button;

    ps4_analog_button.l2       = packet[ps4_packet_index_analog_button_l2];
    ps4_analog_button.r2       = packet[ps4_packet_index_analog_button_r2];

    return ps4_analog_button;
}

/*********************/
/*   B U T T O N S   */
/*********************/

ps4_button_t ps4_parse_packet_buttons( uint8_t *packet )
{
    ps4_button_t ps4_button;
    uint32_t ps4_buttons_raw = *((uint32_t*)&packet[ps4_packet_index_buttons_raw]);

    uint8_t direct         = (uint8_t)(ps4_buttons_raw & ps4_button_mask_direct);
    switch (direct)
    {
    case 0:
        ps4_button.up    = true;
        ps4_button.right = false;
        ps4_button.down  = false;
        ps4_button.left  = false;
        break;
    case 1:
        ps4_button.up    = true;
        ps4_button.right = true;
        ps4_button.down  = false;
        ps4_button.left  = false;
        break;
    case 2:
        ps4_button.up    = false;
        ps4_button.right = true;
        ps4_button.down  = false;
        ps4_button.left  = false;
        break;
    case 3:
        ps4_button.up    = false;
        ps4_button.right = true;
        ps4_button.down  = true;
        ps4_button.left  = false;
        break;
    case 4:
        ps4_button.up    = false;
        ps4_button.right = false;
        ps4_button.down  = true;
        ps4_button.left  = false;
        break;
    case 5:
        ps4_button.up    = false;
        ps4_button.right = false;
        ps4_button.down  = true;
        ps4_button.left  = true;
        break;
    case 6:
        ps4_button.up    = false;
        ps4_button.right = false;
        ps4_button.down  = false;
        ps4_button.left  = true;
        break;
    case 7:
        ps4_button.up    = true;
        ps4_button.right = false;
        ps4_button.down  = false;
        ps4_button.left  = true;
        break;
    default:
        ps4_button.up    = false;
        ps4_button.right = false;
        ps4_button.down  = false;
        ps4_button.left  = false;
        break;
    }

    ps4_button.square   = (ps4_buttons_raw & ps4_button_mask_square)   ? true : false;
    ps4_button.cross    = (ps4_buttons_raw & ps4_button_mask_cross)    ? true : false;
    ps4_button.circle   = (ps4_buttons_raw & ps4_button_mask_circle)   ? true : false;
    ps4_button.triangle = (ps4_buttons_raw & ps4_button_mask_triangle) ? true : false;

    ps4_button.l1       = (ps4_buttons_raw & ps4_button_mask_l1)       ? true : false;
    ps4_button.r1       = (ps4_buttons_raw & ps4_button_mask_r1)       ? true : false;
    ps4_button.l2       = (ps4_buttons_raw & ps4_button_mask_l2)       ? true : false;
    ps4_button.r2       = (ps4_buttons_raw & ps4_button_mask_r2)       ? true : false;

    ps4_button.share    = (ps4_buttons_raw & ps4_button_mask_share)    ? true : false;
    ps4_button.option   = (ps4_buttons_raw & ps4_button_mask_option)   ? true : false;
    ps4_button.l3       = (ps4_buttons_raw & ps4_button_mask_l3)       ? true : false;
    ps4_button.r3       = (ps4_buttons_raw & ps4_button_mask_r3)       ? true : false;

    ps4_button.ps       = (ps4_buttons_raw & ps4_button_mask_ps)       ? true : false;
    ps4_button.touch    = (ps4_buttons_raw & ps4_button_mask_touch)    ? true : false;

    return ps4_button;
}

/*******************************/
/*   S T A T U S   F L A G S   */
/*******************************/
ps4_status_t ps4_parse_packet_status( uint8_t *packet )
{
    ps4_status_t ps4_status;

    // ps4_status.battery    =  packet[ps4_packet_index_status+1];
    // ps4_status.charging   =  ps4_status.battery == ps4_status_battery_charging;
    // ps4_status.connection = (packet[ps4_packet_index_status+2] & ps4_status_mask_bluetooth) ? ps4_status_connection_bluetooth : ps4_status_connection_usb;
    // ps4_status.rumbling   = (packet[ps4_packet_index_status+2] & ps4_status_mask_rumbling) ? false: true;

    return ps4_status;
}

/********************/
/*   S E N S O R S  */
/********************/
ps4_sensor_t ps4_parse_packet_sensor( uint8_t *packet )
{
    ps4_sensor_t ps4_sensor;

    const uint16_t int_offset = 0x200;

    // ps4_sensor.accelerometer.x = (packet[ps4_packet_index_sensor_accelerometer_x] << 8) + packet[ps4_packet_index_sensor_accelerometer_x+1] - int_offset;
    // ps4_sensor.accelerometer.y = (packet[ps4_packet_index_sensor_accelerometer_y] << 8) + packet[ps4_packet_index_sensor_accelerometer_y+1] - int_offset;
    // ps4_sensor.accelerometer.z = (packet[ps4_packet_index_sensor_accelerometer_z] << 8) + packet[ps4_packet_index_sensor_accelerometer_z+1] - int_offset;
    // ps4_sensor.gyroscope.z     = (packet[ps4_packet_index_sensor_gyroscope_z]     << 8) + packet[ps4_packet_index_sensor_gyroscope_z+1]     - int_offset;

    return ps4_sensor;

}
