#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <esp_system.h>
#include "include/ps4.h"
#include "include/ps4_int.h"

/********************************************************************************/
/*                              C O N S T A N T S                               */
/********************************************************************************/

static const uint8_t hid_cmd_payload_ps4_enable[] = { 0x42, 0x03, 0x00, 0x00 };
static const uint8_t hid_cmd_payload_led_arguments[] = { 0xff, 0x27, 0x10, 0x00, 0x32 };


/********************************************************************************/
/*                         L O C A L    V A R I A B L E S                       */
/********************************************************************************/

static ps4_connection_callback_t ps4_connection_cb = NULL;
static ps4_connection_object_callback_t ps4_connection_object_cb = NULL;
static void *ps4_connection_object = NULL;


static ps4_event_callback_t ps4_event_cb = NULL;
static ps4_event_object_callback_t ps4_event_object_cb = NULL;
static void *ps4_event_object = NULL;

static bool is_active = false;


/********************************************************************************/
/*                      P U B L I C    F U N C T I O N S                        */
/********************************************************************************/

/*******************************************************************************
**
** Function         ps4Init
**
** Description      This initializes the bluetooth services to listen
**                  for an incoming PS4 controller connection.
**
**
** Returns          void
**
*******************************************************************************/
void ps4Init()
{
    ps4_spp_init();
    ps4_l2cap_init_services();
}

/*******************************************************************************
**
** Function         ps4Deinit
**
** Description      This deinitializes the bluetooth services to stop
**                  listening for incoming connections.
**
**
** Returns          void
**
*******************************************************************************/
void ps4Deinit()
{
    ps4_l2cap_deinit_services();
    ps4_spp_deinit();
}


/*******************************************************************************
**
** Function         ps4IsConnected
**
** Description      This returns whether a PS4 controller is connected, based
**                  on whether a successful handshake has taken place.
**
**
** Returns          bool
**
*******************************************************************************/
bool ps4IsConnected()
{
    return is_active;
}


/*******************************************************************************
**
** Function         ps4Enable
**
** Description      This triggers the PS4 controller to start continually
**                  sending its data.
**
**
** Returns          void
**
*******************************************************************************/
void ps4Enable()
{
    uint16_t len = sizeof(hid_cmd_payload_ps4_enable);
    hid_cmd_t hid_cmd;

    hid_cmd.code = hid_cmd_code_set_report | hid_cmd_code_type_feature;
    hid_cmd.identifier = hid_cmd_identifier_ps4_enable;

    memcpy( hid_cmd.data, hid_cmd_payload_ps4_enable, len);

    ps4_l2cap_send_hid( &hid_cmd, len );
}

/*******************************************************************************
**
** Function         ps4Cmd
**
** Description      Send a command to the PS4 controller.
**
**
** Returns          void
**
*******************************************************************************/
void ps4Cmd( ps4_cmd_t cmd )
{
    hid_cmd_t hid_cmd = { .data = {0} };
    uint16_t len = sizeof(hid_cmd.data);

    hid_cmd.code = hid_cmd_code_set_report | hid_cmd_code_type_output;
    hid_cmd.identifier = hid_cmd_identifier_ps4_control;

    hid_cmd.data[ps4_control_packet_index_rumble_right_duration]  = cmd.rumble_right_duration;
    hid_cmd.data[ps4_control_packet_index_rumble_right_intensity] = cmd.rumble_right_intensity;
    hid_cmd.data[ps4_control_packet_index_rumble_left_duration]   = cmd.rumble_left_duration;
    hid_cmd.data[ps4_control_packet_index_rumble_left_intensity]  = cmd.rumble_left_intensity;

    hid_cmd.data[ps4_control_packet_index_leds] = 0;
    if (cmd.led1) hid_cmd.data[ps4_control_packet_index_leds] |= ps4_led_mask_led1;
    if (cmd.led2) hid_cmd.data[ps4_control_packet_index_leds] |= ps4_led_mask_led2;
    if (cmd.led3) hid_cmd.data[ps4_control_packet_index_leds] |= ps4_led_mask_led3;
    if (cmd.led4) hid_cmd.data[ps4_control_packet_index_leds] |= ps4_led_mask_led4;

    if (cmd.led1) memcpy( hid_cmd.data + ps4_control_packet_index_led1_arguments, hid_cmd_payload_led_arguments, sizeof(hid_cmd_payload_led_arguments));
    if (cmd.led2) memcpy( hid_cmd.data + ps4_control_packet_index_led2_arguments, hid_cmd_payload_led_arguments, sizeof(hid_cmd_payload_led_arguments));
    if (cmd.led3) memcpy( hid_cmd.data + ps4_control_packet_index_led3_arguments, hid_cmd_payload_led_arguments, sizeof(hid_cmd_payload_led_arguments));
    if (cmd.led4) memcpy( hid_cmd.data + ps4_control_packet_index_led4_arguments, hid_cmd_payload_led_arguments, sizeof(hid_cmd_payload_led_arguments));

    ps4_l2cap_send_hid( &hid_cmd, len );
}


