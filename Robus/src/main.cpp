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
//----------------------------------------------------------------------------------------------------------------------------//

//------------------------------------------------Initialisation de constantes------------------------------------------------//
const double WHEELDIAMETER = 7.62;      //diametre en centimetres
const double WHEELCIRCUMFERENCE = WHEELDIAMETER*PI;
const int WHEELTICKS = 3200;
const double DIAMETERWHEELZ = 19;       //Distance entre mes deux roues (MESURÉE AVEC UNE RÈGLE) 19
const double CIRCUMFERENCEWHEELZ = DIAMETERWHEELZ*PI;
const uint8_t MOTOR2ID = 0;
const uint8_t MOTOR1ID = 1;
const int nb_mvmt = 11;
const double kpa = 0.000125;
const double kda = 0.0000001;
const double kia = 0.000001;
const double kpb = 0.0002;
const double kdb = 0.0000001;
const double kib = 0.00000125;      //MODIFIER PLUS GROS
const int deltaT = 25; //en milisecondes
//----------------------------------------------------------------------------------------------------------------------------//

//--------------------------------------------Initialisation de variables globales--------------------------------------------//
//Variables Globales
float MotorSpeedInput = 0.5;
float MotorSpeedInputRotation = 0.25;
int32_t ReadEncodeur2 = 0;
int32_t ReadEncodeur1 = 0;
int i = 0; //valeur pour la boucle du main
double kp = kpb;
double kd = kdb;
double ki = kib;
double ratio = 1;
int last_error = 2;
int previous_integral = 0;
double error_matrix[3][2] = {
  {MOTOR1ID,MOTOR2ID},
  {0,0},
  {0,0}
};
//0 = valeur de distance (en cm) et 1 valeur de rotation ( en degrées par rapport au centre avant du robot) et 2 une courbe (premiere valeur = distance et 2e Tangeante a distance)
double mvmt_matrix[3][nb_mvmt] = {
  {0,1,0,1,0,1,0,1,0,1,0},
  {104.5,-40,58,82,194.5,-40,97,-82,90,82,122.5},
  {0,0,0,0,0,30,0,0,0,0,0}
};
//Matrice utilisée pour stocker le prochain mouvement (en ticks de rotation)
double traveldistance[2][2] = {
  {MOTOR1ID,MOTOR2ID},
  {0,0}};
//----------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------Fonctions----------------------------------------------------------//

//---Fonction pour calculer le nombre de ticks à faire pour que la roue fasse la distance désirée
/*  double absoluteValue(double value){
    if(value < 0){
      return value*-1;
    }else{
      return value;
    }
  }
*/
  double calculateRatio(double value1, double value2){
    return value1/value2;
  }

  double calculatewheelticks(double distance){
    //Serial.println("Calculatewheelticks");
    return ((distance*WHEELTICKS)/(WHEELCIRCUMFERENCE));
  }

//---Fonction pour calculer le nombre de ticks à atteindre par moteur selon le type de déplacement
  void calculatetravel(int traveltype, double travelvalue){
  traveldistance [1] [MOTOR2ID] = {0};
  traveldistance [1] [MOTOR1ID] = {0};
  //Serial.println("Calculate Travel where traveltype is :");
  //Serial.print(traveltype);
  switch (traveltype){
      case 0:
          // Si le type de valeur est une distance
          //Serial.println("Calculate Travel case 0");
          traveldistance [1] [MOTOR2ID] = calculatewheelticks(travelvalue);
          traveldistance [1] [MOTOR1ID] = traveldistance [1] [0];
          break;
      case 1:
          // Si la type de valeur est un angle
          //Serial.println("Calculate Travel case 1");
          traveldistance [1] [MOTOR2ID] = calculatewheelticks((double)((travelvalue/360)*CIRCUMFERENCEWHEELZ));
          traveldistance [1] [MOTOR1ID] = 0 - traveldistance [1] [0];
          break;
      case 2:
          //Si le type de valeur est une courbe

        break;
      default:
        //Serial.println("Calculate Travel case default");
          break;
    }
    ratio = calculateRatio(traveldistance [1] [MOTOR2ID],traveldistance [1] [MOTOR1ID]);
  }



//Fonction pour le PID
  double pid(double objective,int32_t readEncodeurLive, double ratioDist, int MOTORID){
    double pidValue = 0;
    double error = (objective-(readEncodeurLive*ratioDist));
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
    //double integral = error_matrix[previous_integral][MOTORID] + (error*deltaT);
    double integral = error_matrix[previous_integral][MOTORID] + error;
    //double derivative = (error - error_matrix[last_error][MOTORID])/deltaT;
    double derivative = 0;
    pidValue = (kp*error + kd*derivative + ki*integral);
    error_matrix[last_error][MOTORID] = error;
    error_matrix[previous_integral][MOTORID] = integral;
    //Serial.print("PID : ");
    //Serial.println(pidValue);
    return pidValue;
    //moyenne des deux distances multipliées par leur ratio
    
    
  }






