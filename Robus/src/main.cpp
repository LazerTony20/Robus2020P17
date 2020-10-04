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
const double DIAMETERWHEELZ = 19;       //Distance entre mes deux roues (MESURÉE AVEC UNE RÈGLE)
const double CIRCUMFERENCEWHEELZ = DIAMETERWHEELZ*PI;
const uint8_t MOTOR2ID = 0;
const uint8_t MOTOR1ID = 1;
const int nb_mvmt = 6;
//----------------------------------------------------------------------------------------------------------------------------//

//--------------------------------------------Initialisation de variables globales--------------------------------------------//
//Variables Globales
float MotorSpeedInput = 0.5;
int32_t ReadEncodeur2 = 0;
int32_t ReadEncodeur1 = 0;
int i = 0; //valeur pour la boucle du main
//0 = valeur de distance (en cm) et 1 valeur de rotation ( en degrées par rapport au centre avant du robot) et 2 une courbe (premiere valeur = distance et 2e Tangeante a distance)
double mvmt_matrix[3][nb_mvmt] = {
  {0,1,0,1,0,2},
  {50,45,25,-45,70,90},
  {0,0,0,0,0,30}
};
//Matrice utilisée pour stocker le prochain mouvement (en ticks de rotation)
double traveldistance[2][2] = {
  {MOTOR1ID,MOTOR2ID},
  {0,0}};
//----------------------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------Fonctions----------------------------------------------------------//

//---Fonction pour calculer le nombre de ticks à faire pour que la roue fasse la distance désirée
  double calculatewheelticks(double distance){
    //Serial.println("Calculatewheelticks");
    return ((distance*WHEELTICKS)/(WHEELCIRCUMFERENCE));
  }

//---Fonction pour calculer le nombre de ticks à atteindre par moteur selon le type de déplacement
  void calculatetravel(int traveltype, double travelvalue){
  traveldistance [1] [0] = {0};
  traveldistance [1] [1] = {0};
  //Serial.println("Calculate Travel where traveltype is :");
  //Serial.print(traveltype);
  switch (traveltype){
      case 0:
          // Si le type de valeur est une distance
          //Serial.println("Calculate Travel case 0");
          traveldistance [1] [0] = calculatewheelticks(travelvalue);
          traveldistance [1] [1] = traveldistance [1] [0];
          break;
      case 1:
          // Si la type de valeur est un angle
          //Serial.println("Calculate Travel case 1");
          traveldistance [1] [0] = calculatewheelticks((double)((travelvalue/360)*CIRCUMFERENCEWHEELZ));
          traveldistance [1] [1] = 0 - traveldistance [1] [0];
          break;
      case 2:
          //Si le type de valeur est une courbe

        break;
      default:
        //Serial.println("Calculate Travel case default");
          break;
    }

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
      switch ((int)mvmt_matrix[0][pos_mvmt_matrix])
      {
      //Déplacement de type linéaire (deux roues auront des valeur d'encodeurs croissantes)
      case 0:
        if ((ReadEncodeur2 <= traveldistance [1] [0]) or (ReadEncodeur1 <= traveldistance [1] [1]))
        {
          MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput);
          MOTOR_SetSpeed(MOTOR1ID, MotorSpeedInput);
        } else {
          MOTOR_SetSpeed(MOTOR2ID, 0);
          MOTOR_SetSpeed(MOTOR1ID, 0);
          destinationreached = true;
        } 
        break;
      //Déplacement de type angle (une roue va avoir une décroissance au niveau de l'encodeur)
      case 1:
        if ((traveldistance [1] [0] > 0) and((ReadEncodeur2 < traveldistance [1] [0]) or (ReadEncodeur1 > traveldistance [1] [1])))
          {
            //Si il s'agit d'un angle positif (a rotation horaire)
            MOTOR_SetSpeed(MOTOR2ID, MotorSpeedInput);
            MOTOR_SetSpeed(MOTOR1ID, 0 - MotorSpeedInput);
          } else if((traveldistance [1] [0] < 0) and((ReadEncodeur2 > traveldistance [1] [0]) or (ReadEncodeur1 < traveldistance [1] [1]))){
            //Si il s'agit d'un angle négatif (a rotation anti-horaire)
            MOTOR_SetSpeed(MOTOR2ID, 0 - MotorSpeedInput);
            MOTOR_SetSpeed(MOTOR1ID, MotorSpeedInput);
          }else{
            //Destination atteinte, alors arrêter les moteurs
            MOTOR_SetSpeed(MOTOR2ID, 0);
            MOTOR_SetSpeed(MOTOR1ID, 0);
            destinationreached = true;
          }
        break;
        //Déplacement de type courbe (les deux roues irons dans la même direction mais pas au même rytme)
        case 2:

        break;
      default:
        break;
      }
    } while (destinationreached == false);
    //Puisque la destination a été atteinte, reset la valeur des encodeurs pour le prochain déplacement
    ENCODER_Reset(MOTOR2ID);
    ENCODER_Reset(MOTOR1ID);
  }
//----------------------------------------------------------------------------------------------------------------------------//


//-----------------------------------------------------Setup Robot------------------------------------------------------------//


  void setup(){
    BoardInit();
    Serial.begin(9600);
    delay(1500);
    
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