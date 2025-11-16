// | Adelante             | BTN3        | 0001           | 1              |
// | Atrás                | BTN4        | 0010           | 2              |
// | Izquierda            | BTN1        | 0011           | 3              |
// | Derecha              | BTN2        | 0100           | 4              |
// | Adelante + Derecha   | BTN3 + BTN2 | 0101           | 5              |
// | Adelante + Izquierda | BTN3 + BTN1 | 0110           | 6              |
// | Atrás + Derecha      | BTN4 + BTN2 | 0111           | 7              |
// | Atrás + Izquierda    | BTN4 + BTN1 | 1000           | 8              |

// Pines de LEDs
int ledPin1 = 9;
int ledPin2 = 8;
int ledPin3 = 7;
int ledPin4 = 6;

// Pines de botones
int btn1 = 5;  // Izquierda
int btn2 = 4;  // Derecha
int btn3 = 3;  // Adelante
int btn4 = 2;  // Atrás

// Variable para almacenar el valor binario acumulado
int ledState = 0;

void setup() {
  // Configurar LEDs como salida
  pinMode(ledPin1, OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(ledPin3, OUTPUT);
  pinMode(ledPin4, OUTPUT);

  // Configurar botones como entrada con pull-up
  pinMode(btn1, INPUT_PULLUP);
  pinMode(btn2, INPUT_PULLUP);
  pinMode(btn3, INPUT_PULLUP);
  pinMode(btn4, INPUT_PULLUP);

  Serial.begin(9600); // Comunicación serial
}

void loop() {
  // Leer el estado de cada botón
  int state1 = digitalRead(btn1);
  int state2 = digitalRead(btn2);
  int state3 = digitalRead(btn3);
  int state4 = digitalRead(btn4);

  // Inicializamos ledState a 0 para comenzar
  ledState = 0;

  // Combinaciones de botones con sus valores binarios
  if (state3 == LOW && state2 == HIGH && state1 == HIGH && state4 == HIGH) { // Adelante
    ledState = 0b0001;  // Activar el bit 1
  }
  if (state4 == LOW && state3 == HIGH && state2 == HIGH && state1 == HIGH) { // Atrás
    ledState = 0b0010;  // Activar el bit 2
  }
  if (state1 == LOW && state3 == HIGH && state2 == HIGH && state4 == HIGH) { // Izquierda
    ledState = 0b0011;  // Activar los bits 1 y 2
  }
  if (state2 == LOW && state3 == HIGH && state1 == HIGH && state4 == HIGH) { // Derecha
    ledState = 0b0100;  // Activar el bit 3
  }
  if (state3 == LOW && state2 == LOW && state1 == HIGH && state4 == HIGH) { // Adelante + Derecha
    ledState = 0b0101;  // Activar los bits 1 y 3
  }
  if (state3 == LOW && state2 == HIGH && state1 == LOW && state4 == HIGH) { // Adelante + Izquierda
    ledState = 0b0110;  // Activar los bits 1, 2 y 3
  }
  if (state4 == LOW && state2 == LOW && state1 == HIGH && state3 == HIGH) { // Atrás + Derecha
    ledState = 0b0111;  // Activar los bits 1, 2 y 3
  }
  if (state4 == LOW && state2 == HIGH && state1 == LOW && state3 == HIGH) { // Atrás + Izquierda
    ledState = 0b1000;  // Activar el bit 4
  }

  // Si ningún botón está presionado, resetear a 0000
  if (state1 == HIGH && state2 == HIGH && state3 == HIGH && state4 == HIGH) {
    ledState = 0; // Apagar todos los LEDs
  }

  // Controlar los LEDs de acuerdo con el valor binario
  digitalWrite(ledPin1, (ledState & 0b0001) ? HIGH : LOW);   // LED1 si el bit menos significativo es 1
  digitalWrite(ledPin2, (ledState & 0b0010) ? HIGH : LOW);   // LED2 si el segundo bit es 1
  digitalWrite(ledPin3, (ledState & 0b0100) ? HIGH : LOW);   // LED3 si el tercer bit es 1
  digitalWrite(ledPin4, (ledState & 0b1000) ? HIGH : LOW);   // LED4 si el cuarto bit es 1

  // Enviar señales al Serial (1 = encendido, 0 = apagado)
  Serial.print((ledState & 0b1000) ? 1 : 0);  // Bit 4
  Serial.print(",");
  Serial.print((ledState & 0b0100) ? 1 : 0);  // Bit 3
  Serial.print(",");
  Serial.print((ledState & 0b0010) ? 1 : 0);  // Bit 2
  Serial.print(",");
  Serial.println((ledState & 0b0001) ? 1 : 0);  // Bit 1

  delay(100); // Retardo pequeño para estabilidad
}
