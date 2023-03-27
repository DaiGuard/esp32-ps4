#include <Ps4Controller.h>

void setup()
{
    Serial.begin(115200);
    Ps4.begin();

    String address = Ps4.getAddress();

    Serial.print("The ESP32's Bluetooth MAC address is: ");
    Serial.println(address);
}

void loop()
{

}
