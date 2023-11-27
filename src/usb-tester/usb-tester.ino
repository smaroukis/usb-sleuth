// NB mega is 5V, stm32 is 3V3

#include <Arduino.h>

#define TEST_CONNECTIONS 0

// Set LED_BUILTIN if it is not defined by Arduino framework
#ifndef LED_BUILTIN
    #define LED_BUILTIN 13
#endif

// LED pins
#define GND_LED 22
#define VBUS_LED 24
#define CC_FLIP_A_LED 26
#define CC_FLIP_B_LED 28
#define CC_Rp_LED 30
#define CC_Rd_LED 32
#define Emarker_LED 34

// Inputs & Outputs
#define VBUS_SENSE 6 // RED ① -> A4 ② → 10K → GND
#define GND_SENSE 7 // A1
#define CC1_CTRL 9 // GRN -> R -> B_A5 (B_CC1)
#define CC2_CTRL 11 // BLUE -> R -> B_B4 (B_CC2)
const int B_CC1 = A0; // BLK ① -> [A0] Mega & ② -> B_A5 & ③ -> 10K
const int B_CC2 = A1; // WHT ① -> [A1] Mega ② -> B_B4 ③ -> 10K
#define A_CC1 3 // BLUE → A_A5 (CC1)
#define A_CC2 2 // ORG → A_B4 (CC2)

// flags
char gnd_conn = 0;
char vbus_conn = 0;
char cc1_conn = 0;
char cc1_conn_flip = 0;
char cc2_conn = 0;
char cc2_conn_flip = 0;
char Rp_conn = 0;
char Rd_conn = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println("Serial Started at 9600 bps");
  // initialize LED digital pin as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(GND_LED, OUTPUT);
  pinMode(VBUS_LED, OUTPUT);
  pinMode(CC_FLIP_A_LED, OUTPUT);
  pinMode(CC_FLIP_B_LED, OUTPUT);
  pinMode(CC_Rp_LED, OUTPUT);
  pinMode(CC_Rd_LED, OUTPUT);
  pinMode(Emarker_LED, OUTPUT);

  // initialize inputs
  pinMode(VBUS_SENSE, INPUT);
  pinMode(GND_SENSE, INPUT_PULLUP);
  pinMode(B_CC1, INPUT);
  pinMode(B_CC2, INPUT);

  // init. outputs
  pinMode(CC1_CTRL, OUTPUT);
  pinMode(CC2_CTRL, OUTPUT);
  pinMode(A_CC1, OUTPUT);
  pinMode(A_CC2, OUTPUT);

  // TODO setup outputs as initial HIGH/LOW
  digitalWrite(A_CC1, LOW);
  digitalWrite(A_CC2, LOW);
}

