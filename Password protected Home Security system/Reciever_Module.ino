/*
Project     : Home Security System (Reciever Module)
Developer   : Saurabh Agrawal
Components  : Arduino UNO , Keypad , LCD (16x2) , RF Reciever,
              Buzzer , LEDs , Battery (9V) ; 
*/


#include <LiquidCrystal.h>
#include <Wire.h>
#include <Keypad.h>
#include <VirtualWire.h>
#include <Password.h>

//Defining Variables for Keypad
const byte rows = 4;
const byte cols = 4;
#define lengthPass 4;

char keys[rows][cols] = 
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

//Assigning pins to Keyboard
byte rowPins[rows] = {11,10,9,8};
byte colPins[cols] = {7,6,5,4};

Keypad keypad = Keypad(makeKeymap(keys) , rowPins , colPins , rows , cols);

//Assigning pins to LCD
LiquidCrystal lcd (A0 , A1 , A2 , A3 , A4 , A5);

//Assigning pins to LEDs , RF Module and Buzzer 
int ledR2 = 13; // wrong password
int ledR1 = 13; // to indicate that reciever ckt. is ON (0_0)
int ledG = 2;
//int ledG2 = 0;// for right password
int receive_pin = 12;
int buz = 3; // alarm system

//assigning password 
Password password = Password("1234");

//variables
int sett;
int flag ;
int active ;
int correctPass ;
int initial_stage;
int beep;
int ff;

void setup()
{
   Serial.begin(9600); // Debugging only
   
   //initializing the LCD
   LCD_initialize();
   
   //keypad Initializing
   keypad.addEventListener(keypadEvent);
   keypad.setHoldTime(200);
   keypad.setDebounceTime(200);
   
   // Initialise the IO and ISR
   vw_set_rx_pin(receive_pin);
   vw_setup(4000); // Transmission rate
   // Start the receiver PLL
   vw_rx_start();
   
   // Set LED pin and Buzzer
   pinMode(ledR1, OUTPUT);
   pinMode(ledR2, OUTPUT);
   pinMode(ledG, OUTPUT);
  // pinMode(ledG2, OUTPUT);
   pinMode(buz, OUTPUT);
   
   // Initialize them with some value
   digitalWrite(ledR2 , HIGH);
   digitalWrite(ledR1 , LOW);
   digitalWrite(ledG , LOW);
   //digitalWrite(ledG2 , LOW);
   digitalWrite(buz , LOW);
   
   //Initialize Variables
   sett=0;
   correctPass=0;
   flag=0;
   active=0;
   initial_stage=0;
   beep=0;
   ff = 0;
}
 
void loop()
{
    uint8_t buf[VW_MAX_MESSAGE_LEN];
    uint8_t buflen = VW_MAX_MESSAGE_LEN;

   char key = keypad.getKey();
   digitalWrite(buz, LOW);
   if(active)
   {
      if (vw_get_message(buf, &buflen)) 
      {
        if(buf[0]=='1')
        {
          if(beep==0)
          {
             lcd.clear();
             lcd.print("Motion Detected");
             beep=1;
          }
          //key = keypad.getKey();
          Serial.println("Motion detected!");
          //delay(500);
          playTone(300, 160);
          //delay(1500);
          initial_stage=1;
        }  
         if(buf[0]=='0')
         {
           if(initial_stage==1)
           {
             playTone(300,160);
           }
         }
     }
   }
   if(!active)  initial_stage=0;
}

//take care of Keypad events
void keypadEvent(KeypadEvent eKey)
{
    switch (keypad.getState())
    {
     case PRESSED:
      if(sett==1)
      {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Enter Password :");
        lcd.setCursor(0,1);
        sett=0;
      }
     Serial.println(eKey);
	switch (eKey)
        {
	  case '*': checkPassword(); break;
	   default:
                  lcd.print("*");
		   password.append(eKey);
        }
    }
}

