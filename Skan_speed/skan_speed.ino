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
  delay(2000);
  Serial.println("\n=== ФИНАЛЬНЫЙ ШАГ: СБРОС ОФСЕТА И ВОЗВРАТ НА 9600 ===");

  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  // 1. Подключаемся на текущей скорости 1200 baud
  Serial.println("Подключение на 1200 baud...");
  ModbusSerial.begin(1200, SERIAL_8N1, RX_PIN, TX_PIN);
  node.begin(1, ModbusSerial);
  delay(500);

  if (node.readHoldingRegisters(0x0100, 1) == ModbusMaster::ku8MBSuccess) {
    float temp = node.getResponseBuffer(0) / 10.0;
    Serial.printf("Модуль на связи! Текущая температура: %.1f C\n", temp);
    
    // 2. Сбрасываем регистр 0x0004 (и пару соседей для страховки)
    Serial.println("Очищаем калибровочный офсет...");
    uint16_t offsetRegs[] = {0x0004, 0x0065, 0x00FE, 0x0200};
    for (auto reg : offsetRegs) {
      node.writeSingleRegister(reg, 0);
      delay(100);
    }

    // Проверяем температуру снова
    if (node.readHoldingRegisters(0x0100, 1) == ModbusMaster::ku8MBSuccess) {
      temp = node.getResponseBuffer(0) / 10.0;
      Serial.printf("Температура после очистки: %.1f C\n", temp);
    }

    // 3. Возвращаем скорость на 9600 (пробуем код 3)
    Serial.println("Отправляем код '3' в регистр скорости (0x0003)...");
    node.writeSingleRegister(0x0003, 3);
    delay(200);

    // 4. Переключаем ESP32 и проверяем 9600
    Serial.println("Переключаем порт на 9600 baud...");
    ModbusSerial.end();
    ModbusSerial.begin(9600, SERIAL_8N1, RX_PIN, TX_PIN);
    node.begin(1, ModbusSerial);
    delay(500);

    if (node.readHoldingRegisters(0x0100, 1) == ModbusMaster::ku8MBSuccess) {
      temp = node.getResponseBuffer(0) / 10.0;
      Serial.printf("\n>>> ПОБЕДА! Модуль на 9600 baud. Температура: %.1f C <<<\n", temp);
      Serial.println("Для полной гарантии передерните питание модулю и убедитесь, что он остался на 9600.");
    } else {
      Serial.println("\nНа 9600 не отвечает. Возможно, для 9600 нужен код '4'.");
      Serial.println("Запустите 'Глубокое Сканирование' еще раз, чтобы узнать, на какую скорость его перебросил код '3'.");
    }
  } else {
    Serial.println("Ошибка: Модуль не найден на 1200 baud. Проверьте провода.");
  }
}

void loop() {}
