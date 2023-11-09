#include <LiquidCrystal_I2C.h>
#include "Horno.h"


LiquidCrystal_I2C lcd_1(0x27,16,2);

int pin_calentador = 3;

Horno pava(300,200,28,28,Horno::Control::PID);

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
  
  
  pava.horno_encendido = true; //encendemos el horno
}

void loop()
{

  //pabadas para que se vea bien en el lcd y el monitor serial
  lcd_1.setCursor(6, 0);
  lcd_1.print(pava.t_horno);

  Serial.print(pava.t_horno);
  Serial.print("   ");
  Serial.print(pava.suma_errores);
  Serial.print("   ");
  Serial.print(pava.controlador);
  Serial.print("   ");

  lcd_1.setCursor(6, 1);

<<<<<<< Updated upstream
  // aqui iria la toma de temperatura del sensor cuando implementemos
  //  pava.lectura_termometro(valor_del_termometro);
=======
  
  
  pava.lectura_termometro(sensor.getTempCByIndex(0));
  Serial.print(sensor.getTempCByIndex(0));
  Serial.print("°C   ");
>>>>>>> Stashed changes

  float PWM = pava.select_calentador();

  pava.ganacia_horno_simulada(PWM);// por la simulacion
  
  // salida PWM
  analogWrite(pin_calentador, 255 * PWM / 100);


  Serial.print(PWM);
  Serial.print("\n");
  lcd_1.print(PWM);
  
  pava.perdidas_horno_simulada();// por la simulacion
  
  delay(pava.delay_en_ms);
  
}