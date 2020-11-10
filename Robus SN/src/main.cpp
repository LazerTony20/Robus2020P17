#include <Arduino.h>
#include <LibRobus.h>
//0=moteur de gauche et 1=moteur de droite
void setup()
{
  // put your setup code here, to run once:
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
void tourner(int sens, float angle)
{
  float distance,distancelu = 0;
  if (sens == 0)
  {
    distance = (19*3.1416*(angle/360))*300;
   while(distance > distancelu)
    {
      delay(10);
      MOTOR_SetSpeed(1,0.25);
      distancelu = ENCODER_Read(1);
    }
  }
   if (sens == 1)
  {
    distance = (19*3.1416*(angle/360))*300;
   while(distance  > distancelu)
    {
      delay(10);
      MOTOR_SetSpeed(0,0.25);
      distancelu = ENCODER_Read(0);
    }
  }
 
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
  delay(100);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
}
void ligneDroite(float distance)
{
  //Pour aller en ligne droite sur une distance déterminée en mètre
  float borne = 0;
  borne = distance * 13367;
  float kp = 0.0005;
  float ki = 0.00003;
  float errorp = 0;
  float errori = 0;
  
  float speedright = 0.5;
  float speedleft = 0.5;
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
    delay(200);
    float valeur_voulue = ENCODER_ReadReset(0);
    float valeur_reel = ENCODER_ReadReset(1);
    distancep += valeur_reel;
    errorp = valeur_voulue - valeur_reel;
    errori = ((nbcycle * valeur_voulue) - distancep);
    adjust = (errorp * kp) + (errori * ki);
    MOTOR_SetSpeed(1, (speedright + adjust));
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
  delay(100);
  ENCODER_Reset(0);
  ENCODER_Reset(1);
}
// appeler whelle pour instaurer la dominance.
void whelle(int temps)
{
  //whellé d'un temps en millisecondes
  MOTOR_SetSpeed(0, -1);
  MOTOR_SetSpeed(1, -1);
  delay(444);
  MOTOR_SetSpeed(0, -0.75);
  MOTOR_SetSpeed(1, -0.75);
  delay(temps);
  //virage(1, 135);
  MOTOR_SetSpeed(0, 0);
  MOTOR_SetSpeed(1, 0);
}
// appeler courbe pour tourner selon un arc de cercle. distance à parcourir en degré "angle" d'un cercle de rayon "rayon" en cm.
// 0 = virage vers la gauche. 1 = virage vers la droite.
void courbe(float angle, float rayon, int sens)
{
  float kp = 0.0001;
  float ki = 0.00002;
  float errorp;
  float errori;
  float speedright = 0.5;
  float speedleft = 0.5;
  float adjust;
  float distance = 0;
  int nbcycle = 0;
  float distancegauche = 0;
  float distancedroite = 0;
  if (sens == 1)
  {
    distancegauche = (2 * 3.1416 * (rayon + 19) * (angle / 360));
    distancedroite = (2 * 3.1416 * (rayon) * (angle / 360));
  }
  if (sens == 0)
  {
    distancegauche = (2 * 3.1416 * (rayon) * (angle / 360));
    distancedroite = (2 * 3.1416 * (rayon + 19) * (angle / 360));
  }
  distance = distancedroite;
  while (distance > distancedroite)
  {
    MOTOR_SetSpeed(1, (distancedroite / distancegauche) * speedright);
    nbcycle++;
    MOTOR_SetSpeed(0, speedleft);
    delay(50);
    float valeur_voulue = ((distancedroite / distancegauche) * ENCODER_Read(0));
    float valeur_reel = ENCODER_Read(1);
    distance += valeur_reel;
    errorp = valeur_voulue - valeur_reel;
    errori = ((nbcycle * valeur_voulue) - distance);
    adjust = (errorp * kp) + (errori * ki);
    MOTOR_SetSpeed(1, (speedright + adjust));
    ENCODER_Reset(0);
    ENCODER_Reset(1);
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
}


void loop()
{
  //VARIABLE
  // put your main code here, to run repeatedly:
  delay(200);
  ligneDroite(0.31);
  tourner(1,80);
  ligneDroite(0.04);
  tourner(0,82);
  ligneDroite(0.235);
  delay(500); //lire couleur

  ligneDroite(1.16);
  //Ramasse balle
  delay(2500);
  

  //Si couleur detecter est jaune, faire ce trajet
  /*ligneDroite(0.81);
  tourner(1,80);
  ligneDroite(0.20);
  delay(200000);*/

  //Si couleur detecter est bleu
  /*ligneDroite(1.66);
  tourner(0,82);
  ligneDroite(0.08);
  delay(200000);*/

  //Si couleur detecter est rose
  ligneDroite(2.32);
  tourner(1,80);
  ligneDroite(0.17);
  delay(200000);

  delay(200000);
}