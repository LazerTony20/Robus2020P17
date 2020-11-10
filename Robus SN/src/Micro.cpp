#include "Arduino.h"
#include "LibRobUS.h"


uint32_t soundLenght = 0;
void sound_detection() {
  if(analogRead(A0) > 520)
    {
      soundLenght += 25;
    }
  else
    {
      soundLenght = 0;
    }
  delay(25);
  if(soundLenght > 1200)
    {
      Serial.println("Son detecte");
    }
}