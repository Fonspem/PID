#ifndef Horno_h
#define Horno_h
#include "Arduino.h" 



class Horno
{
  public:
    int pin_calentador{};

    float t_ambiente {28};
    float t_horno {t_ambiente};
    float t_max {300};
    float t_deseada {200};

    bool horno_encendido {false};
    bool calentador_encendido {false};

    float dt_calentar {200.0};
    float dt_perdidas {3.0};
    
    float porcentaje_banda {10};//P [%]
    float constante_derivariva {5};//D [s]
    
    float e_anterior{0};
    
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

    Horno(float t_m,float t_d,float t_a, Control tipo,int pin);

    float salida_proporcional_porcentaje();
    float proporcional_derivativa_porcentaje();
    void perdidas_horno();
    bool calentador_on_off();
    float calentador_P();
    float calentador_PD();
};

#endif
