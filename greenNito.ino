/*
 * Sistema de Riego Automático con Arduino - Versión 4.1
 * 
 * Autor: Nito
 * Fecha: 2024-07-05
 * 
 * Este programa controla un sistema de riego automático que incluye:
 * - Lectura de humedad del suelo con un sensor de humedad.
 * - Lectura de temperatura con un sensor LM35.
 * - Control de LEDs para indicar diferentes estados.
 * - Control de una bomba de agua.
 * - Control de un ventilador.
 * - Uso de un servo para el control del flujo de agua (opcional).
 * - Uso de un modo de bajo consumo para ahorrar energía (opcional).
 * 
 * Para usar el programa:
 * 1. Conecta los sensores y actuadores a los pines especificados en el código.
 * 2. Descomenta las líneas de definición (#define) según los componentes que estés utilizando.
 * 3. Sube el código al Arduino y monitoriza la salida serie para depuración (si debug está habilitado).
 */

#include <LowPower.h>  // Librería para modo de bajo consumo
#include <LM35.h>      // Librería para el sensor LM35

// Descomentar según el uso de los componentes
#define tempUsed       // Usar si se utiliza el sensor de temperatura
//#define servoWaterUsed // Usar si se utiliza el servo
#define ledUsed        // Usar si se utilizan los LEDs
#define lowPowerUsed   // Usar si se utiliza el modo de bajo consumo

// Configuración de depuración
const bool debug = 1;  // Establecer a 1 para habilitar la depuración por serial

// Definición de pines
const int rojo_temp_pin = 12;          // Pin del LED rojo
const int verde_mojado_pin = 11;       // Pin del LED verde
const int amarillo_seco_pin = 10;      // Pin del LED amarillo
const int pump = 2;                    // Pin de la bomba de agua
const int fan = 13;                    // Pin del ventilador
const int EnServo = 5;                 // Pin del servo
const int EnSensor2 = 4;               // Pin del sensor 2
const int EnSensor = 3;                // Pin del sensor
const int settButton = 8;              // Pin del botón de configuración
const int waterButton = 9;             // Pin del botón de riego
const int servoPin = A5;               // Pin del servo (analógico)
const int soilPin = A3;                // Pin del sensor de humedad del suelo (analógico)
const int sensor2 = A4;                // Pin del sensor 2 (analógico)
const int ldrPin = A1;                 // Pin del sensor de luz LDR (analógico)
const int tempPin = A0;                // Pin del sensor de temperatura LM35 (analógico)

// Declaración de variables
int histeresisT = 3;                   // Histéresis de temperatura
int histeresisL = 10;                  // Histéresis de luz
int histeresisH = 10;                  // Histéresis de humedad
int attempt = 0;                       // Intentos de riego
int pos = 0;                           // Posición del servo
int Ldr = 0;                           // Valor del sensor de luz
int SoilSensor = 0;                    // Valor del sensor de humedad del suelo
int TempC = 0;                         // Valor de la temperatura en grados Celsius
int waterTime = 15000;                 // Tiempo que la bomba estará encendida por ciclo (en milisegundos)
int soilPlanta = 70;                   // Nivel de humedad del suelo para activar la bomba
int maxAttempts = 10;                  // Número máximo de intentos de riego
int ldrVal = 30;                       // Valor del LDR para controlar la bomba

unsigned long moistureTest;            // Tiempo de la última prueba de humedad
unsigned long ldrTest;                 // Tiempo de la última prueba de luz
unsigned long sampleTemp;              // Tiempo de la última muestra de temperatura

boolean waterButtonState = true;       // Estado del botón de riego
boolean settButtonState = true;        // Estado del botón de configuración

// Inicialización del sensor de temperatura
#ifdef tempUsed
LM35 tempsensor(tempPin);
#endif

void setup() {
  if (debug) Serial.begin(9600);  // Iniciar comunicación serie para depuración

  // Configuración de pines
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

  // Lectura del sensor de temperatura cada 5 segundos
  #ifdef tempUsed
    if (millis() - sampleTemp >= 5000) {
      sampleTemp = millis();
      TempC = tempsensor.getTempC();
    }
  #endif

  #ifdef ledUsed
    if (debug) {
      Serial.print("TempC: ");
      Serial.print(TempC);
      Serial.println("C");
    }
  #endif

  // Lectura del botón de configuración
  settButtonState = digitalRead(settButton);
  if (!settButtonState) {
    if (debug) Serial.println("settButton Presionado");
    delay(2000);  // Evitar rebotes del botón
    while (!settButtonState) {
      settButtonState = digitalRead(settButton);
      if (debug) Serial.println("settButton Presionado");
      delay(2000);  // Evitar rebotes del botón
    }
    // Código para el botón de configuración
  }

  // Lectura del botón de riego
  waterButtonState = digitalRead(waterButton);
  if (!waterButtonState) {
    if (debug) Serial.println("waterButton Presionado");
    delay(2000);  // Evitar rebotes del botón
    while (!waterButtonState) {
      waterButtonState = digitalRead(waterButton);
      if (debug) Serial.println("waterButton Presionado");
      delay(2000);  // Evitar rebotes del botón
    }
    // Código para el botón de riego
  }

  // Lectura de la humedad del suelo
  SoilSensor = getSoilMoisture();

  #ifdef ledUsed
    if (debug) {
      Serial.print("Humedad del Suelo: ");
      Serial.print(SoilSensor);
      Serial.println("%");
    }
  #endif

  // Control de los LEDs y la bomba de agua
  if (SoilSensor > soilPlanta && Ldr < ldrVal) {
    digitalWrite(verde_mojado_pin, LOW);
    digitalWrite(amarillo_seco_pin, HIGH);
    // Activar la bomba de agua
  } else {
    digitalWrite(verde_mojado_pin, HIGH);
    digitalWrite(amarillo_seco_pin, LOW);
    // Desactivar la bomba de agua
  }

  // Control del servo y el LED rojo según la temperatura
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

// Función para obtener la humedad del suelo
int getSoilMoisture() {
  moistureTest = millis();
  if (moistureTest - sampleTemp >= 1000) {
    sampleTemp = moistureTest;
    return analogRead(soilPin);
  } else {
    return SoilSensor;
  }
}

// Función para poner el Arduino en modo de bajo consumo
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