void loop()
{

// Test to see if the wiring is correct
  if (TEST_CONNECTIONS) {
    digitalWrite(LED_BUILTIN, HIGH);
    digitalWrite(GND_LED, HIGH);
    digitalWrite(VBUS_LED, HIGH);
    digitalWrite(CC_FLIP_A_LED, HIGH);
    digitalWrite(CC_FLIP_B_LED, HIGH);
    digitalWrite(CC_Rp_LED, HIGH);
    digitalWrite(CC_Rd_LED, HIGH);
    digitalWrite(Emarker_LED, HIGH);
    delay(1000);
    digitalWrite(LED_BUILTIN, LOW);
    digitalWrite(GND_LED, LOW);
    digitalWrite(VBUS_LED, LOW);
    digitalWrite(CC_FLIP_A_LED, LOW);
    digitalWrite(CC_FLIP_B_LED, LOW);
    digitalWrite(CC_Rp_LED, LOW);
    digitalWrite(CC_Rd_LED, LOW);
    digitalWrite(Emarker_LED, LOW);
    delay(1000);
    return; // start loop again
  }


// 1 - Test GND 
// B side is -> GND, A side is sense side into MCU (internal pullup)
if (digitalRead(GND_SENSE) == LOW){
  gnd_conn = 1;   
}
else {
  gnd_conn = 0;
}

// 2 - Test Vbus
// B_VBUS is pulled up to Vdd via 2K; A_VBUS is pulled down via 10K; Vsense = 10K/12K = HIGH
if (digitalRead(VBUS_SENSE) == HIGH) {
  vbus_conn = 1;
}
else {
  vbus_conn = 0;
}


// --------- 3 - Test for Usb-C<>Usb-C
// TODO - actually for possibilities A->B, Af->B, Af->Bf, A->Bf
// Check A_CC1 -> B_CC1
digitalWrite(CC1_CTRL, LOW); // through pull down
digitalWrite(CC2_CTRL, LOW); // through pull down
digitalWrite(A_CC1, HIGH);
delay(10);
if (digitalRead(B_CC1) == HIGH) {
  // successful pin continuity check
  cc1_conn = 1;
  }
else cc1_conn = 0; // pulled down via CC1_CTRL, not connected

// Check Flipped (A_CC1 -> B_CC2)
if (digitalRead(B_CC2) == HIGH) {
  // then A_CC1 has pulled B_CC2 high -> B side is flipped
  cc1_conn_flip = 1;
  }
else cc1_conn_flip = 0;

digitalWrite(A_CC2, HIGH);
delay(10);
if (digitalRead(B_CC2) == HIGH) cc2_conn = 1;
else cc2_conn = 0;
if (digitalRead(B_CC1) == HIGH) cc2_conn_flip = 1;
else cc2_conn_flip = 0;
//  -------- End testing C<>C and CC config

// TODO - test active / emarker

// -------- 4 Test USB-A for Pullup Rp
// take CC1_CTRL and CC2_CTRL → GND, if analog Read of B_CC1/2 is ~ 1/7 V_CTRL, Rp exists
digitalWrite(CC1_CTRL, LOW);
digitalWrite(CC2_CTRL, LOW);
int read_cc1 = analogRead(B_CC1);
int read_cc2 = analogRead(B_CC2);
Serial.print("B_CC1 voltage: "); 
Serial.println(read_cc1);
Serial.print("B_CC2 voltage: "); 
Serial.println(read_cc2);
Serial.println();
// if cc1 or cc2 != we have a Rp to Vbus on the B_CC line
// TODO - handle any noise or rise above ground
if ( (read_cc1 > 5) || (read_cc2) > 5) {
// Rp exists
Rp_conn = 1;
}
else Rp_conn = 0;
// TODO - later - this also helps us know which side is UP
// --------- End 4 Pullup Rp


// 5 --------  Test For Rd on A_CC1 (use INPUT_PULLUP)
pinMode(A_CC1, INPUT_PULLUP);
pinMode(A_CC2, INPUT_PULLUP);
delay(10);
int a_cc1 = digitalRead(A_CC1);
int a_cc2 = digitalRead(A_CC2);
if ( (a_cc1 == 0) || (a_cc2 == 0) ){
  // if one is pulled down, then we have an Rp present
  Rd_conn = 1;
}
else Rd_conn = 0;

// when done need to put A_CC1 back to output
pinMode(A_CC1, OUTPUT);
pinMode(A_CC2, OUTPUT);

// -------- end 5 Test for Rd


// Toggle LEDs
digitalWrite(GND_LED, gnd_conn);
digitalWrite(VBUS_LED, vbus_conn);
digitalWrite(CC_FLIP_A_LED, cc1_conn);
digitalWrite(CC_FLIP_B_LED, cc1_conn_flip);
// digitalWrite(30, cc1_conn);
// digitalWrite(32, cc1_conn_flip);
digitalWrite(CC_Rp_LED, Rp_conn);
digitalWrite(CC_Rd_LED, Rd_conn);

delay(1000);

}