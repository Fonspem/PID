//#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
class Horno {
public:
  float t_ambiente{ 28 };
  float t_horno{ t_ambiente };  // temperatura actual del horno, obtenida por simulacion o lectura del termometro
  float t_max{ 300 };
  float t_deseada{ 200 };

  bool horno_encendido{ false };  // no te olvides de encender el horno
  bool resistencia_encendida{ false };
  // para la simulacion
  float dt_calentar{ 30.0 };  // °C por minuto
  float dt_perdidas{ 3.0 };   // °C por minuto


  // constantes P I D
  float porcentaje_banda{ 10 };
  float constante_derivariva{ 5 };  //D
  float constante_integral{ 100 };  //I

  // escenciales para integal y derivada
  float e_anterior{ 0 };
  float suma_errores{ 0 };
  float pendiente_temperatura{ 0 };

  //regula la velocidad de medicion
  int delay_en_ms{ 5000 };


  // enum para hacer mas entendible el switch
  enum Control {
    On_Off = 1,
    P,
    PD,
    PII,
    PID,
  };

  // varible que guarda el tipo de PID a usar
  int controlador{ On_Off };

  Horno(float t_m, float t_d, float t_i, float t_a, Control tipo) {
    t_ambiente = t_a;
    t_horno = t_i;
    t_max = t_m;
    t_deseada = t_d;
    controlador = tipo;
  };

  void lectura_termometro(float temperatura) {
    t_horno = temperatura;
  }

  float  salida_on_off_porcentaje() {
    if (t_horno < t_deseada) {
      return 100.0;
    } else {
      return 0.0;
    }
  }

  float salida_proporcional_porcentaje() {
    float x = (t_deseada - t_horno) * 100.0 / (t_deseada * (porcentaje_banda / 100.0));

    return x;
  }

  float salida_derivativa_porcentaje() {
    float E, Kd, x;

    Kd = (100.0 * constante_derivariva) / (t_deseada * (porcentaje_banda / 100.0));
    E = t_deseada - t_horno;

    pendiente_temperatura = ((E - e_anterior) / (delay_en_ms / 1000.0));

    x = (Kd * pendiente_temperatura);
    e_anterior = E;

    return x;
  }

  float salida_integral_porcentaje() {
    suma_errores += (t_deseada - t_horno);

    float x = (constante_integral * suma_errores) / (10000.0 * t_deseada * (porcentaje_banda / 100));

    return x;
  }

  float select_calentador() {
    float PWM{ 0 };
    if (horno_encendido) {
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
      if (PWM > 100) {
        PWM = 100;
      }
      if (PWM < 0) {
        PWM = 0;
      }
    }
    return PWM;
  }
};

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

const int pin_select{ 10 };
const int pin_subir{ 9 };
const int pin_bajar{ 8 };
const int pin_on_off{ 7 };
const int pin_led{ 6 };

LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
//LiquidCrystal_I2C lcd(0x27, 16, 2);

float temperatura_actual{ 190 };
float temperatura_set{ 200 };
float banda_set{ 15 };

Horno pava(500, temperatura_set, temperatura_actual, 28, Horno::Control::PID);

void setup() {

  lcd.begin(16, 2);
  //lcd.init();
  //lcd.setBacklight(1);

  pava.horno_encendido = true;

  pava.delay_en_ms = 100;
  pinMode(LED_BUILTIN, OUTPUT);
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

bool set{ false };
bool subir{ false };
bool bajar{ false };
bool on_off{ false };

const long unsigned millis_ciclo{ 500 };
long unsigned millis_pwm{ 0 };

float pwm_porcentaje{ 0 };
float pwm_porcentaje_set{ 0 };

bool PWM_set() {
  if ((millis() - millis_pwm) >= millis_ciclo) {
    millis_pwm = millis();
    pwm_porcentaje_set = pwm_porcentaje;
  }
  if ((float(millis() - millis_pwm) / float(millis_ciclo)) * 100.0 <= pwm_porcentaje_set and pwm_porcentaje_set != 0) {
    return true;
  } else {
    return false;
  }
}

unsigned long tiempo{ 0 };

void loop() {
  delay(1);

  if (set or subir or bajar or on_off) {
    if (digitalRead(pin_select) == LOW
        and digitalRead(pin_subir) == LOW
        and digitalRead(pin_bajar) == LOW
        and digitalRead(pin_on_off) == LOW) {
      set = subir = bajar = false;
    }
    return;
  }
  if (digitalRead(pin_select)) {
    set = true;
    cambiar_display();
  }
  if (digitalRead(pin_subir)) {
    subir = true;
    if (tempset) {
      temperatura_set += 10;
    } else {
      banda_set += 1;
    }
  }
  if (digitalRead(pin_bajar)) {
    bajar = true;
    if (tempset) {
      temperatura_set -= 10;
    } else {
      banda_set -= 1;
    }
  }

  pava.t_deseada = temperatura_set;
  pava.porcentaje_banda = banda_set;

  if (PWM_set()) {
    digitalWrite(pin_led, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
    temperatura_actual += (pava.dt_calentar / 60000) * (millis() - tiempo);
  } else {
    digitalWrite(pin_led, LOW);
    digitalWrite(LED_BUILTIN, LOW);
    temperatura_actual -= (pava.dt_perdidas / 60000) * (millis() - tiempo);
  }

  if (millis() - tiempo >= pava.delay_en_ms) {
    tiempo = millis();

    pava.lectura_termometro(temperatura_actual);

    pwm_porcentaje =  pava.select_calentador();
  }

  actualizar_display();
}