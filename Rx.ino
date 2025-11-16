const int D0 = 10;
const int D1 = 3;
const int D2 = 2;
const int D3 = 4;

const int M1 = 9;  // Izquierda
const int M2 = 8;  // Derecha
const int M3 = 7;  // Adelante
const int M4 = 6;  // Atrás

const bool INPUT_ACTIVE_HIGH = true;

const unsigned long SAMPLE_MS     = 40; // intervalo de muestreo
const uint8_t       BUF_SIZE      = 3;  // 3 muestras para mayoría
const unsigned long APPLY_HYST_MS = 80; // estabilidad mínima antes de aceptar cambio

uint8_t buf[BUF_SIZE] = {0};
uint8_t bufIdx = 0;
unsigned long lastSampleMs = 0;

uint8_t lastAcceptedCode = 0xFF;
unsigned long lastAcceptedMs = 0;

void setup() {
  Serial.begin(115200);

  // ENTRADAS: pull-down externos -> INPUT 
  pinMode(D0, INPUT);
  pinMode(D1, INPUT);
  pinMode(D2, INPUT);
  pinMode(D3, INPUT);

  // SALIDAS a motores
  pinMode(M1, OUTPUT);
  pinMode(M2, OUTPUT);
  pinMode(M3, OUTPUT);
  pinMode(M4, OUTPUT);

  allMotorsOff();
  Serial.println("RX listo (INPUT con pull-down externos, HIGH=1)");
}

void allMotorsOff() {
  digitalWrite(M1, LOW);
  digitalWrite(M2, LOW);
  digitalWrite(M3, LOW);
  digitalWrite(M4, LOW);
}

uint8_t readCodeInstant() {
  int b0 = digitalRead(D0);
  int b1 = digitalRead(D1);
  int b2 = digitalRead(D2);
  int b3 = digitalRead(D3);
  if (!INPUT_ACTIVE_HIGH) { b0 = !b0; b1 = !b1; b2 = !b2; b3 = !b3; }
  return (uint8_t)((b3 << 3) | (b2 << 2) | (b1 << 1) | (b0));
}

uint8_t majorityOfBuffer() {
  int counts[16] = {0};
  for (uint8_t i = 0; i < BUF_SIZE; ++i) counts[ buf[i] & 0x0F ]++;
  for (int v = 0; v < 16; ++v) if (counts[v] >= 2) return (uint8_t)v;
  return 0xFF; // sin mayoritario
}

void printCode(uint8_t code) {
  Serial.print("Patron RX = ");
  Serial.print((code >> 3) & 1);
  Serial.print((code >> 2) & 1);
  Serial.print((code >> 1) & 1);
  Serial.print((code >> 0) & 1);
  Serial.print(" (dec="); Serial.print(code); Serial.println(")");
}

void applyMapping(uint8_t code) {
  allMotorsOff();
  switch (code) {
    case 0b0001: digitalWrite(M3, HIGH); Serial.println("→ Adelante (M3)"); break;
    case 0b0010: digitalWrite(M4, HIGH); Serial.println("→ Atrás (M4)"); break;
    case 0b0011: digitalWrite(M1, HIGH); Serial.println("→ Izquierda (M1)"); break;
    case 0b0100: digitalWrite(M2, HIGH); Serial.println("→ Derecha (M2)"); break;
    case 0b0101: digitalWrite(M3, HIGH); digitalWrite(M2, HIGH); Serial.println("→ Adelante + Derecha (M3,M2)"); break;
    case 0b0110: digitalWrite(M3, HIGH); digitalWrite(M1, HIGH); Serial.println("→ Adelante + Izquierda (M3,M1)"); break;
    case 0b0111: digitalWrite(M4, HIGH); digitalWrite(M2, HIGH); Serial.println("→ Atrás + Derecha (M4,M2)"); break;
    case 0b1000: digitalWrite(M4, HIGH); digitalWrite(M1, HIGH); Serial.println("→ Atrás + Izquierda (M4,M1)"); break;
    case 0b0000: allMotorsOff(); Serial.println("→ 0000 recibido (motores OFF)"); break;
    default: Serial.println("→ Ninguna combinación válida: motores OFF"); break;
  }
}

void loop() {
  unsigned long now = millis();
  if (now - lastSampleMs >= SAMPLE_MS) {
    lastSampleMs = now;

    uint8_t code = readCodeInstant();
    buf[bufIdx] = code;
    bufIdx = (uint8_t)((bufIdx + 1) % BUF_SIZE);

    uint8_t maj = majorityOfBuffer();
    if (maj != 0xFF) {
      if (maj != lastAcceptedCode) {
        if ((millis() - lastAcceptedMs) >= APPLY_HYST_MS || lastAcceptedCode == 0xFF) {
          lastAcceptedCode = maj;
          lastAcceptedMs = millis();
          Serial.print("Cambio aceptado por mayoria: ");
          printCode(maj);
          applyMapping(maj);
        }
      } else {
        lastAcceptedMs = millis(); // refresco mientras se mantiene estable
      }
    } else {
      // Sin mayoría -> mantener estado (evita "apagones" por ruido)
    }
  }
}
