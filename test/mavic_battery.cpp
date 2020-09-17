// Attiny85 (2-PB3, 3-PB4, 5-PB0, 6-PB1, 7-PB2)
#include <Arduino.h>


#define but_tap 3 
#define but_count 4  
#define volt 1
// #define led 0
     

void onBat();
boolean flag = false;      
int buttonPushCounter=0;


void setup() 
{
  pinMode(but_tap, OUTPUT);
  pinMode(but_count, INPUT);
  digitalWrite(but_tap, HIGH);
  pinMode(volt, INPUT);
  // pinMode(led, OUTPUT);
  // digitalWrite(led, LOW);
}

void loop() 
{
    
 
    if (digitalRead(but_count) == LOW)
       {
           buttonPushCounter++;
           delay(100);
       }    
    if (buttonPushCounter >=2 && digitalRead(volt) == LOW && flag == false)
      {  
        buttonPushCounter = 0;
        flag = true;
        delay(500);
      }
    
 
    if (flag == true && digitalRead(volt) == HIGH && buttonPushCounter == 0)
    {            
      delay(6000);
      onBat();
    }
 
    if (buttonPushCounter >= 2 && digitalRead(volt) == HIGH && flag == true)
    { 
      buttonPushCounter = 0; 
      flag = false;
      delay(500);
    }
}
void onBat(){                                
      digitalWrite(but_tap, LOW);
      delay(300);
      digitalWrite(but_tap, HIGH);
      delay(600);
      digitalWrite(but_tap, LOW);
      delay(3000);
      digitalWrite(but_tap, HIGH);
      delay(2000);
}