void checkPassword(){
     if (password.evaluate()){
              ledG_blink();
              lcd.clear();
              password.reset(); //resets password after correct entry
              flag = 1;
              correctPass=1;
              active = !active;
              if(active)
              {
                lcd.setCursor(0,0);
                lcd.print("Mr.Saurabh--");
                lcd.setCursor(3,1);
                lcd.print("--Lock Active");
                //digitalWrite(ledG2 , HIGH );
                delay(2000); 
                sett=1;
              }
              else
              { 
                stop_buz();
                lcd.setCursor(1,0);
                lcd.print("Lock Deactive");
              //  digitalWrite(ledG2 , LOW);
                delay(2000);
                evaluatePass();
                beep=0;
                sett=1;
              }
     }
  
     
     else{
              lcd.clear();
	      ledR1_blink();
              lcd.setCursor(0,0);
	      lcd.print("INVALID PASSWORD");
              //lcd.setCursor(3,1);
              //lcd.print("PASSWORD");
              password.reset(); //resets password after INCORRECT entry
              delay(2000);
              lcd.clear();
              lcd.setCursor(1,0);
              lcd.print("Enter Again:");
              sett=1;
              flag = 0;
              correctPass=0;
     }
}

void stop_buz()
{
  if(!active)
  {
    digitalWrite(buz , LOW );
    lcd.setCursor(0,0);
    lcd.print("Alarmed---");
    lcd.setCursor(6,1);
    lcd.print("---Stopped");
    delay(2000);
    lcd.clear();
  }
}
void ledR1_blink() // WRONG PASSWORD
{
  digitalWrite(ledR1,HIGH);
  delay(500);
  digitalWrite(ledR1,LOW);
  delay(500);
  digitalWrite(ledR1,HIGH);
  delay(500);
  digitalWrite(ledR1,LOW);
  delay(500);
}
void ledG_blink() // WRITE PASSWORD
{
  digitalWrite(ledG,HIGH);
  delay(500);
  digitalWrite(ledG,LOW);
  delay(500);
  digitalWrite(ledG,HIGH);
  delay(500);
  digitalWrite(ledG,LOW);
  delay(500);
}

void ledR2_blink()  // TO SHOW TRANSMISSION
{
  digitalWrite(ledR2,HIGH);
  delay(500);
  digitalWrite(ledR2,LOW);
  delay(500);
  digitalWrite(ledR2,HIGH);
  delay(500);
  digitalWrite(ledR2,LOW);
  delay(500);
}


void evaluatePass()
{
              lcd.clear();
              lcd.setCursor(0,0);
              lcd.print("-Access-");
              lcd.setCursor(4,1);
              lcd.print("Granted");
              delay(2000);
              lcd.clear();
              lcd.setCursor(4,0);
              lcd.print("Welcome");
              lcd.setCursor(2,1);
              lcd.print("Mr. Saurabh");
              delay(2000);
}

void LCD_initialize()
{
   lcd.begin(16,2);
   lcd.clear();
   lcd.setCursor(6,0);
   lcd.print("Home");
   lcd.setCursor(0,1);
   lcd.print("Security System");
   delay(2000);
   lcd.clear();
   lcd.setCursor(2,0);
   lcd.print("Designed By");
   lcd.setCursor(0,1);
   lcd.print("Saurabh Agrawal");
   delay(2000);
   lcd.clear();
   lcd.setCursor(1,0);
   lcd.print("Enter Password:");
   lcd.setCursor(0,5);
   lcd.cursor();
     
}

// duration in mSecs, frequency in hertz
void playTone(long duration, int freq) 
{
    duration *= 1000;
    int period = (1.0 / freq) * 1000000;
    long elapsed_time = 0;
    while (elapsed_time < duration) 
    {
    digitalWrite( buz , HIGH );
    delayMicroseconds(period / 2);
    digitalWrite( buz , LOW );
    delayMicroseconds( period / 2 );
    elapsed_time += ( period );
   }
}
