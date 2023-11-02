#include <LiquidCrystal_I2C.h>
#include "Horno.h"
#include "Arduino.h"



LiquidCrystal_I2C lcd_1(0x27,16,2);


int pin_calentador = 2;



Horno pava(100,80,82,28,Horno::Control::PD,pin_calentador);

void setup()
{
  lcd_1.init();
  lcd_1.setBacklight(1);
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.setCursor(0, 1);
  lcd_1.print("PWM%: ");
  pinMode(pin_calentador, OUTPUT);

}

void loop()
{
  pava.perdidas_horno();

  lcd_1.setCursor(6, 0);
  lcd_1.print(pava.t_horno);
  
  lcd_1.setCursor(6, 1);
  lcd_1.print(pava.calentador_PD());

  delay(pava.delay_en_ms);
  
}