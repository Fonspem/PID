#include "Arduino.h" 
#include "Horno.h"

Horno::Horno(float t_m,float t_d,float t_i,float t_a, Control tipo, int pin)
{
  t_ambiente = t_a;
  t_horno = t_i;
  t_max = t_m;
  t_deseada = t_d;
  controlador = tipo;
  pin_calentador = pin;
};

float Horno::salida_proporcional_porcentaje()
{
  float x = (t_deseada - t_horno) *100 / (t_deseada * (porcentaje_banda/100));
    
  return x;
}

float Horno::salida_derivativa_porcentaje()
{
  float E,Kd,x;

  Kd = (100.0 * constante_derivariva) / (t_deseada * (porcentaje_banda/100));
  E = t_deseada - t_horno;
  
  x = ( Kd * ( (E - e_anterior) / (delay_en_ms / 1000.0) ) );
  e_anterior = E;

  return x;
}

float Horno::salida_integral_porcentaje()
{

  suma_errores +=  (t_deseada - t_horno);
  
  float x = (constante_integral * suma_errores) / (10000.0 * t_deseada * (porcentaje_banda/100));

  return x;
}

void Horno::perdidas_horno(){
  if(t_horno > t_ambiente){
    t_horno -= dt_perdidas/(60000/delay_en_ms);
  }
  else if(t_horno < t_ambiente){
    t_horno = t_ambiente;
  }
}

bool Horno::calentador_on_off(){
  if (horno_encendido){
    if(t_horno < t_deseada){//// Ver si corresponde la implementacion
      calentador_encendido = true;
      digitalWrite(pin_calentador, HIGH);
    }
    else{
      calentador_encendido = false;
      digitalWrite(pin_calentador, LOW);
    }
  }
  if (calentador_encendido){// para la simulacion solamente
      t_horno += ( dt_calentar / (60000 / delay_en_ms) );
    }
  return calentador_encendido;
}

float Horno::calentador_P(){

  float PWM = salida_proporcional_porcentaje();
if(PWM > 100){
    PWM = 100;
  }
  if(PWM < 0){
    PWM = 0;
  }
  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += ( dt_calentar / (60000/delay_en_ms) ) * PWM / 100;
  return PWM;
}

float Horno::calentador_PD(){

  float PWM = salida_proporcional_porcentaje() + salida_derivativa_porcentaje();
  if(PWM > 100){
    PWM = 100;
  }
  if(PWM < 0){
    PWM = 0;
  }
  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += (dt_calentar / (60000 / delay_en_ms)) * PWM / 100;
  return PWM;
}

float Horno::calentador_PII(){

  float PWM = salida_proporcional_porcentaje() + salida_integral_porcentaje();
  if(PWM > 100){
    PWM = 100;
  }
  if(PWM < 0){
    PWM = 0;
  }
  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += (dt_calentar / (60000 / delay_en_ms)) * PWM / 100;
  return PWM;
}
float Horno::calentador_PID(){

  float PWM = salida_proporcional_porcentaje() + salida_derivativa_porcentaje() + salida_integral_porcentaje();
  if(PWM > 100){
    PWM = 100;
  }
  if(PWM < 0){
    PWM = 0;
  }
  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += (dt_calentar / (60000 / delay_en_ms)) * PWM / 100;
  return PWM;
}
float Horno::select_calentador(){
  switch (controlador) {
    case On_Off:
      return calentador_on_off();
    case P:
      return calentador_P();
    case PD:
      return calentador_PD();
    case PII:
      return calentador_PII();
    case PID:
      return calentador_PID();
  }
}




