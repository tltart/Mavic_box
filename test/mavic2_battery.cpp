// Attiny85 (2-PB3, 3-PB4, 5-PB0, 6-PB1, 7-PB2)
#include <Arduino.h>


#define but_tap 3 
#define volt 1
     

void onBat();
boolean flag = false;      

void setup() 
{
  pinMode(but_tap, OUTPUT);
  digitalWrite(but_tap, HIGH);
  pinMode(volt, INPUT);
}

void loop() 
{ 
  if (flag && digitalRead(volt) == HIGH) 
  {
     onBat();
     delay(2000);
     onBat();
     flag = false;
  }

  while (flag == false)
  {
       if (digitalRead(volt) == LOW)
    {  
      delay(500);
      if (digitalRead(volt) == LOW) 
        {
          flag = true;
        }      
      else
      {
        break;
      }
      
   }
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
