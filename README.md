# GreenNito 
#### Descripción
Este código controla un sistema de riego y monitoreo de temperatura utilizando un sensor LM35 y varios componentes opcionales como servos y LEDs. El sistema también puede entrar en modo de bajo consumo utilizando la biblioteca `LowPower`.

### Instrucciones de Uso

1. **Configuración de Hardware**:
   - Conecte el sensor LM35 al pin A0.
   - Conecte los LEDs a los pines digitales 10, 11 y 12.
   - Conecte la bomba de agua al pin digital 2.
   - Conecte el ventilador al pin digital 13.
   - Configure los pines para los botones de configuración y riego en los pines digitales 8 y 9 respectivamente.
   - Opcionalmente, puede conectar un servo al pin A5.

2. **Librerías Necesarias**:
   - `LowPower.h`: para el modo de bajo consumo.
   - `LM35.h`: para el sensor de temperatura.

3. **Configuración del Código**:
   - Comente o descomente las definiciones en la parte superior del código para habilitar o deshabilitar componentes según su configuración.
   - Ajuste las variables de histéresis y otros parámetros según sus necesidades.

4. **Carga y Ejecución**:
   - Suba el código a su placa Arduino.
   - Monitoree los valores y el estado del sistema a través del monitor serie si la depuración está habilitada (`debug`).