//---Fonction pour faire bouger le robot selon le type de mouvement jusqua sa destination
  void move(int pos_mvmt_matrix){
    bool destinationreached = false;
    
    //Serial.println("inside move with position :");
    //Serial.println(pos_mvmt_matrix);
    //Serial.println("matrix value at this position :");
    //Serial.println(mvmt_matrix[0][pos_mvmt_matrix]);
    calculatetravel((int)mvmt_matrix[0][pos_mvmt_matrix], mvmt_matrix[1][pos_mvmt_matrix]);
  //--Boucle for qui sert a faire avancer les moteurs jusqu'a temps que la destination voulue sera atteinte
    do
    {
      ReadEncodeur2 = ENCODER_Read(MOTOR2ID);
      ReadEncodeur1 = ENCODER_Read(MOTOR1ID);
      double moyenneEncodeurs = (ReadEncodeur2+(ReadEncodeur1*ratio))/2;
      Serial.print("Moyenne : ");
      Serial.println(moyenneEncodeurs);
      switch ((int)mvmt_matrix[0][pos_mvmt_matrix])
      {
      //Déplacement de type linéaire (deux roues auront des valeur d'encodeurs croissantes)
      case 0:
        if ((ReadEncodeur2 <= traveldistance [1] [0]) and (ReadEncodeur1 <= traveldistance [1] [1])) 
        {
          
          double pid1 = pid(moyenneEncodeurs,ReadEncodeur2,1,MOTOR2ID);
          double pid2 = pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID);
          Serial.print("Pid 1 :");
          Serial.println(pid1);
          Serial.print("Pid 2:");
          Serial.println(pid2);
          MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput);
          MOTOR_SetSpeed(MOTOR1ID, (MotorSpeedInput + pid2)*ratio);
        } else {
          MOTOR_SetSpeed(MOTOR2ID, 0);
          MOTOR_SetSpeed(MOTOR1ID, 0);
          destinationreached = true;
        } 
        break;
      //Déplacement de type angle (une roue va avoir une décroissance au niveau de l'encodeur)
      case 1:
         if ((traveldistance [1] [0] > 0) and((ReadEncodeur2 < traveldistance [1] [0]) or (ReadEncodeur1 > traveldistance [1] [1])))    //REMPLACER PAR PID EVENTUELLEMENT
          {
            double rotationspeed = MotorSpeedInputRotation;
            //Si il s'agit d'un angle positif (a rotation horaire)
            
            //MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInputRotation + pid(moyenneEncodeurs,ReadEncodeur2,1,MOTOR2ID));
            MOTOR_SetSpeed(MOTOR2ID, rotationspeed);
            //MOTOR_SetSpeed(MOTOR1ID, 0- MotorSpeedInputRotation - ratio*pid(moyenneEncodeurs,ReadEncodeur1,ratio,MOTOR1ID));
            MOTOR_SetSpeed(MOTOR1ID, (rotationspeed + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))/ratio);
          } else if((traveldistance [1] [0] < 0) and((ReadEncodeur2 > traveldistance [1] [0]) or (ReadEncodeur1 < traveldistance [1] [1]))){
            //Si il s'agit d'un angle négatif (a rotation anti-horaire)
            double rotationspeed = 0 - MotorSpeedInputRotation;
            MOTOR_SetSpeed(MOTOR2ID, rotationspeed);
            //MOTOR_SetSpeed(MOTOR2ID, 0 - MotorSpeedInputRotation - pid(moyenneEncodeurs,ReadEncodeur2,1,MOTOR2ID));
            MOTOR_SetSpeed(MOTOR1ID,  (rotationspeed + pid(ReadEncodeur2,ReadEncodeur1,ratio,MOTOR1ID))/ratio);
            //MOTOR_SetSpeed(MOTOR1ID, MotorSpeedInputRotation + ratio*pid(moyenneEncodeurs,ReadEncodeur1,ratio,MOTOR1ID));
          }else{
            //Destination atteinte, alors arrêter les moteurs
            MOTOR_SetSpeed(MOTOR2ID, 0);
            MOTOR_SetSpeed(MOTOR1ID, 0);
            destinationreached = true;
            delay(100);
          }
        break;
        //Déplacement de type courbe (les deux roues irons dans la même direction mais pas au même rytme)
        case 2:

        break;
      default:
        break;
      }
      delay(deltaT);
    } while (destinationreached == false);
    //Puisque la destination a été atteinte, reset la valeur des encodeurs pour le prochain déplacement
    ENCODER_Reset(MOTOR2ID);
    ENCODER_Reset(MOTOR1ID);
    error_matrix[last_error][MOTOR2ID] = 0;
    error_matrix[last_error][MOTOR1ID] = 0;
    error_matrix[previous_integral][MOTOR2ID] = 0;
    error_matrix[previous_integral][MOTOR1ID] = 0;
  }

  
//----------------------------------------------------------------------------------------------------------------------------//


//-----------------------------------------------------Setup Robot------------------------------------------------------------//

  void setup(){
    BoardInit();
    delay(1000);
  }

//----------------------------------------------------------------------------------------------------------------------------//


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
//----------------------------------------------------------------------------------------------------------------------------//