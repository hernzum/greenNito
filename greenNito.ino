

    /* LM35 analog temperaure sensor with Arduino example code. More info: https://www.makerguides.com */
    /* Led Amarillo  pin---> D10
      Led Verde  pin---> D11
      Led Rojo  pin---> D12
      Servo  pin---> D9
      NEO Pixel LED Pin ----> D6
      Water Pump  pin---> D2
      Ventilador  pin---> D5
     ** Relay for Luces ---> D13**
      Temp LM35Dt  pin---> A0
      LDR   pin--->A1
      Soil sensor  pin---> A3 --> commercial
      Sensor pin---> A4 ---> 
    */
    
    #include <LowPower.h>          //sleep library
    #include <LM35.h> // LM35DT -- version Latest
   // #include <Servo.h>   // Servo - Version: Latest 
    #include <Adafruit_NeoPixel.h>
    
    #define tempUsed //comment out if temp is not used
    //#define servoWaterUsed //comment out if servo is not used.
    //#define neopixelUsed //comment out if neopixel is not used
    #define ledUsed //comment out if Led is not used
    #define lowPowerUsed //comment out if you using arduino mega
    
    const bool debug = 1; //set to 1 to enable serial debugging
    
    const int rojo_temp_pin = 12;           //led rojo para mostar temp sobre 27 grados
    const int verde_mojado_pin = 11;        // led verde para para mostar q esta regado
    const int amarillo_seco_pin = 10;       // led amarillo mostrar q necesita agua
    const int pump          = 2;
    const int fan           = 13;
    const int EnServo       = 5;
    const int EnSensor2     = 4;  // enable sensor no habilitado
    const int EnSensor      = 3;
    const int settButton    = 8;
    const int waterButton   = 9;
    const int servoPin      = A5;
    const int soilPin       = A3;
    const int sensor2       = A4; // sensor no habilitado
    const int ldrPin        = A1;
    const int tempPin       = A0;
    const int LEDPIN        = 6;
    const int NUMPIXELS     = 2;
    const int led1 = 0;
    const int led2 = 1;
    const char *msg = "hello";   
    int histeresisT = 3;   // histerresisT = rango en que opera el servo
    int histeresisL = 10; //histeresisL = acion de la luz
    int histeresisH = 10;  // histereisisH = rango que opera la bomba de agua
    int attempt = 0;
    int pos = 0;
    int Ldr = 0 ; // variable para sensor de luz
    int SoilSensor  = 0;// Variable para sensor humedad en tierra.
    int TempC = 0 ; // Variable para sensor de Temperatura LM35
    int waterTime = 15000; //how long the pump will run each cycle/attempt
    int soilPlanta = 70;
    int maxAttempts = 10;
    int ldrVal      = 30;
    unsigned long moistureTest; // Variable para refrescar datos del sensor de suelo
    unsigned long ldrTest; // variable para refrescar sensor de Luz
    unsigned long sampleTemp; //variable para refrscar sensor de  Temperatura
    //unsigned long sampleTemp;
     /***** Boolean Flags *****/
    boolean waterButtonState=true;
    boolean settButtonState=true;

    #ifdef servoWaterUsed
    Servo waterServo;  // Variable usada para el Servo
    #endif 
  
    #ifdef tempUsed
    LM35 tempsensor(tempPin);
    #endif
    #ifdef neopixelUsed
    Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800);
    #endif
