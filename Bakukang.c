#define moveForward     0x0A                                                    //00001010
#define moveBackward    0x05                                                    //00000101
#define turnLeft        0x09                                                    //00001001
#define turnRight       0x06                                                    //00000110
#define openArm         0x20                                                    //00100000
#define closeArm        0x10                                                    //00010000
#define moveM1Forward   0x02                                                    //00000010
#define moveM2Forward   0x08                                                    //00001000
#define moveM1Backward  0x01                                                    //00000001
#define moveM2Backward  0x04                                                    //00000100
#define stop            0x00                                                    //00000000

//------Troubleshooting------
//PORTC PIN0 = Current phase of the machine
//PORTC PIN1 = Wheels not aligned: Rotary Sensor should be aligned to 00 or 11
//PORTC PIN2 = Color blue is detected
//PORTC PIN3 = Color red is detected

//------PIN CONFIGURATION------
//PORTB PIN0-PIN1 -> Motor1(Right Motor)
//PORTB PIN2-PIN3 -> Motor2(Left Motor)
//PORTB PIN4-PIN5 -> Arm Motor

//PORTD PIN0      -> Rotary Sensor1(Right Motor)
//PORTD PIN1      -> Rotary Sensor2(Left Motor)
//PORTD PIN2      -> Object Sensor
//PORTD PIN3      -> Color Sensor
//PORTD PIN4      -> Floor Sensor

unsigned char nextStep, colorDetected, rotary1;
unsigned char rotary2 = 0x03;

void func_moveForwardStraight() {
     // Set wheels to move forward
     if ((PORTD.F0 == 0x00 && PORTD.F1 == 0x00) == 1) {
        PORTB = moveForward;
        rotary1 = 0x00;
        rotary2 = 0x00;
     }

     else if ((PORTD.F0 == 0x01 && PORTD.F1 == 0x01) == 1) {
          PORTB = moveForward;
          rotary1 = 0x00;
          rotary2 = 0x01;
     }

     else {
          rotary1 = 0x01;
     }

     // If wheels are not aligned (i.e. operating at different speeds), then align them
     if (rotary1 == 0x01) {
        if (rotary2 == 0x00) {
           if (PORTD.F0 == 1) {                                                 //If nag-una ang motor1
              PORTB = moveM2Forward;
           }

           else if (PORTD.F1 == 1) {                                            //If nag-una ang motor2
                PORTB = moveM1Forward;
           }
        }

        else if (rotary2 == 0x01) {
           if (PORTD.F0 == 0) {                                                 //If nag-una ang motor1
              PORTB = moveM2Forward;
           }

           else if (PORTD.F1 == 0) {                                            //If nag-una ang motor2
                PORTB = moveM1Forward;
           }
        }

        else if (rotary2 == 0x03) {
             PORTC.F1 = 0x01;
        }
     }

     else {}
}

void func_moveBackwardStraight() {

}

// Roaming Phase
void func_Roaming() {

     func_moveForwardStraight();

     // When a box is detected
     if (PORTD.F2 == 1) {
        PORTB = stop;
        nextStep = 0x01;
        Delay_ms(1000);
     }
}

// Getting the box Phase
void func_getBox() {
     int i;
     
     // Estimate to "< 39" since there are no visual indicators in arena
     for (i = 0; i < 39; i++) {
         func_moveForwardStraight();
         Delay_ms(50);
     }
     
     PORTB = moveBackward;
     Delay_ms(75);
     PORTB = closeArm;
     
     nextStep = 0x02;
}

// Detect color of box Phase
void func_detectColor() {
     if (PORTD.F3 == 0) {
        PORTC.F2 = 0x01;
        colorDetected = 0x00;                                                   //ZERO IF BOX IS BLUE
     }
     
     else if (PORTD.F3 == 1) {                                                  //ONE IF BOX IS RED
          PORTC.F3 = 0x01;
          colorDetected = 0x01;
     }
     
     nextStep = 0x03;
}

// Going back to home base Phase
void func_goHome() {
     //TEMPORARY
     PORTB = moveBackward;
     Delay_ms(3000);
     
     nextStep = 0x04;
}

// Sorting the box Phase
void func_sortBox() {
     if (colorDetected == 0) {
        PORTB = turnRight;
        Delay_ms(1500);
        PORTB = moveForward;
        Delay_ms(2000);
        PORTB = moveBackward;
        Delay_ms(2000);
        PORTB = turnLeft;
        Delay_ms(1500);
        PORTB = stop;
     }
     
     else if (colorDetected == 1) {
        PORTB = turnLeft;
        Delay_ms(1500);
        PORTB = moveForward;
        Delay_ms(2000);
        PORTB = moveBackward;
        Delay_ms(2000);
        PORTB = turnRight;
        Delay_ms(1500);
        PORTB = stop;
     }

     nextStep = 0x00;
}

// Program starts here
void main() {
     ADCON1 = 0x06;
     CMCON = 0x07;
     
     TRISB = 0x00;                                                              //PORTB ALL OUTPUT
     TRISC = 0x00;
     TRISD = 0xFF;                                                              //PORTD ALL INPUT
     
     PORTB = openArm;
     PORTC = 0x00;
     PORTD = 0x00;
     
     Delay_ms(1000);

     while(1) {
              switch(nextStep) {
                           case 0x00: func_Roaming(); break;
                           case 0x01: func_getBox(); break;
                           case 0x02: func_detectColor(); break;
                           case 0x03: func_goHome(); break;
                           case 0x04: func_sortBox(); break;
                           default:   PORTC.F0 = 0x01; break;
              }

     }

}
