#include <LiquidCrystal.h>
class Horno {
public:
  float t_ambiente{ 28 };
  float t_horno{ t_ambiente };  // temperatura actual del horno, obtenida por simulacion o lectura del termometro
  float t_max{ 500 };
  float t_deseada{ 200 };

  bool resistencia_encendida{ false };
  // para la simulacion
  float dt_calentar{ 30.0 };  // °C por minuto
  float dt_apagado{ 3.0 };    // °C por minuto


  // constantes P I D
  float porcentaje_banda{ 5 };
  float constante_derivativa{ 3 };  //D
  float constante_integral{ 100 };  //I

  // escenciales para integal y derivada
  float e_anterior{ 0 };
  float suma_errores{ 0 };
  float pendiente_error{ 0 };

  //regula la velocidad de medicion
  int delay_en_ms{ 2000 };  // cada 2s


  Horno(float t_m, float t_d, float t_i, float t_a) {
    t_ambiente = t_a;
    t_horno = t_i;
    t_max = t_m;
    t_deseada = t_d;
    e_anterior = t_deseada - t_horno;
    pendiente_error = (t_deseada - t_horno - e_anterior) / (delay_en_ms / 1000.0);
  };

  float salida_proporcional_porcentaje() {
    float x = (t_deseada - t_horno) * 100.0 / (t_deseada * (porcentaje_banda / 100.0));

    return x;
  }

  float salida_derivativa_porcentaje() {
    float E, Kd, x;

    Kd = (100.0 * constante_derivativa) / (t_deseada * (porcentaje_banda / 100.0));
    E = t_deseada - t_horno;

    pendiente_error = ((E - e_anterior) / (delay_en_ms / 1000.0));

    x = (Kd * pendiente_error);
    e_anterior = E;

    return x;
  }

  float salida_integral_porcentaje() {
    suma_errores += (t_deseada - t_horno);

    float x = (constante_integral * suma_errores) / (10000.0 * t_deseada * (porcentaje_banda / 100.0));

    return x;
  }

  float salida_pwm_calentador() {
    float PWM{ 0 };

    PWM = salida_proporcional_porcentaje() + salida_derivativa_porcentaje() + salida_integral_porcentaje();

    if (PWM > 100) {
      PWM = 100;
    }
    if (PWM < 0) {
      PWM = 0;
    }
    return PWM;
  }
};


const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);


float temperatura_actual{ 28 };
float temperatura_set{ 200 };
float banda_set{ 10 };

Horno pava(500, temperatura_set, temperatura_actual, 28);


float pwm_porcentaje{ 0 };

const int pin_select{ 10 };
const int pin_subir{ 9 };
const int pin_bajar{ 8 };
const int pin_on_off{ 7 };
const int pin_led{ 6 };

void setup() {

  lcd.begin(16, 2);


  pinMode(rs, OUTPUT);
  pinMode(en, OUTPUT);
  pinMode(d4, OUTPUT);
  pinMode(d5, OUTPUT);
  pinMode(d6, OUTPUT);
  pinMode(d7, OUTPUT);

  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(pin_select, INPUT);
  pinMode(pin_subir, INPUT);
  pinMode(pin_bajar, INPUT);
  pinMode(pin_on_off, INPUT);
  pinMode(pin_led, OUTPUT);


  pwm_porcentaje = pava.salida_pwm_calentador();
}

bool tempset{ true };

void cambiar_display() {
  lcd.setCursor(0, 1);
  lcd.print("T.Actual:");
  lcd.print(temperatura_actual);
  if (tempset) {

    tempset = false;
    lcd.setCursor(0, 0);

    lcd.setCursor(0, 0);
    lcd.print("Banda.Set: ");
    lcd.print(banda_set);

  } else {

    tempset = true;
    lcd.setCursor(0, 0);

    lcd.setCursor(0, 0);
    lcd.print("Temp.Set: ");
    lcd.print(temperatura_set);
  }
}
void actualizar_display() {
  lcd.setCursor(0, 1);
  lcd.print("T.Actual:");
  lcd.print(temperatura_actual);
  if (!tempset) {

    lcd.setCursor(0, 0);

    lcd.setCursor(0, 0);
    lcd.print("Banda.Set: ");
    lcd.print(banda_set);

  } else {
    lcd.setCursor(0, 0);

    lcd.setCursor(0, 0);
    lcd.print("Temp.Set: ");
    lcd.print(temperatura_set);
  }
}