//    RH_ASK driver(2000, 4, 7, 0); // speed RX_pin TX_pin
    void setup() {
      
    if(debug) Serial.begin(9600); //turn on serial for debugging or monitoring values
    
    #ifdef ledUsed
    pinMode(rojo_temp_pin, OUTPUT);
    pinMode(verde_mojado_pin, OUTPUT);
    pinMode(amarillo_seco_pin, OUTPUT);
    #endif
      
    pinMode(pump, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(EnSensor2, OUTPUT);
    pinMode(EnSensor, OUTPUT);
    pinMode(EnServo, OUTPUT);
    pinMode(Ldr, INPUT);
    pinMode(SoilSensor, INPUT);
    pinMode(TempC, INPUT);
    pinMode(waterButton,INPUT_PULLUP);
    pinMode(settButton,INPUT_PULLUP);
     
    #ifdef servoWaterUsed
    waterServo.attach(servoPin);
    digitalWrite(EnServo,HIGH);  //turn on servo
    waterServo.write(0); //rotate servo to the center position (90)
    delay(1000); //give the servo time to move to the center.
   digitalWrite(EnServo,LOW); //turn off servo
    #endif 
      
    #ifdef neopixelUsed
       /**** Pixxel   */
    pixels.begin(); //initialize neopixels
    pixels.setPixelColor(led2, pixels.Color(0,10,10)); //Turn LED white
    pixels.show(); 
    delay(50);
    pixels.setPixelColor(led2, pixels.Color(0,0,5)); //Turn LED white
    pixels.show();
    #endif
    #ifdef servoWaterUsed
        if(debug)Serial.println(".....::: Con servo activado:::....."); 
        #endif
        
        #ifdef tempUsed
        if(debug)Serial.println(".....::: Con Temperatura activado:::.....");
        #endif
        
        #ifdef ledUsed
        if(debug)Serial.println(".....::: Con Led :::.....");
        #endif
        
        #ifdef neopixelUsed
        if(debug)Serial.println(".....::: Con Neopixel:::.....");
        #endif
     
   
   detector_luz();  //primera lectura de la luz ambiente
   moistureSampling(); //primera lectura de la humedad de la tierra
   
   #ifdef tempUsed
   SamplingTemp ();  // Primera lectura de la temperatura ambiente
   #endif

      
    }
    
    void loop() 
    {
    
      //----------------Testea la humedad de la tierra------------//
      
    if (millis() - moistureTest > 120000UL)  // Sondeamos la humedad de la tierra cada 2 min
      {
        moistureSampling();
        moistureTest = millis();
      }
    
    //----- If left button is pressed water the plant but only if a daylight -----//
    
    if(digitalRead(waterButton)==!waterButtonState && (Ldr > ldrVal))   
    {
    water();
    waterButtonState=!waterButtonState;
    
    }
    
    // si la cantidad de intentos de riego supera los 10
    
    if (attempt >= maxAttempts) 
    {
     
    #ifdef neopixelUsed
     pixels.setPixelColor(led2, pixels.Color(0,50,0)); //Turn LED RED
     pixels.show();
    #endif

   #ifdef ledUsed
    digitalWrite(rojo_temp_pin, HIGH);
   #endif
   
   digitalWrite(pump, LOW);  
   if(debug)Serial.println("NOWATER");
   #ifdef lowPowerUsed
   LowPower.powerDown(SLEEP_4S, ADC_OFF, BOD_OFF); //pone el Arduino a dormir
   #endif
    }
  
    //------------------Fin testeo Humedad de la tierra----//
    
    
    //------------------ Testea la Cantidad de Luz--------------//
    
   if (millis() - ldrTest > 60000UL)  // Sondeamos la luz cada 1 min
 
      {
      detector_luz();
      ldrTest = millis();
      }
      //-------------------Fin testeo cantidad de luz------//
      
       //----------------Testea la Temperatura------------//

    #ifdef tempUsed
    if (millis() - sampleTemp > 120000UL)  // Sondeamos la temperatura ambiente cada 2 minutos
      {
      SamplingTemp();
      sampleTemp = millis();
      }
    
      //------------------Fin testeo Temperatura----//
    #endif
      
        // Chequeo si el settboton fue presionado//
     if (digitalRead(settButton)==!settButtonState)   
    {
     
      #ifdef neopixelUsed
      pixels.setPixelColor(led2, pixels.Color(0,0,0)); //Turn LED off
      pixels.show();
      #endif

    #ifdef ledUsed
      digitalWrite(rojo_temp_pin, LOW);
     
    #endif
     attempt=0;
     settButtonState=!settButtonState;
    }
    }
  
    
    void moistureSampling () { // Rutina para sondear la humedad de la tierra.
    digitalWrite(EnSensor,HIGH); //activa el sensor para lectura durante 300 mili
    delay(300);
    SoilSensor = analogRead(soilPin); // Leemos sensor de suelo, entrada analogica pin A3:
    SoilSensor = map (SoilSensor, 1023, 0, 0, 100); // --> Soil--Mapeamos el valor del sensor de 0 a 100.
         
         //---------- Muestra los datos en la com serial -------//
         
    if(debug)Serial.print(SoilSensor);
    if(debug)Serial.println(" % humedad"); 
    if (SoilSensor < soilPlanta && Ldr > ldrVal ) // Si el sensor detecta menos del valor en setup y es de dia 
      {
    // digitalWrite(EnSensor,LOW);
    water();
    attempt++;
    waterButtonState=!waterButtonState;
    
    if(debug)Serial.print(attempt);
    if(debug)Serial.println(" Intentos de Riego");
    
    
      }
    if (SoilSensor > (soilPlanta + histeresisH))// Si detecta mas del valor plus histeresis
      {
       
    #ifdef neopixelUsed
      pixels.setPixelColor(led1, pixels.Color(10,0,0)); //Turn LED Verde
      pixels.show();
    #endif
  
    #ifdef ledUsed
      digitalWrite(amarillo_seco_pin, LOW);
      digitalWrite(verde_mojado_pin, HIGH);
    #endif 
    }
            
   digitalWrite(EnSensor,LOW);
   
  }
   void water () {
     //delay (250);
    digitalWrite(pump, HIGH);
   if(debug)Serial.println("Regando");
     
   #ifdef ledUsed
    digitalWrite(amarillo_seco_pin, HIGH);
    digitalWrite(verde_mojado_pin, LOW);
   #endif  
     
   #ifdef neopixelUsed
     pixels.setPixelColor(led1, pixels.Color(0,0,50)); //Turn LED ambar
     pixels.show();
   #endif
     
   delay(waterTime);
   digitalWrite(pump, LOW);
   
   if(debug)Serial.println("Regado Finalizado");
    
   #ifdef ledUsed
    digitalWrite(amarillo_seco_pin, LOW);
   #endif
     
   #ifdef neopixelUsed
     pixels.setPixelColor(led1, pixels.Color(0,0,0)); //Turn LED OFF  
     pixels.show();
   #endif
     
   waterButtonState=!waterButtonState;
   
   }
  
   
   
   void detector_luz () { // Rutina para scanear la intensidad de Luz
      
   Ldr = analogRead(ldrPin);
   Ldr = map (Ldr, 0, 1023, 100, 0);             // Mapeamos el valor del sensor de 0 a 100.
   if (Ldr < ldrVal)
     
  {
    if(debug)Serial.println("NOCHE NO Riega");
     
   #ifdef ledUsed
    digitalWrite(verde_mojado_pin, HIGH);
    delay(10);
    digitalWrite(verde_mojado_pin, LOW);
    #endif

    #ifdef neopixelUsed
     pixels.setPixelColor(led2, pixels.Color(10,10,0)); //Turn LED 
     pixels.show();
     delay(10);
     pixels.setPixelColor(led2, pixels.Color(0,0,0)); //Turn LED 
     pixels.show();
   #endif
    
    }
    
   if (Ldr > (ldrVal - histeresisL))
    
    {
   
   #ifdef ledUsed
   //digitalWrite(rojo_temp_pin, LOW);
   #endif
    
    }
    
   if(debug)Serial.print(Ldr);
   if(debug)Serial.println(" lux");
    
    }
   #ifdef tempUsed
   void SamplingTemp () { // Rutina para scanear la temperatura
      
   TempC = tempsensor.getTemp(CELCIUS);
      
   if (TempC > 27)
      {
   #ifdef ledUsed
    digitalWrite(rojo_temp_pin, HIGH);
    delay(50);
    digitalWrite(rojo_temp_pin, LOW);
        //digitalWrite(fan, HIGH);
   #endif
    
   #ifdef servoWaterUsed
    digitalWrite(EnServo, HIGH);
    waterServo.write(70);
    delay(1000);
    digitalWrite(EnServo, LOW);
   #endif
        
      }
    
   if (TempC < (27 - histeresisT))
      {
   #ifdef ledUsed
    digitalWrite(rojo_temp_pin, LOW);
        //digitalWrite(fan, LOW);
   #endif 

   #ifdef servoWaterUsed
    digitalWrite(EnServo, HIGH);
    waterServo.write(0);
    delay(1000);
    digitalWrite(EnServo, LOW);
   #endif
      }
   if(debug)Serial.print(TempC);             // Print the temperature in the Serial Monitor:
   if(debug)Serial.print(" \xC2\xB0");       // shows degree symbol
   if(debug)Serial.println("C");
    }
   #endif 
 
