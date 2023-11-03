#ifndef Horno_h
#define Horno_h

class Horno
{
  public:
    float t_ambiente {28};
    float t_horno {t_ambiente};// temperatura actual del horno, obtenida por simulacion o lectura del termometro
    float t_max {300};
    float t_deseada {200};

    bool horno_encendido {false};                          // no te olvides de encender el horno

    // para la simulacion
    float dt_calentar {20.0};// °C por minuto
    float dt_perdidas {3.0};// °C por minuto
    

    // constantes P I D
    float porcentaje_banda {10};
    float constante_derivariva {5};//D 
    float constante_integral {100};//I
    
    // escenciales para integal y derivada
    float e_anterior {0};
    float suma_errores {0};
    
    //regula la velocidad de medicion
    int delay_en_ms {500};
    

    // enum para hacer mas entendible el switch
    enum Control 
    {
      On_Off=1,
      P,
      PD,
      PII,
      PID,
    };

    // varible que guarda el tipo de PID a usar
    int controlador {On_Off};

    Horno(float t_m,float t_d,float t_i,float t_a, Control tipo);

    
  private:
    float salida_on_off_porcentaje();
    float salida_proporcional_porcentaje();
    float salida_derivativa_porcentaje();
    float salida_integral_porcentaje();

  public:

    void lectura_termometro(float temperatura);// permite guardar la lectura de un termometro

    float select_calentador();

    // solo para las simulaciones
    void perdidas_horno_simulada();
    void ganacia_horno_simulada(float pwm);

};

#endif
