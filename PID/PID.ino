#include <LiquidCrystal_I2C.h>
#include "Horno.h"
#include "Arduino.h"



LiquidCrystal_I2C lcd_1(0x27,16,2);


int pin_calentador = 2;



Horno pava(300,200,28,28,Horno::Control::PID,pin_calentador);

void setup()
{
  lcd_1.init();
  lcd_1.setBacklight(1);
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.setCursor(0, 1);
  lcd_1.print("PWM%: ");
  pinMode(pin_calentador, OUTPUT);
  Serial.begin(9600);
}

void loop()
{
  

  lcd_1.setCursor(6, 0);
  lcd_1.print(pava.t_horno);

  Serial.print(pava.t_horno);
  Serial.print("   ");
  Serial.print(pava.controlador);
  Serial.print("   ");

  lcd_1.setCursor(6, 1);
  float pp = pava.select_calentador();
  Serial.print(pp);
  Serial.print("\n");
  lcd_1.print(pp);
  
  pava.perdidas_horno();
  
  delay(pava.delay_en_ms);
  
}