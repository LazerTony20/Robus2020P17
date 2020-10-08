#include <Arduino.h>
#include <LibRobus.h>
//------------------------------------------------Initialisation de constantes------------------------------------------------//
const double WHEELDIAMETER = 7.62;      //diametre en centimetres
const double WHEELCIRCUMFERENCE = WHEELDIAMETER*PI;
const int WHEELTICKS = 3200;
const double DIAMETERWHEELZ = 19;       //Distance entre mes deux roues (MESURÉE AVEC UNE RÈGLE) 19
const double CIRCUMFERENCEWHEELZ = DIAMETERWHEELZ*PI;
const uint8_t MOTOR2ID = LEFT;
const uint8_t MOTOR1ID = RIGHT;
const double kpa = 0.0002;
const double kia = 0.000002;
const double kpb = 0.0002;
const double kib = 0.00002;      //MODIFIER PLUS GROS
const int deltaT = 25; //en milisecondes
const int direction = 0;
const int angle = 1;
const int curve = 2;
const int deg45 = 40;
const int deg90 = 81;
const int deg180 = 175;
//----------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------Fonctions----------------------------------------------------------//
//---Fonction pour calculer le nombre de ticks à faire pour que la roue fasse la distance désirée
  double absoluteValue(double value){
    if(value < 0){
      return value*-1;
    }else{
      return value;
    }
  }

  double calculateRatio(double value1, double value2){
    return value1/value2;
  }

  double calculatewheelticks(double distance){
    //Serial.println("Calculatewheelticks");
    return ((distance*WHEELTICKS)/(WHEELCIRCUMFERENCE));
  }