#include "Arduino.h" 
#include "Horno.h"

Horno::Horno(float t_m,float t_d,float t_a, Control tipo, int pin)
{
  t_ambiente = t_a;
  t_horno = t_a;
  t_max = t_m;
  t_deseada = t_d;
  controlador = tipo;
  pin_calentador = pin;
};

float Horno::salida_proporcional_porcentaje()
{
  float x = (t_deseada - t_horno)*100.0 / (t_deseada * porcentaje_banda / 100);
    
  if(x > 100){
    return 100;
  }
  if(x < 0){
    return 0;
  }
  return x;
}

float Horno::proporcional_derivativa_porcentaje()
{
  float Kp,E,Kd,x;

  Kp = 100.0 / (porcentaje_banda * t_deseada/100.0);
  Kd = (100.0 * constante_derivariva) / (porcentaje_banda * t_deseada / 100.0);
  E = t_deseada - t_horno;
  
  x = (Kp * E) + ( Kd * (e_anterior/delay_en_ms) );
  e_anterior = E;

  if(x > 100){
    return 100;
  }
  if(x < 0){
    return 0;
  }
  return x;
}

void Horno::perdidas_horno(){
  if(t_horno > t_ambiente){
    t_horno -= dt_perdidas/(60000/delay_en_ms);
  }
  else{
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

  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += ( dt_calentar / (60000/delay_en_ms) ) * PWM / 100;
  return PWM;
}

float Horno::calentador_PD(){

  float PWM = proporcional_derivativa_porcentaje();
  
  analogWrite(pin_calentador, 255 * PWM / 100);
  // para la simulacion solamente
  t_horno += (dt_calentar / (60000 / delay_en_ms)) * PWM / 100;
  return PWM;
}


