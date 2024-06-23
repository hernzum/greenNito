/* Sistema de Control de Riego y Temperatura con Arduino
 * Autor: Nito
 * Instrucciones de Uso:
 * 1. Conectar el sensor LM35 al pin A0.
 * 2. Conectar los LEDs a los pines digitales 10, 11 y 12.
 * 3. Conectar la bomba de agua al pin digital 2.
 * 4. Conectar el ventilador al pin digital 13.
 * 5. Configurar los pines para los botones de configuración y riego en los pines digitales 8 y 9 respectivamente.
 * 6. Opcionalmente, conectar un servo al pin A5.
 * 7. Comentar o descomentar las definiciones en la parte superior del código para habilitar o deshabilitar componentes según su configuración.
 * 8. Ajustar las variables de histéresis y otros parámetros según sus necesidades.
 * 9. Subir el código a su placa Arduino y monitorear los valores a través del monitor serie si la depuración está habilitada (debug).
 */

#include <LowPower.h>  // Librería para modo de bajo consumo
#include <LM35.h>      // Librería para el sensor LM35

#define tempUsed       // Descomentar si se usa el sensor de temperatura
//#define servoWaterUsed // Descomentar si se usa el servo
#define ledUsed        // Descomentar si se usan los LEDs
#define lowPowerUsed   // Descomentar si se usa el modo de bajo consumo

const bool debug = 1;  // Establecer a 1 para habilitar la depuración por serial

const int rojo_temp_pin = 12;
const int verde_mojado_pin = 11;
const int amarillo_seco_pin = 10;
const int pump = 2;
const int fan = 13;
const int EnServo = 5;
const int EnSensor2 = 4;
const int EnSensor = 3;
const int settButton = 8;
const int waterButton = 9;
const int servoPin = A5;
const int soilPin = A3;
const int sensor2 = A4;
const int ldrPin = A1;
const int tempPin = A0;

int histeresisT = 3;
int histeresisL = 10;
int histeresisH = 10;
int attempt = 0;
int pos = 0;
int Ldr = 0;
int SoilSensor = 0;
int TempC = 0;
int waterTime = 15000;
int soilPlanta = 70;
int maxAttempts = 10;
int ldrVal = 30;

unsigned long moistureTest;
unsigned long ldrTest;
unsigned long sampleTemp;

boolean waterButtonState = true;
boolean settButtonState = true;

#ifdef servoWaterUsed
#include <Servo.h>
Servo waterServo;
#endif

#ifdef tempUsed
LM35 tempsensor(tempPin);
#endif

void setup() {
  if (debug) Serial.begin(9600);  // Iniciar comunicación serie para depuración

#ifdef ledUsed
  pinMode(rojo_temp_pin, OUTPUT);
  pinMode(verde_mojado_pin, OUTPUT);
  pinMode(amarillo_seco_pin, OUTPUT);
#endif

#ifdef servoWaterUsed
  waterServo.attach(servoPin);
#endif

  pinMode(pump, OUTPUT);
  pinMode(fan, OUTPUT);
  pinMode(ldrPin, INPUT);
  pinMode(soilPin, INPUT);
  pinMode(sensor2, INPUT);
  pinMode(tempPin, INPUT);
  pinMode(settButton, INPUT_PULLUP);
  pinMode(waterButton, INPUT_PULLUP);

#ifdef lowPowerUsed
  delay(2000);  // Permitir tiempo para abrir el monitor serie
  sleepNow();
#endif

#ifdef tempUsed
  sampleTemp = millis();  // Iniciar toma de muestra del sensor
#endif
}

void loop() {
#ifdef lowPowerUsed
  sleepNow();
#endif

#ifdef tempUsed
  if (millis() - sampleTemp >= 5000) {  // Muestra cada 5s
    sampleTemp = millis();
    TempC = tempsensor.getTempC();  // Obtener temperatura
  }
#endif

#ifdef ledUsed
  if (debug) {
    Serial.print("TempC: ");
    Serial.print(TempC);
    Serial.println("C");
  }
#endif

  settButtonState = digitalRead(settButton);
  if (!settButtonState) {
    if (debug) Serial.println("settButton Presionado");
    delay(2000);  // Evitar rebotes del botón
    while (!settButtonState) {
      settButtonState = digitalRead(settButton);
      if (debug) Serial.println("settButton Presionado");
      delay(2000);  // Evitar rebotes del botón
    }
    // Agregar código para el botón de configuración
  }

  waterButtonState = digitalRead(waterButton);
  if (!waterButtonState) {
    if (debug) Serial.println("waterButton Presionado");
    delay(2000);  // Evitar rebotes del botón
    while (!waterButtonState) {
      waterButtonState = digitalRead(waterButton);
      if (debug) Serial.println("waterButton Presionado");
      delay(2000);  // Evitar rebotes del botón
    }
    // Agregar código para el botón de riego
  }

  SoilSensor = getSoilMoisture();  // Obtener humedad del suelo

#ifdef ledUsed
  if (debug) {
    Serial.print("Humedad del Suelo: ");
    Serial.print(SoilSensor);
    Serial.println("%");
  }
#endif

  if (SoilSensor > soilPlanta && Ldr < ldrVal) {
    digitalWrite(verde_mojado_pin, LOW);
    digitalWrite(amarillo_seco_pin, HIGH);
    // Agregar código para activar la bomba de agua
  } else {
    digitalWrite(verde_mojado_pin, HIGH);
    digitalWrite(amarillo_seco_pin, LOW);
    // Agregar código para desactivar la bomba de agua
  }

#ifdef servoWaterUsed
  if (TempC > 27) {
    if (debug) {
      Serial.print("Temp Exceeds Limit: ");
      Serial.print(TempC);
      Serial.println("C");
    }
    digitalWrite(rojo_temp_pin, HIGH);
    waterServo.write(90);
    delay(2000);  // Permitir flujo de agua
    waterServo.write(0);
    delay(2000);  // Esperar para el próximo ciclo
    digitalWrite(rojo_temp_pin, LOW);
  } else {
    digitalWrite(rojo_temp_pin, LOW);
  }
#endif
}

int getSoilMoisture() {
  moistureTest = millis();
  if (moistureTest - sampleTemp >= 1000) {
    sampleTemp = moistureTest;
    return analogRead(soilPin);
  } else {
    return SoilSensor;
  }
}

#ifdef lowPowerUsed
void sleepNow() {
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  delay(2000);
  sleep_mode();
  delay(2000);
  sleep_disable();
}
#endif
