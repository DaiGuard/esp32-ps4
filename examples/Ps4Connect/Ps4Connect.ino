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
    Serial.println("Connected!");
  }

  delay(3000);
}
