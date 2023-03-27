#include <Ps4Controller.h>

void setup()
{
    Serial.begin(115200);
    Ps4.begin("01:02:03:04:05:06");
    Serial.println("Ready.");
}

void loop()
{
    if (Ps4.isConnected()){

        // Turn rumble on full intensity for 1 second
        Ps4.setRumble(100.0, 1000);
        delay(2000);

        // Turn rumble on full intensity indefinitely
        Ps4.setRumble(100.0);
        delay(2000);

        // Turn off rumble
        Ps4.setRumble(0.0);
    }

    delay(2000);
}
