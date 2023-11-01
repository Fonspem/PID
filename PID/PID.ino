#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd_1(0x27,16,2);

const float t_ambiente {28};
float t_horno {175};
const float t_max {300};
float t_deseada {200};

//P [%]
float porcentaje_banda {10};
//D [s]
float constante_derivariva {5};

bool horno {true};
bool calentador {false};

const int delay_en_ms {10};

const float dt_encendido {200.0};
const float dt_apagado {3.0};

int pin_temperatura = A5;
int pin_calentador = 2;

float salida_proporcional_porcentaje(float t_deseada, float t_actual, float banda)// °C,°C,% -> %
{
  float x { (t_deseada - t_actual)*100.0 / (t_deseada*banda/100) };
    
  if(x > 100){
    return 100;
  }
  if(x < 0){}
    return 0;
  }
  return x;
}

float proporcional_derivativa_porcentaje(float t_deseada, float t_actual, float banda, float const_d)// °C,°C,%,k -> %
{
  float Kp,E,Kd,x;

  Kp = 100.0 / (banda*t_deseada/100.0);
  Kd = (100.0 * const_d) / (banda*t_deseada/100.0);
  E = t_deseada - t_actual;
    
  if(x > 100){
    return 100;
  }
  if(x < 0){}
    return 0;
  }
  return x;
}

void calentador_on_off(int PWM){
  if (horno){
    if(t_horno < t_deseada){
      calentador = true;
      digitalWrite(pin_calentador, HIGH);
    }
    else{
      calentador = false;
      digitalWrite(pin_calentador, LOW);
    }
  }
  if (calentador){
      t_horno += (dt_encendido/(60000/delay_en_ms)) * PWM / 100;
    }  
}

void perdidas_horno(){
  if(t_horno > t_ambiente){
    t_horno -= dt_apagado/(60000/delay_en_ms);
  }
  else{
    t_horno = t_ambiente;
  }
}

void setup()
{
  lcd_1.init();
  lcd_1.setBacklight(1);
  lcd_1.setCursor(0, 0);
  lcd_1.print("Temp: ");
  lcd_1.setCursor(0, 1);
  lcd_1.print("PWM%: ");
  pinMode(pin_calentador, OUTPUT);
  pinMode(pin_temperatura, OUTPUT);
}

void loop()
{
  perdidas_horno();

  lcd_1.setCursor(6, 0);
  lcd_1.print(t_horno);
  
  float pwm = salida_proporcional_porcentaje(t_deseada, t_horno, porc_banda);
  
  lcd_1.setCursor(6, 1);
  lcd_1.print(pwm);
  
  calentador_on_off(pwm);

  delay(delay_en_ms);
  
}