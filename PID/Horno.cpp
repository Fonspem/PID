#include "Horno.h"

Horno::Horno(float t_m,float t_d,float t_i,float t_a, Control tipo)
{
  t_ambiente = t_a;
  t_horno = t_i;
  t_max = t_m;
  t_deseada = t_d;
  controlador = tipo;
};
void Horno::lectura_termometro(float temperatura){
  t_horno = temperatura;
}

float Horno::salida_on_off_porcentaje(){
  if(t_horno < t_deseada){
    return 100.0;
  }
  else{
    return 0.0 ;
  }
}

float Horno::salida_proporcional_porcentaje()
{
  float x = (t_deseada - t_horno) *100.0 / (t_deseada * (porcentaje_banda/100.0));
    
  return x;
}

float Horno::salida_derivativa_porcentaje()
{
  float E,Kd,x;

  Kd = (100.0 * constante_derivariva) / (t_deseada * (porcentaje_banda/100.0));
  E = t_deseada - t_horno;
  
  pendiente_temperatura = ( (E - e_anterior) / (delay_en_ms / 1000.0) );
  
  x = ( Kd *  pendiente_temperatura);
  e_anterior = E;

  return x;
}

float Horno::salida_integral_porcentaje()
{
  suma_errores +=  (t_deseada - t_horno);
  
  float x = (constante_integral * suma_errores) / (10000.0 * t_deseada * (porcentaje_banda/100));

  return x;
}

float Horno::select_calentador(){
  float PWM {0};
  if(horno_encendido){
    switch (controlador) {
      case On_Off:
        PWM = salida_on_off_porcentaje();
        break;
      case P:
        PWM = salida_proporcional_porcentaje();
        break;
      case PD:
        PWM = salida_proporcional_porcentaje() + salida_derivativa_porcentaje();
        break;
      case PII:
        PWM = salida_proporcional_porcentaje() + salida_integral_porcentaje();
        break;
      case PID:
        PWM = salida_proporcional_porcentaje() + salida_derivativa_porcentaje() + salida_integral_porcentaje();
        break;
    }
    if(PWM > 100){
      PWM = 100;
    }
    if(PWM < 0){
      PWM = 0;
    }
  }
  return PWM;
}