const long unsigned millis_ciclo{ 500 };  // ciclo con una duracion de 0.5s
long unsigned millis_pwm{ 0 };


float pwm_porcentaje_set{ 0 };

const long unsigned millis_inercia{ 10000 };         // inercia por 10s
const long unsigned mantiene_temperatura{ 120000 };  // tiempo que mantiene temperatura
long unsigned millis_i_transcurridos{ 0 };

unsigned long tiempo{ 0 };
long unsigned millis_t_anterior{ 0 };

void PWM_set(const int pin_led1) {

  if ((millis() - millis_pwm) >= millis_ciclo) {  // si ya transcurrió el tiempo de un ciclo reinicia los parametros
    millis_pwm = millis();
    pwm_porcentaje_set = pwm_porcentaje;
  }
  if ((float(millis() - millis_pwm) / float(millis_ciclo)) * 100.0 <= pwm_porcentaje_set and pwm_porcentaje_set != 0 and pwm_porcentaje != 0) {
    digitalWrite(pin_led1, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    float aux = (float(pava.dt_calentar) / 60000.0) * (millis() - tiempo);

    if (temperatura_actual + aux <= 500) {
      temperatura_actual += aux;
    }
    millis_i_transcurridos = 0;
  } else {
    digitalWrite(pin_led1, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    if (millis_i_transcurridos == 0) {
      millis_i_transcurridos = millis();
    }
    if ((millis() - millis_i_transcurridos) <= millis_inercia) {
      float aux = (pava.pendiente_error * float(millis() - tiempo) / 1000.0) * (-1.0);
      if (temperatura_actual + aux <= pava.t_max) {
        temperatura_actual += aux;
      }

    } else if ((millis() - millis_i_transcurridos) >= mantiene_temperatura) {
      float aux = (pava.dt_apagado / 60000) * (millis() - tiempo);
      if (temperatura_actual - aux >= pava.t_ambiente) {
        temperatura_actual -= aux;
      }
    }
  }
  millis_t_anterior = millis();
  tiempo = millis();
}



bool set{ false };
bool subir{ false };
bool bajar{ false };
bool on_off{ false };

bool horno_encendido{ true };

unsigned long tiempo_PID{ 0 };

void loop() {
  delay(100);


  // uso de los botones y control para comodidad
  if (set or subir or bajar or on_off) {
    if (digitalRead(pin_select) == LOW
        and digitalRead(pin_subir) == LOW
        and digitalRead(pin_bajar) == LOW
        and digitalRead(pin_on_off) == LOW) {
      set = subir = bajar = on_off = false;
    }
    return;
  } else {
    if (digitalRead(pin_on_off)) {
      on_off = true;
      if (horno_encendido) {
        horno_encendido = false;
      } else {
        horno_encendido = true;
      }
    }
    if (digitalRead(pin_select)) {
      set = true;
      cambiar_display();
    }
    if (digitalRead(pin_subir)) {
      subir = true;
      if (tempset) {
        if (temperatura_set + 10 <= 350)
          temperatura_set += 10;
      } else {
        if (banda_set + 1 <= 15)
          banda_set += 1;
      }
    }
    if (digitalRead(pin_bajar)) {
      bajar = true;
      if (tempset) {
        if (temperatura_set - 10 >= 150)
          temperatura_set -= 10;
      } else {
        if (banda_set - 1 >= 3)
          banda_set -= 1;
      }
    }
    if (horno_encendido) {
      pava.t_deseada = temperatura_set;
      pava.porcentaje_banda = banda_set;

      PWM_set(pin_led);

      if (millis() - tiempo_PID >= pava.delay_en_ms) {
        tiempo_PID = millis();

        pava.t_horno = temperatura_actual;

        pwm_porcentaje = pava.salida_pwm_calentador();
      }
    } else {
      pwm_porcentaje = 0;
      PWM_set(pin_led);
    }
  }
  actualizar_display();
}