/*******************************************************************************
**
** Function         ps4SetLed
**
** Description      Sets the LEDs on the PS4 controller to the player
**                  number. Up to 10 players are supported.
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetLed( uint8_t player )
{
    ps4_cmd_t cmd = {0};
    ps4SetLedCmd(&cmd, player);
    ps4Cmd(cmd);
}


/*******************************************************************************
**
** Function         ps4SetLed
**
** Description      Sets the LED bits on the PS4 controller command to the
**                  player number. Up to 10 players are supported.
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetLedCmd( ps4_cmd_t *cmd, uint8_t player )
{

    //           led4  led3  led2  led1
    // player 1                    1
    // player 2              1
    // player 3        1
    // player 4  1
    // player 5  1                 1
    // player 6  1           1
    // player 7  1     1
    // player 8  1     1           1
    // player 9  1     1     1
    // player 10 1     1     1     1

    if( (cmd->led4 = player >= 4) != 0 ) player -= 4;
    if( (cmd->led3 = player >= 3) != 0 ) player -= 3;
    if( (cmd->led2 = player >= 2) != 0 ) player -= 2;
    if( (cmd->led1 = player >= 1) != 0 ) player -= 1;

}


/*******************************************************************************
**
** Function         ps4SetConnectionCallback
**
** Description      Registers a callback for receiving PS4 controller
**                  connection notifications
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetConnectionCallback( ps4_connection_callback_t cb )
{
    ps4_connection_cb = cb;
}


/*******************************************************************************
**
** Function         ps4SetConnectionObjectCallback
**
** Description      Registers a callback for receiving PS4 controller
**                  connection notifications
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetConnectionObjectCallback( void *object, ps4_connection_object_callback_t cb )
{
    ps4_connection_object_cb = cb;
    ps4_connection_object = object;
}

/*******************************************************************************
**
** Function         ps4SetEventCallback
**
** Description      Registers a callback for receiving PS4 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetEventCallback( ps4_event_callback_t cb )
{
    ps4_event_cb = cb;
}


/*******************************************************************************
**
** Function         ps4SetEventObjectCallback
**
** Description      Registers a callback for receiving PS4 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetEventObjectCallback( void *object, ps4_event_object_callback_t cb )
{
    ps4_event_object_cb = cb;
    ps4_event_object = object;
}


/*******************************************************************************
**
** Function         ps4SetBluetoothMacAddress
**
** Description      Writes a Registers a callback for receiving PS4 controller events
**
**
** Returns          void
**
*******************************************************************************/
void ps4SetBluetoothMacAddress( const uint8_t *mac )
{
    // The bluetooth MAC address is derived from the base MAC address
    // https://docs.espressif.com/projects/esp-idf/en/stable/api-reference/system/system.html#mac-address
    uint8_t base_mac[6];
    memcpy(base_mac, mac, 6);
    base_mac[5] -= 2;
    esp_base_mac_addr_set(base_mac);
}


/********************************************************************************/
/*                      L O C A L    F U N C T I O N S                          */
/********************************************************************************/

void ps4_connect_event( uint8_t is_connected )
{
    if(is_connected){
        ps4Enable();
    }else{
        is_active = false;
    }
}


void ps4_packet_event( ps4_t ps4, ps4_event_t event )
{
    // Trigger packet event, but if this is the very first packet
    // after connecting, trigger a connection event instead
    if(is_active){
        if(ps4_event_cb != NULL)
        {
            ps4_event_cb( ps4, event );
        }

        if(ps4_event_object_cb != NULL && ps4_event_object != NULL)
        {
            ps4_event_object_cb( ps4_event_object, ps4, event );
        }
    }else{
        is_active = true;

        if(ps4_connection_cb != NULL)
        {
            ps4_connection_cb( is_active );
        }

        if(ps4_connection_object_cb != NULL && ps4_connection_object != NULL)
        {
            ps4_connection_object_cb( ps4_connection_object, is_active );
        }
    }
}
