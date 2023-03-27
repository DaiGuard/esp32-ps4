#include <Ps4Controller.h>

void notify()
{
    Serial.print(Ps4.data.sensor.accelerometer.x);
    Serial.print(" ");
    Serial.print(Ps4.data.sensor.accelerometer.y);
    Serial.print(" ");
    Serial.print(Ps4.data.sensor.accelerometer.z);

    /* Uncomment the following if you also want
       to display the gryoscope data: */
    // Serial.print(" ");
    // Serial.print(Ps4.data.sensor.gyroscope.z);

    Serial.println();
}


void setup()
{
    Serial.begin(115200);
    Ps4.attach(notify);
    Ps4.begin("01:02:03:04:05:06");
}


void loop()
{

}
