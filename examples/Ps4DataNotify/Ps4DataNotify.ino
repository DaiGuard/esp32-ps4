#include <Ps4Controller.h>

void notify()
{
    if( Ps4.data.button.cross ){
        Serial.println("Pressing the cross button");
    }

    if( Ps4.data.button.square ){
        Serial.println("Pressing the square button");
    }

    if( Ps4.data.button.triangle ){
        Serial.println("Pressing the triangle button");
    }

    if( Ps4.data.button.circle ){
        Serial.println("Pressing the circle button");
    }
}

void onConnect(){
  Serial.println("Connected!.");
}

void setup()
{
    Serial.begin(115200);
    Ps4.attach(notify);
    Ps4.attachOnConnect(onConnect);
    Ps4.begin("01:02:03:04:05:06");
    Serial.println("Ready.");
}

void loop()
{
}
