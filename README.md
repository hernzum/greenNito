# GreenNito 
# Sistema de Riego Automático con Arduino

## Descripción

Este proyecto implementa un sistema de riego automático utilizando un Arduino. Incluye la lectura de sensores de humedad del suelo y temperatura, el control de una bomba de agua y un ventilador, y la indicación del estado mediante LEDs.

## Componentes

- Arduino (cualquier modelo compatible)
- Sensor de humedad del suelo
- Sensor de temperatura LM35
- Bomba de agua
- Ventilador
- LEDs (rojo, verde, amarillo)
- Botones (para configuración y riego manual)
- Servo (opcional)
- Sensor de luz LDR (opcional)
- Resistencias, cables, y una protoboard

## Conexiones

| Componente         | Pin de Arduino |
|--------------------|----------------|
| LED rojo           | 12             |
| LED verde          | 11             |
| LED amarillo       | 10             |
| Bomba de agua      | 2              |
| Ventilador         | 13             |
| Servo              | A5             |
| Sensor de humedad  | A3             |
| Sensor de luz LDR  | A1             |
| Sensor de temperatura LM35 | A0    |
| Botón de configuración | 8         |
| Botón de riego     | 9              |

## Instrucciones de Uso

1. **Conectar los Componentes**: Conecta todos los sensores y actuadores a los pines especificados en la tabla de conexiones.

2. **Configurar el Código**: Descomenta las líneas de definición (`#define`) según los componentes que estés utilizando:
   - `#define tempUsed` para usar el sensor de temperatura LM35.
   - `#define servoWaterUsed` para usar el servo.
   - `#define ledUsed` para usar los LEDs.
   - `#define lowPowerUsed` para usar el modo de bajo consumo.

3. **Subir el Código**: Sube el código al Arduino utilizando el IDE de Arduino.

4. **Depuración**: Si la depuración está habilitada (`const bool debug = 1;`), abre el monitor serie a 9600 baudios para ver las lecturas y estados del sistema.

5. **Funcionamiento**:
   - El sistema lee la humedad del suelo y la temperatura a intervalos regulares.
   - Si la humedad del suelo es baja y la luz ambiental es adecuada, se activa la bomba de agua.
   - El ventilador se activa si la temperatura supera un límite especificado.
   - Los LEDs indican el estado del sistema: rojo para temperatura alta, verde para suelo húmedo, y amarillo para suelo seco.
   - Los botones permiten configurar el sistema y activar el riego manualmente.

## Autor

- **Nito**

## Fecha

- 2024-07-05

## Versión

- 4.1
