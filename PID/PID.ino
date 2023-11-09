#include <LiquidCrystal_I2C.h>
#include <LiquidCrystal.h>
#include "Horno.h"

const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;

const int pin_select{ 10 };
const int pin_subir{ 9 };
const int pin_bajar{ 8 };
const int pin_on_off{ 7 };
const int pin_led{ 6 };

//LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
LiquidCrystal_I2C lcd(0x27, 16, 2);

float temperatura_actual{ 28 };
float temperatura_set{ 200 };
float banda_set{ 5 };

Horno pava(500, temperatura_set, temperatura_actual, 28, Horno::Control::PID);

void setup() {

  lcd.begin(16, 2);
  lcd.init();
  lcd.setBacklight(1);

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

const long unsigned millis_ciclo{ 6000 };
long unsigned millis_pwm{ 0 };

float pwm_porcentaje{ 100 };
float pwm_porcentaje_set{ 100 };

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