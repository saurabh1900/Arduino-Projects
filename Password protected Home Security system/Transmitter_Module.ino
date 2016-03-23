/*
Project     : Home Security System (Transmitter Module)
Developer   : Saurabh Agrawal
Roll. No.   : 130002033
Components  : Arduino NANO , RF Transmitter, PIR sensor(2)
              Buzzer , LEDs , Battery (9V) , Switch ; 
*/

#include <LiquidCrystal.h>
#include <Wire.h>
#include <Keypad.h>
#include <VirtualWire.h>
#include <Password.h>

int ledR1 = 13; // Laser Security Breached
int ledR2 = 2;  // to indicate that tarnsmitter ckt. is ON (O_0)
//int switch1 = 0;
int ledB1 = 6; // to show Transmission 
int ledB2 = 12; // PIR  breached 
int transmit_pin = 8;
int pir = 4; // main PIR
int buz = 3; 
int temp = A2 ;  

int pir_state = LOW;

int val;
float vv;

void setup()
{
   Serial.begin(9600);
   //Initialize transmission
   vw_set_tx_pin(transmit_pin);
   vw_setup(4000); // Transmission rate
   
   //set All components
   pinMode(ledR1, OUTPUT);
   pinMode(ledR2, OUTPUT);
   pinMode(ledB1, OUTPUT);
   pinMode(ledB2, OUTPUT);
   pinMode(buz , OUTPUT);
   pinMode(pir , INPUT);
   pinMode(temp , INPUT);
   
   //Initialize then with some value
   digitalWrite(ledR2 , HIGH);
   digitalWrite(ledR1 , LOW);
   digitalWrite(ledB1 , LOW);
   digitalWrite(ledB2 , LOW);
   digitalWrite(buz , LOW);
}
 
void loop()
{
  //Serial.println(analogRead(temp));
  vv = analogRead(temp);
  vv = (vv *500)/1024;
  //vv = vv / 1024 ;
  Serial.println(vv);
  delay(1000);
  char msg[1] = {'0'};
  // Get sensor value
  val = digitalRead(pir);
 //Serial.println(digitalRead(pir));
  //msg[1] = vv;
  // Change message if motion is detected
  if (val== 1)
  {
      msg[0] = '1';
      ledB1_blink();
      //digitalWrite(ledB1, HIGH); // Flash a light to show transmitting
      vw_send((uint8_t *)msg, 1);
      vw_wait_tx(); // Wait until the whole message is gone
      if (pir_state == LOW) 
      {
     // Serial.println("Motion detected!");
      pir_state = HIGH;
      }
   }
 else
 {
   msg[0] = '0';
   digitalWrite(ledB1, LOW);
   digitalWrite(ledB2, LOW);
   vw_send((uint8_t *)msg, 1);
   vw_wait_tx(); // Wait until the whole message is gone
   if (pir_state == HIGH)
   {
     // Serial.println("Motion ended!");
      pir_state = LOW;
   }
  }
}

void ledB1_blink()
{
  digitalWrite(ledB1,HIGH);
  delay(500);
  digitalWrite(ledB1,LOW);
  delay(500);
  digitalWrite(ledB1,HIGH);
  delay(500);
  digitalWrite(ledB1,LOW);
  delay(500);
}


