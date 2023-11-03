#ifndef Horno_h
#define Horno_h
#include "Arduino.h" 



class Horno
{
  public:

    float t_ambiente {28};
    float t_horno {t_ambiente};
    float t_max {300};
    float t_deseada {200};

    bool horno_encendido {false};
    bool calentador_encendido {false};

    float dt_calentar {20.0};// °C por minuto
    float dt_perdidas {3.0};// °C por minuto
    
    float porcentaje_banda {10};
    float constante_derivariva {5};//D 
    float constante_integral {100};//I
    
    float e_anterior {0};
    float suma_errores {0};

    int delay_en_ms {500};
    
    enum Control 
    {
      On_Off=1,
      P,
      PD,
      PII,
      PID,
    };
    int controlador {On_Off};

    Horno(float t_m,float t_d,float t_i,float t_a, Control tipo);

    void lectura_termometro(float temperatura);

    float salida_on_off_porcentaje();
    float salida_proporcional_porcentaje();
    float salida_derivativa_porcentaje();
    float salida_integral_porcentaje();
    void perdidas_horno_simulada();
    void ganacia_horno_simulada(float pwm);
    float select_calentador();
};

#endif
