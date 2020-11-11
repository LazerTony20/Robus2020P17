#include <Arduino.h>
#include <LibRobus.h>

//=======================================================DÉCLARATIONS========================================================//
void activerServo(uint8_t id_Servo,uint8_t angle_Servo);

//=========================================================FONCTIONS=========================================================//
void activerServo(uint8_t id_Servo,uint8_t angle_Servo){
    SERVO_Enable(id_Servo);
    SERVO_SetAngle(id_Servo, angle_Servo);
    SERVO_Disable(id_Servo);
}