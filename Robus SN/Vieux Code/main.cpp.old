#include <Arduino.h>
#include <librobus.h>
#include <Adafruit_TCS34725.h>

/*
void setup() {
  // put your setup code here, to run once:
  BoardInit();
  Serial.begin(9600);
}
*/

Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
int longif(int moyennea, int moyenneb, int moyennec);

  void couleur(int nbfois)
  {
    uint16_t clear, red, green, blue;
    int sommer = 0;
    int sommeb = 0;
    int sommeg = 0;
    int moyenner = 0;
    int moyenneg = 0;
    int moyenneb = 0;
      for(int i=0;i<nbfois;i++)
      {
        tcs.setInterrupt(false);
        delay(60);
        tcs.getRawData(&red, &green, &blue, &clear);
        tcs.setInterrupt(true);
        sommer+=red;
        sommeg+=green;
        sommeb+=blue;
      }
      moyenner=sommer/nbfois;
      moyenneg=sommeg/nbfois;
      moyenneb=sommeb/nbfois;
      Serial.println(moyenner);
      Serial.print("\n");
      Serial.println(moyenneg);
      Serial.print("\n");
      Serial.println(moyenneb);
      Serial.print("\n");
      longif(moyenner, moyenneg, moyenneb);
      Serial.println(longif(moyenner, moyenneg, moyenneb));
  }

  int longif(int moyennea, int moyenneb, int moyennec)
  {
    if(moyennea<=155 && moyennea>=145 && moyenneb<=179 && moyenneb>=169 && moyennec<=135 && moyennec>=125)
    {
      return 1;
    }
    else if(moyennea<=123 && moyennea>=113 && moyenneb<=163 && moyenneb>=153 && moyennec<=135 && moyennec>=128)
    {
      return 2;
    }
    else if(moyennea<=350 && moyennea>=225 && moyenneb<=375 && moyenneb>=300 && moyennec<=325 && moyennec>=251)
    {
      return 3;
    }
    else 
    {
      return 0;
    }
  }
/*
void loop() {
  couleur(3);
  delay(5000);
  // put your main code here, to run repeatedly:
}
*/