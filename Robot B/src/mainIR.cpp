#include <Arduino.h>
#include <librobus.h>


uint16_t input;
bool quille = true;
bool parchoc = false;
bool faitVirage = false;

uint32_t soundlenght = 0 ;
int sound_detection1()
{
  if(analogRead(A0) > 520)
  {
    soundlenght += 25;
  }
  else
  {
    soundlenght = 0;
  }
  delay(25);
  if(soundlenght > 1200)
  {
    Serial.println("son detecte");
    return 0;
  }
  return 1;
}

void setup()
{
  BoardInit();
}

float Angle(float theta)
{
  // theta (entier): angle en degré à laquelle le robot doit virer

  float nbLecture = 0;
  nbLecture = (7979.002624 * (theta / 360));
  //Serial.println(nbLecture);
  return nbLecture;
}
// appeler virage pour tourner sur soi-même selon l'angle "angle". 0 = virage à gauche. 1 = virage à droite
void virage(int sens, float angle)
{
  // sens (entier): 0 pour virer à gauche et 1 pour virer à droite
  // angle (entier): angle du virage en degré
  float lecture = (float)ENCODER_Read(0);
  while ((float)ENCODER_Read(0) <= (lecture + Angle(angle)) && (float)ENCODER_Read(0) >= ((lecture - Angle(angle))))
  {
    // Serial.println(virageSelonAngle(angle));
    if (sens == 1)
    {
      MOTOR_SetSpeed(0, 0.20);
      MOTOR_SetSpeed(1, -0.20);
    }
    else if (sens == 0)
    {
      MOTOR_SetSpeed(0, -0.20);
      MOTOR_SetSpeed(1, 0.20);
    }
  }
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
  ENCODER_Reset(1);
  ENCODER_Reset(0);
  delay(100);
}
// appeler lignerDroite pour avancer selon la distance en mètre "distance".

void ligneDroite(float distance)
{
  //Pour aller en ligne droite sur une distance déterminée en mètre
  float borne = 0;
  borne = distance * 13367;
  float kp = 0.0004;
  float ki = 0.0001;
  float errorp = 0;
  float errori = 0;
  
  float speedright = 0.35;
  float speedleft = 0.35;
  float adjust = 0;
  
  float distancep = 0;
  int nbcycle = 0;
  

  MOTOR_SetSpeed(0, 0.15);
  MOTOR_SetSpeed(1, 0.15);
  delay(50);
  MOTOR_SetSpeed(0, 0.25);
  MOTOR_SetSpeed(1, 0.25);
  delay(50);
  MOTOR_SetSpeed(0, 0.35);
  MOTOR_SetSpeed(1, 0.35);
  delay(50);
  MOTOR_SetSpeed(1, speedright);
  MOTOR_SetSpeed(0, speedleft);

  while (distancep < borne)
  {
    nbcycle++;
    delay(100);
    input = ROBUS_ReadIR(3);
    Serial.println(input);

    float valeur_voulue = ENCODER_ReadReset(0);
    float valeur_reel = ENCODER_ReadReset(1);
    distancep += valeur_reel;
    errorp = valeur_voulue - valeur_reel;
    errori = ((nbcycle * valeur_voulue) - distancep);
    adjust = (errorp * kp) + (errori * ki);
    MOTOR_SetSpeed(1, (speedright + adjust));
    
    /*parchoc = ROBUS_IsBumper(2);
    Serial.println(parchoc);

    if (parchoc == true)
    {
      MOTOR_SetSpeed(0, 0.35);
      MOTOR_SetSpeed(1, 0.35);
      delay(50);
      MOTOR_SetSpeed(0, 0.25);
      MOTOR_SetSpeed(1, 0.25);
      delay(50);
      MOTOR_SetSpeed(0, 0.15);
      MOTOR_SetSpeed(1, 0.15);
      delay(50);
      MOTOR_SetSpeed(0, 0);
      MOTOR_SetSpeed(1, 0);
      delay(2000);
      MOTOR_SetSpeed(0, speedleft);
      MOTOR_SetSpeed(1, speedright);
      delay(50);
    }
    */
   
    Serial.println(input);

    if(input > 90 && quille == true)
     {   
       
      MOTOR_SetSpeed(0, 0.35);
      MOTOR_SetSpeed(1, 0.35);
      delay(20);
      MOTOR_SetSpeed(0, 0.15);
      MOTOR_SetSpeed(1, 0.15);
      delay(20);
      MOTOR_SetSpeed(0, 0);
      MOTOR_SetSpeed(1, 0);
      delay(50);

 if(distancep/13367 > 2 && distancep/13367 < 3)
      {
        delay(2000);    
      }

      virage(0,90);
      delay(500);

      quille = false;
      faitVirage = true;

      ligneDroite(0.55);
      delay(500);  

      virage(1,85);
      delay(500);

      MOTOR_SetSpeed(0, 0.15);
      MOTOR_SetSpeed(1, 0.15);
      delay(50);
      MOTOR_SetSpeed(0, 0.25);
      MOTOR_SetSpeed(1, 0.25);
      delay(50);
      MOTOR_SetSpeed(0, 0.35);
      MOTOR_SetSpeed(1, 0.35);

      ligneDroite(4.5 - (distancep/13367));
      delay(20000);
     }

  }
    MOTOR_SetSpeed(0, 0.35);
    MOTOR_SetSpeed(1, 0.35);
    delay(50);
    MOTOR_SetSpeed(0, 0.25);
    MOTOR_SetSpeed(1, 0.25);
    delay(50);
    MOTOR_SetSpeed(0, 0.15);
    MOTOR_SetSpeed(1, 0.15);
    delay(50);
    MOTOR_SetSpeed(0, 0);
    MOTOR_SetSpeed(1, 0);
    delay(50);
    ENCODER_Reset(1);
    ENCODER_Reset(0);

}

void loop()
{
  
  if(sound_detection1() == 0)
  {
   ligneDroite(4.5);

    if(faitVirage == false)
    {
        virage(0,93);
        delay(200);
        ligneDroite(0.55);
    }
    input = ROBUS_ReadIR(3);
    Serial.println(input);
    delay(100000);
  }
}