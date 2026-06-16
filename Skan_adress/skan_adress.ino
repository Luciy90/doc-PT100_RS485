#include <ModbusMaster.h>

HardwareSerial ModbusSerial(2);
ModbusMaster node;

const int DE_RE_PIN = 14;
const int RX_PIN    = 16;
const int TX_PIN    = 13;

void preTransmission()  { digitalWrite(DE_RE_PIN, HIGH); delayMicroseconds(50); }
void postTransmission() { delayMicroseconds(50); digitalWrite(DE_RE_PIN, LOW); }

void setup() {
  Serial.begin(115200);
  delay(1000);
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);
  ModbusSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);
}

void loop() {
  bool found = false;
  for (uint8_t addr = 1; addr <= 5; addr++) {
    node.begin(addr, ModbusSerial);
    uint8_t res = node.readHoldingRegisters(0x0100, 1);
    if (res == ModbusMaster::ku8MBSuccess) {
      int16_t raw = (int16_t)node.getResponseBuffer(0);
      Serial.printf("==> НАЙДЕН: addr=%d temp=%.1f C\n", addr, raw * 0.1f);
      found = true;
    }
    delay(50);
  }
  if (!found) Serial.println("Не найден (1-5)");
  delay(2000);
}
