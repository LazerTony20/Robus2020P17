#include <Arduino.h>
#include <LibRobus.h>
//------------------------------------------------Initialisation de constantes------------------------------------------------//
#define WHEELDIAMETER 7.62      //diametre des roues en centimetres
#define WHEELRADIUS WHEELDIAMETER/2
#define WHEELTICKS 3200
#define DIAMETERWHEELZ 19       //Distance entre mes deux roues (MESURÉE AVEC UNE RÈGLE) 19
#define RADIUSWHEELZ DIAMETERWHEELZ/2
#define MotorSpeedInput 0.5
#define MotorSpeedInputRotation 0.25
#define WHEELCIRCUMFERENCE WHEELDIAMETER*PI
#define CIRCUMFERENCEWHEELZ DIAMETERWHEELZ*PI
#define MOTOR2ID LEFT
#define MOTOR1ID RIGHT
#define TypeTravel 0
#define ValueTravel 1
#define AngleDifferencial 2
#define last_error 2            //Position de la derniere erreur dans ma matrice d'erreur
#define previous_integral 1     //Position de ma somme dans ma matrice d'erreur
#define kpa 0.0002
#define kia 0.00002
#define kpb 0.0005
#define kib 0.00001              //MODIFIER PLUS GROS
#define deltaT 25                //en milisecondes
#define direction 0              //Si il s'agit d'un mouvement de type linéaire
#define angle 1                  //Si il s'agit d'un mouvement de type rotation
#define curve 2                  //Si il s'agit d'un mouvement de type courbe 
#define deg45 45
#define deg90 90
#define deg180 180
//----------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------Fonctions----------------------------------------------------------//
//---Fonction pour calculer le nombre de ticks à faire pour que la roue fasse la distance désirée
  float absoluteValue(float value){
    if(value < 0){
      return value*-1;
    }else{
      return value;
    }
  }

  float calculateRatio(float value1, float value2){
    return value1/value2;
  }

  float calculatewheelticks(float distance){
    //Serial.println("Calculatewheelticks");
    return ((distance*WHEELTICKS)/(WHEELCIRCUMFERENCE));
  }