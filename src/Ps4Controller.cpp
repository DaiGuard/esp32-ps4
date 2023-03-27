#include "Ps4Controller.h"

#include <esp_bt_main.h>
#include <esp_bt_defs.h>

extern "C" {
#include  "esp_bt_device.h"
#include  "include/ps4.h"
}


#define ESP_BD_ADDR_HEX_STR        "%hhx:%hhx:%hhx:%hhx:%hhx:%hhx"
#define ESP_BD_ADDR_HEX_ARR(addr)   addr[0],  addr[1],  addr[2],  addr[3],  addr[4],  addr[5]
#define ESP_BD_ADDR_HEX_PTR(addr)  &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]


Ps4Controller::Ps4Controller()
{

}


bool Ps4Controller::begin()
{
    ps4SetEventObjectCallback(this, &Ps4Controller::_event_callback);
    ps4SetConnectionObjectCallback(this, &Ps4Controller::_connection_callback);

    if(!btStarted() && !btStart()){
        log_e("btStart failed");
        return false;
    }

    esp_bluedroid_status_t bt_state = esp_bluedroid_get_status();
    if(bt_state == ESP_BLUEDROID_STATUS_UNINITIALIZED){
        if (esp_bluedroid_init()) {
            log_e("esp_bluedroid_init failed");
            return false;
        }
    }

    if(bt_state != ESP_BLUEDROID_STATUS_ENABLED){
        if (esp_bluedroid_enable()) {
            log_e("esp_bluedroid_enable failed");
            return false;
        }
    }

    ps4Init();
    return true;

}


bool Ps4Controller::begin(const char *mac)
{
    esp_bd_addr_t addr;

    if (sscanf(mac, ESP_BD_ADDR_HEX_STR, ESP_BD_ADDR_HEX_PTR(addr)) != ESP_BD_ADDR_LEN){
        log_e("Could not convert %s\n to a MAC address", mac);
        return false;
    }

    ps4SetBluetoothMacAddress( addr );

    return begin();

}


bool Ps4Controller::end()
{
    ps4Deinit();
    return true;
}


String Ps4Controller::getAddress() {
    String address = "";

    if (btStarted()) {
        char mac[18];
        const uint8_t* addr = esp_bt_dev_get_address();

        sprintf(mac, ESP_BD_ADDR_STR, ESP_BD_ADDR_HEX_ARR(addr));

        address = String(mac);
    }

    return address;
}


bool Ps4Controller::isConnected()
{
    return ps4IsConnected();

}


void Ps4Controller::setPlayer(int player)
{
    this->player = player;
    ps4SetLed(player);
}


void Ps4Controller::setRumble(float intensity, int duration) {

    const float int_min = 0.0;
    const float int_max = 100.0;

    const int dur_min = 0;
    const int dur_max = 5000;

    uint8_t raw_intensity = map(constrain(intensity, int_min, int_max), int_min, int_max, 0, 255);
    uint8_t raw_duration = map(constrain(duration, dur_min, dur_max), dur_min, dur_max, 0, 254);

    if (duration == -1) {
        raw_duration = 255;
    }

    ps4_cmd_t cmd = {};

    cmd.rumble_right_intensity = raw_intensity;
    cmd.rumble_left_intensity = raw_intensity;

    cmd.rumble_right_duration = raw_duration;
    cmd.rumble_left_duration = raw_duration;

    ps4SetLedCmd(&cmd, this->player);
    ps4Cmd(cmd);

}


void Ps4Controller::attach(callback_t callback)
{
    _callback_event = callback;

}


void Ps4Controller::attachOnConnect(callback_t callback)
{
    _callback_connect = callback;

}

void Ps4Controller::attachOnDisconnect(callback_t callback)
{
    _callback_disconnect = callback;

}


void Ps4Controller::_event_callback(void *object, ps4_t data, ps4_event_t event)
{
    Ps4Controller* This = (Ps4Controller*) object;

    memcpy(&This->data, &data, sizeof(ps4_t));
    memcpy(&This->event, &event, sizeof(ps4_event_t));

    if (This->_callback_event){
        This->_callback_event();
    }
}


void Ps4Controller::_connection_callback(void *object, uint8_t is_connected)
{
    Ps4Controller* This = (Ps4Controller*) object;

    if (is_connected)
    {
        // Set LED1 by default
        This->setPlayer(1);

        if (This->_callback_connect){
            This->_callback_connect();
        }
    }else
    {
        if (This->_callback_disconnect){
            This->_callback_disconnect();
        }
    }

}

#if !defined(NO_GLOBAL_INSTANCES)
Ps4Controller Ps4;
#endif
