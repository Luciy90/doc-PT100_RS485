#include <ModbusMaster.h>

HardwareSerial ModbusSerial(2);
ModbusMaster node;

const int NEW_ADDRESS = 2; // Новый адресс назначение
const int OLD_ADDRESS = 1; // Убедитесь, что модуль сейчас на этом адресе
const int DE_RE_PIN = 14;

void preTransmission()  { digitalWrite(DE_RE_PIN, HIGH); }
void postTransmission() { digitalWrite(DE_RE_PIN, LOW); }

void setup() {
  Serial.begin(115200);
  delay(2000); // Даем время открыть монитор порта
  
  pinMode(DE_RE_PIN, OUTPUT);
  digitalWrite(DE_RE_PIN, LOW);
  
  ModbusSerial.begin(9600, SERIAL_8N1, 16, 13);
  node.begin(OLD_ADDRESS, ModbusSerial);
  node.preTransmission(preTransmission);
  node.postTransmission(postTransmission);

  Serial.println("--- Старт теста записи ---");
  
  // Читаем значение ДО попытки записи (для проверки связи)
  uint8_t res = node.readHoldingRegisters(0x0100, 1);
  if (res == node.ku8MBSuccess) {
    Serial.println("Связь установлена, датчик отвечает.");
  } else {
    Serial.printf("КРИТИЧЕСКАЯ ОШИБКА: Нет связи с модулем! Код: 0x%02X\n", res);
    return;
  }

  // Пытаемся записать
  Serial.printf("Записываем %d в регистр 0x0002...\n", NEW_ADDRESS);
  uint8_t result = node.writeSingleRegister(0x0002, NEW_ADDRESS);
  
  // ПРИНУДИТЕЛЬНЫЙ ВЫВОД РЕЗУЛЬТАТА
  Serial.printf("Функция записи вернула код: 0x%02X\n", result);
  
  if (result == node.ku8MBSuccess) {
    Serial.println("УСПЕХ! Адрес должен был смениться.");
  } else {
    Serial.println("НЕУДАЧА! Ветка УСПЕХ не сработала.");
    if (result == 0xE2) Serial.println("Причина: 0xE2 (Таймаут/Нет ответа).");
    if (result == 0x02) Serial.println("Причина: 0x02 (Ошибка адреса регистра).");
    if (result == 0x03) Serial.println("Причина: 0x03 (Ошибка значения данных).");
  }
}

void loop() {}
