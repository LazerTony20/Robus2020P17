#include <Arduino.h>
#include <LibRobus.h>
#include<Adafruit_TCS34725.h>

//=======================================================DÉCLARATIONS========================================================//
//void activerServo(uint8_t id_Servo,uint8_t angle_Servo);

//=========================================================FONCTIONS=========================================================//

void activerServo(int id_Servo,int angle_Servo){
    SERVO_Enable(id_Servo);
    SERVO_SetAngle(id_Servo, angle_Servo);
    delay(500);
    SERVO_Disable(id_Servo);
    delay(500);
}
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);
int longif(int moyennea, int moyenneb, int moyennec);

  int couleur(int nbfois)
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
      return longif(moyenner, moyenneg, moyenneb);
  }

int longif(int moyennea, int moyenneb, int moyennec)
  {
    if(moyennea<=320 && moyennea>=300 && moyenneb<=290 && moyenneb>=275) //&& moyennec<=260 && moyennec>=245)
    {
      pinMode(11, OUTPUT);
      digitalWrite(11, HIGH);
      digitalWrite(12, LOW);
      digitalWrite(13, LOW);
      return 1;
    }
    else if(moyennea<=335 && moyennea>=320 && moyenneb<=350 && moyenneb>=311) //&& moyennec<=290 && moyennec>=280)
    {
      pinMode(12, OUTPUT);
      digitalWrite(12, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(13, LOW);
      //analogWrite(A4, LOW);
      return 2;
    }
    else if(moyennea<=299 && moyennea>=0 && moyenneb<=310 && moyenneb>=291) //&& moyennec<=255 && moyennec>=245)
    {
      pinMode(13, OUTPUT);
      digitalWrite(13, HIGH);
      digitalWrite(11, LOW);
      digitalWrite(12, LOW);
      //analogWrite(A5, LOW);
      return 3;
    }
    else 
    {
      return 0;
    }
  }