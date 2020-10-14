/*----------------------------------------------------------------------------------------------------------------------------//
//NOM DU PROGRAMME: Robus                                                                                                     //
//                                                                                                                            //
//NOM DU FICHIER: Main.cpp                                                                                                    //
//                                                                                                                            //
//PROGRAMMEURS: Anthony Royer, Vincent Dagenais et Bertrand Labrecque                                                         //
//                                                                                                                            //
//DESCRIPTION: Fichier pour le déplacement du robot ROBUS pour la compétition                                                 //
//                                                                                                                            //
//----------------------------------------------------------------------------------------------------------------------------*/

//---------------------------------------------Inclusions des librairies requises---------------------------------------------//
#include <Arduino.h>
#include <librobus.h>
#include <Valeurs.h>

//--------------------------------------------------------Constantes----------------------------------------------------------//
#define nb_mvmt 23
#define kp kpb
#define kd 0.00002
#define ki kib

//--------------------------------------------Initialisation de variables globales--------------------------------------------//
int32_t ReadEncodeur2 = 0;
int32_t ReadEncodeur1 = 0;
int32_t i = 0;    //valeur pour la boucle du main
float rotationspeed = 0;
float ratio = 0;
float error_matrix[3][2] = {
  {MOTOR1ID,MOTOR2ID},
  {0,0},
  {0.0001,0}
};
float mvmt_matrix[3][nb_mvmt] = {
  {direction,curve,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction,angle,direction},
  {104.5,150,62,deg90,194.5,-deg45,97,-deg90,90,deg90,122.5,deg180,122.5,-deg90,90,deg90,97,deg45,194.5,-deg90,62,deg45,104.5},
  {0,-30,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
//Matrice utilisée pour stocker le prochain mouvement (en ticks de rotation)
float traveldistance[2][2] = {
  {MOTOR1ID,MOTOR2ID},
  {0,0}};

//=========================================================Fonctions==========================================================//

//---Fonction pour calculer le nombre de ticks à atteindre par moteur selon le type de déplacement----------------------------//
  void calculatetravel(int traveltype, float travelvalue, float travelDifferentiel){
  traveldistance [1] [MOTOR2ID] = {0};
  traveldistance [1] [MOTOR1ID] = {0};
  float rayon = 0;
  float longeurT = 0;
  float angleMilieu = 0;
  /*Serial.println("Calculate Travel where traveltype is :");
  Serial.print(traveltype);
  Serial.println("Travel Value :");
  Serial.print(traveltype);*/
  switch (traveltype){
      case direction:
          // Si le type de valeur est une distance
          traveldistance [1] [MOTOR2ID] = calculatewheelticks(travelvalue);
          traveldistance [1] [MOTOR1ID] = traveldistance [1] [MOTOR2ID];
          break;
      case angle:
          // Si la type de valeur est un angle
          traveldistance [1] [MOTOR2ID] = calculatewheelticks((float)((travelvalue/360)*CIRCUMFERENCEWHEELZ));
          traveldistance [1] [MOTOR1ID] = 0 - traveldistance [1] [MOTOR2ID];
          break;
      case curve:
          //Si le type de valeur est une courbe
          rayon = (((travelvalue / 2) * (travelvalue / 2)) / absoluteValue(travelDifferentiel));
          longeurT = rayon - absoluteValue(travelDifferentiel);
          angleMilieu = ((acos((longeurT / rayon))*2)*180)/PI;
          if (travelDifferentiel > 0)
          {
            traveldistance[1][MOTOR2ID] = calculatewheelticks((float)((angleMilieu * (((rayon + RADIUSWHEELZ) * 2) * PI)) / 360));
            traveldistance[1][MOTOR1ID] = calculatewheelticks((float)((angleMilieu * (((rayon - RADIUSWHEELZ) * 2) * PI)) / 360));
            /*Serial.println("I'M IN");
            Serial.println(traveldistance[1][MOTOR2ID]);
            Serial.println(traveldistance[1][MOTOR1ID]);
            */
          }
          else
          {
            traveldistance[1][MOTOR2ID] = calculatewheelticks((float)((angleMilieu * (((rayon - RADIUSWHEELZ) * 2) * PI)) / 360));
            traveldistance[1][MOTOR1ID] = calculatewheelticks((float)((angleMilieu * (((rayon + RADIUSWHEELZ) * 2) * PI)) / 360));
          }
        break;

      default:
        //Serial.println("Calculate Travel case default");
          break;
    }
    ratio = calculateRatio(traveldistance [1] [MOTOR2ID],traveldistance [1] [MOTOR1ID]);
  }

//------Fonction pour le PID--------------------------------------------------------------------------------------------------//
  float pid(float objective,int32_t value, float ratioDist, int16_t MOTORID){
    float pidValue = 0;
    float error = (objective-(value*ratioDist));
    /*Serial.print("Motor : ");
    Serial.println(MOTORID);
    Serial.print("VALEUR : ");
    Serial.println(readEncodeurLive);
    Serial.print("Erreur : ");
    Serial.println(error);
    Serial.print("ratio : ");
    Serial.println(ratio);
    Serial.print("last error :");
    Serial.println(error_matrix[last_error][MOTORID]);
    */
    float integral = error_matrix[previous_integral][MOTORID] + error;
    float derivative = (error - error_matrix[last_error][MOTORID])/deltaT;
    pidValue = (kp*error + kd*derivative + ki*integral);
    error_matrix[last_error][MOTORID] = error;
    error_matrix[previous_integral][MOTORID] = integral;
    //Serial.print("PID : ");Serial.println(pidValue);
    return pidValue;
    //moyenne des deux distances multipliées par leur ratio
  }

//------Fonction pour faire bouger le robot selon le type de mouvement jusqua sa destination----------------------------------//
  void move(int pos_mvmt_matrix){
    bool destinationreached = false;
    /*Serial.println("inside move with position :");
    Serial.println(pos_mvmt_matrix);
    Serial.println("matrix value at this position :");
    Serial.println(mvmt_matrix[0][pos_mvmt_matrix]);
    */
    calculatetravel((int)mvmt_matrix[TypeTravel][pos_mvmt_matrix], mvmt_matrix[ValueTravel][pos_mvmt_matrix], mvmt_matrix[AngleDifferencial][pos_mvmt_matrix]);
  //--Boucle for qui sert a faire avancer les moteurs jusqu'a temps que la destination voulue sera atteinte
    ENCODER_Reset(MOTOR2ID);
    ENCODER_Reset(MOTOR1ID);
    do
    {
      ReadEncodeur2 = ENCODER_Read(MOTOR2ID);
      ReadEncodeur1 = ENCODER_Read(MOTOR1ID);
      switch ((int)mvmt_matrix[0][pos_mvmt_matrix])
      {
      //Déplacement de type linéaire (deux roues auront des valeur d'encodeurs croissantes)
      case direction:
        if ((ReadEncodeur2 <= traveldistance [1] [MOTOR2ID]) and (ReadEncodeur1 <= traveldistance [1] [MOTOR1ID])) 
        {
          /*Serial.print("Pid 1 :");
          Serial.println(pid1);
          Serial.print("Pid 2:");
          Serial.println(pid2);
          */
          MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput);
          MOTOR_SetSpeed(MOTOR1ID, (MotorSpeedInput + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))*ratio);
        } else {
          //MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput/2);
          //MOTOR_SetSpeed(MOTOR1ID, MotorSpeedInput/2);
          destinationreached = true;
        } 
        break;
      //Déplacement de type angle (une roue va avoir une décroissance au niveau de l'encodeur)
      case angle:
         if ((traveldistance [1] [MOTOR2ID] > 0) and((ReadEncodeur2 < traveldistance [1] [MOTOR2ID]) or (ReadEncodeur1 > traveldistance [1] [MOTOR1ID])))    //REMPLACER PAR PID EVENTUELLEMENT
          {
            rotationspeed = MotorSpeedInputRotation;
            //Si il s'agit d'un angle positif (a rotation horaire)
            MOTOR_SetSpeed(MOTOR2ID, rotationspeed);
            MOTOR_SetSpeed(MOTOR1ID, (rotationspeed + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))/ratio);
          } else if((traveldistance [1] [MOTOR2ID] < 0) and((ReadEncodeur2 > traveldistance [1] [MOTOR2ID]) or (ReadEncodeur1 < traveldistance [1] [MOTOR1ID]))){
            //Si il s'agit d'un angle négatif (a rotation anti-horaire)
            rotationspeed = 0 - MotorSpeedInputRotation;
            MOTOR_SetSpeed(MOTOR2ID, rotationspeed);
            MOTOR_SetSpeed(MOTOR1ID,  (rotationspeed + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))/ratio);
          }else{
            //Destination atteinte, alors arrêter les moteurs
            //MOTOR_SetSpeed(MOTOR2ID, rotationspeed/2);
            //MOTOR_SetSpeed(MOTOR1ID, (rotationspeed*ratio)/2);
            destinationreached = true;
          }
        break;
        //Déplacement de type courbe (les deux roues irons dans la même direction mais pas au même rytme)
        case curve:
        if ((ReadEncodeur2 <= traveldistance [1] [MOTOR2ID]) or (ReadEncodeur1 <= traveldistance [1] [MOTOR1ID])) 
        {
          MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput);
          MOTOR_SetSpeed(MOTOR1ID, (MotorSpeedInput*ratio + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))*ratio);
        } else {
          destinationreached = true;
        }
        break;
      default:
        break;
      }
      delay(deltaT);
    } while (destinationreached == false);
    //Puisque la destination a été atteinte, reset la valeur des encodeurs pour le prochain déplacement
    MOTOR_SetSpeed(MOTOR2ID, 0);
    MOTOR_SetSpeed(MOTOR1ID, 0);
    ENCODER_Reset(MOTOR2ID);
    ENCODER_Reset(MOTOR1ID);
    error_matrix[last_error][MOTOR2ID] = 0;
    error_matrix[last_error][MOTOR1ID] = 0.0001;
    error_matrix[previous_integral][MOTOR2ID] = 0;
    error_matrix[previous_integral][MOTOR1ID] = 0;
    destinationreached = NULL;
    delay(500);
  }

//-----------------------------------------------------Setup Robot------------------------------------------------------------//
  void setup(){
    BoardInit();
    delay(1000);
  }

//----------------------------------------------------------Main--------------------------------------------------------------//
// put your main code here, to run repeatedly:
  void loop() {
  //Pour chaque mouvement dans ma matrice de mouvement ou i est la position dans ma matrice du mouvement actuel
  if (i < nb_mvmt){
    //Serial.println(i);
    move(i);
  }else if (i >= nb_mvmt)
  {
    i = nb_mvmt + 1;
  }
  //Augmenter i pour la prochaine passe afin de faire le prochain mouvement
  i = i + 1;  
}