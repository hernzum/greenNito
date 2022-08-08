  /* LM35 analog temperature sensor with Arduino example code. More info: https://www.makerguides.com */
  /* Led Amarillo  pin---> D6 
    Led Verde  pin---> D7
    Led Rojo  pin---> D8
    Servo  pin---> D9
    Water Pump  pin---> D2
    ventilador  pin---> D3
    Relay for Luces ---> D4
    Temp LM35Dt  pin---> A0
    LDR   pin--->A1
    Soil sensor  pin---> A2 --> commercial
    DIYSoil sensor pin---> A2 ---> DIY
  */

  #include <LM35.h> // LM35DT -- version Latest
  #include <Servo.h>   // Servo - Version: Latest 
  int rojo_temp_pin = 8; //led rojo para mostar temp sobre 27 grados
  int verde_mojado_pin = 7; // led verde para para mostar q esta regado
  int amarillo_seco_pin = 6; // led amarillo mostrar q necesita agua
  int pump = 2;
  int fan = 3;
  int luz = 4;
  int histeresisT = 3;   // histerresisT = rango en que opera el servo
  int histeresisL = 10; //histeresisL = acion de la luz
  int histeresisH = 10;  // histereisisH = rango que opera la bomba de agua
  int pos = 0;
  int Ldr; // variable para sensor de luz
  int SoilSensor;// Variable para sensor humedad en tierra.
  int TempC; // Variable para sensor de Temperatura LM35
  unsigned long  moistureTest; // Variable para refrescar datos del sensor de suelo
  unsigned long ldrTest; // variable para refrescar sensor de Luz
  unsigned long sampleTemp; //variable para refrscar sensor de  Temperatura
  Servo myservo;  // Variable usada para el Servo
  LM35 tempsensor(0);
  
  void setup() {
    Serial.begin(9600);  // Begin serial communication at a baud rate of 9600:
    pinMode(rojo_temp_pin, OUTPUT);
    pinMode(verde_mojado_pin, OUTPUT);
    pinMode(amarillo_seco_pin, OUTPUT);
    pinMode(pump, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(luz, OUTPUT);
    pinMode(Ldr, INPUT);
    pinMode(SoilSensor, INPUT);
    pinMode(TempC, INPUT);
    myservo.attach(9); // Inicia Servo on Pin 9
    myservo.write(pos);
    
   
  }
  
  void loop() {
  
      
    //----------------Testea la humedad de la tierra------------//
    
    if (millis() - moistureTest > 3000UL)  // Sondeamos la humedad de la tierra cada 3 segundos
    {
      moistureSampling();
      moistureTest = millis();
    }
  
    //------------------Fin testeo Humedad de la tierra----//
  
  
    //------------------ Testea la Cantidad de Luz--------------//
  
    if (millis() - ldrTest > 3000UL)  // Sondeamos la luz cada 3 segundos
    {
      detector_luz();
      ldrTest = millis();
    }
    //-------------------Fin testeo cantidad de luz------//
     //----------------Testea la Temperatura------------//
  
      if (millis() - sampleTemp > 5000UL)  // Sondeamos la temperatura cada  5 segundos
      {
      SamplingTemp();
      sampleTemp = millis();
      }
  
      //------------------Fin testeo Temperatura----//
    
    //---------- Muestra los datos en la com serial -------//
  
    Serial.print(Ldr);
    Serial.println(" lux");
    Serial.print(SoilSensor);
    Serial.println(" % humedad");
    Serial.print(TempC);                            // Print the temperature in the Serial Monitor:
    Serial.print(" \xC2\xB0");                        // shows degree symbol
    Serial.println("C");
    //Serial.println(soil);
   
  }
  
  void moistureSampling () { // Rutina para sondear la humedad de la tierra.
  
   SoilSensor = analogRead(A2); // Leemos sensor de suelo, entrada analogica pin A2:
   SoilSensor = map (SoilSensor, 550, 212, 0, 100); // --> Soil--Mapeamos el valor del sensor de 0 a 100.
  //  SoilSensor = map (SoilSensor, 212, 550, 100, 0); // -->DIY-- Mapeamos el valor del sensor de 0 a 100.
   
   
    if (SoilSensor < 30) // Si el sensor detecta menos de 30%H
    {
      digitalWrite(pump, HIGH);
      digitalWrite(verde_mojado_pin, HIGH);
      
    }
    if (SoilSensor > (30 + histeresisH))// Si detecta mas de 90%H
    {
      digitalWrite(pump, LOW);
      digitalWrite(verde_mojado_pin, HIGH);
      delay(100);
      digitalWrite(verde_mojado_pin, LOW);
      
    }
  }
  void detector_luz () { // Rutina para scanear la intensidad de Luz
    
    Ldr = analogRead(A1);
    Ldr = map (Ldr, 0, 1023, 100, 0);             // Mapeamos el valor del sensor de 0 a 100.
  if (Ldr < 60)
    {
    digitalWrite(luz, HIGH);
    digitalWrite(amarillo_seco_pin, HIGH);
    delay(100);
    digitalWrite(amarillo_seco_pin, LOW);
  }
  if (Ldr > (60 - histeresisL))
  
  {
    digitalWrite(luz, LOW);
    digitalWrite(amarillo_seco_pin, LOW);
  }
  
  
  }
  void SamplingTemp () { // Rutina para scanear la temperatura
    
    TempC = tempsensor.getTemp(CELCIUS);
    
     if (TempC > 27)
    {
      myservo.write(70);     
      digitalWrite(fan, HIGH);
      digitalWrite(rojo_temp_pin, HIGH);
     
   }
  
    if (TempC < (27 - histeresisT))
    {
      myservo.write(0);
      digitalWrite(fan, LOW);
      digitalWrite(rojo_temp_pin, LOW);
      
    }
  
  }
  
