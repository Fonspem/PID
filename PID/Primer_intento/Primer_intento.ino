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
float temperatura_set{ 100 };
float banda_set{ 5 };

unsigned long tiempo{ 0 };

Horno pava(500, temperatura_set, temperatura_actual, 28, Horno::Control::PID);

void setup() {

  //lcd.begin(16, 2);
  lcd.init();
  lcd.setBacklight(1);

  pava.horno_encendido = true;

  pava.delay_en_ms = 100;
}

bool tempset{ true };

void cambiar_display() {
  lcd.setCursor(0, 1);
  lcd.print("Temp.Actual:");
  lcd.print(temperatura_actual);
  if (tempset) {

    tempset = false;
    lcd.setCursor(0, 0);
    //lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Banda.Set: ");
    lcd.print(banda_set);


  } else {

    tempset = true;
    lcd.setCursor(0, 0);
    //lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Temp.Set: ");
    lcd.print(temperatura_set);
  }
}
void actualizar_display() {
  lcd.setCursor(0, 1);
  lcd.print("Temp.Actual:");
  lcd.print(temperatura_actual);
  if (!tempset) {

    lcd.setCursor(0, 0);
    //lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Banda.Set: ");
    lcd.print(banda_set);

  } else {
    lcd.setCursor(0, 0);
    //lcd.print("                ");
    lcd.setCursor(0, 0);
    lcd.print("Temp.Set: ");
    lcd.print(temperatura_set);
  }
}


bool set{ false };
bool subir{ false };
bool bajar{ false };
bool on_off{ false };

void pidd() {

  pava.lectura_termometro(temperatura_actual);

  float PWM = pava.select_calentador();

  pava.ganacia_horno_simulada(PWM);  // por la simulacion

  temperatura_actual = pava.t_horno;

  analogWrite(pin_led, 255 * PWM / 100);

  pava.perdidas_horno_simulada();  // por la simulacion
}

const long unsigned millis_ciclo{ 60000 };
long unsigned millis_pwm{ 0 };

bool PWM_set(float porcentaje) {
  bool salida{ false };
  if (millis() - millis_pwm < millis_ciclo) {
    if (((millis() - millis_pwm) / millis()) * 100 <= porcentaje) {
      salida = true;
    }
  } else {
    millis_pwm = millis();
  }
  return salida;
}

void loop() {

  if (set or subir or bajar or on_off) {
    if (digitalRead(pin_select) == LOW
        and digitalRead(pin_subir) == LOW
        and digitalRead(pin_bajar) == LOW
        and digitalRead(pin_on_off) == LOW) {
      set = subir = bajar = false;
    }
    tiempo += 10;
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

  if (millis() - tiempo >= pava.delay_en_ms) {
    tiempo = millis();
    pidd();
  }

  digitalWrite(pin_led, PWM_set(pava.select_calentador()));

  actualizar_display();
}