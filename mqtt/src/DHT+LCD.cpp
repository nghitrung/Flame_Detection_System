#include "DHT+LCD.h"

DHT20 dht20;
LiquidCrystal_I2C lcd(0x21, 16, 2);

static bool isI2cDeviceReady(uint8_t address) {
  Wire.beginTransmission(address);
  return Wire.endTransmission() == 0;
}

static void scanI2cBus() {
  Serial.print("[DHT] I2C devices:");
  bool foundAny = false;
  for (uint8_t addr = 0x03; addr <= 0x77; ++addr) {
    if (isI2cDeviceReady(addr)) {
      Serial.printf(" 0x%02X", addr);
      foundAny = true;
    }
  }
  if (!foundAny) {
    Serial.print(" none");
  }
  Serial.println();
}

void temp_monitor(void *pvParameters) {
  Serial.printf("\n[DHT] Task start (SDA=%d, SCL=%d)\n", SDA_PIN, SCL_PIN);
  if ((SDA_PIN == FLAME1_ANAPIN) || (SDA_PIN == FLAME2_ANAPIN) ||
      (SCL_PIN == FLAME1_ANAPIN) || (SCL_PIN == FLAME2_ANAPIN)) {
    Serial.println("[DHT] WARNING: I2C pins overlap flame analog pins.");
  }

  Wire.begin(SDA_PIN, SCL_PIN);
  Wire.setClock(100000);
  vTaskDelay(pdMS_TO_TICKS(100));
  scanI2cBus();
  bool dhtReady = isI2cDeviceReady(0x38);
  if (dhtReady) {
    dht20.begin();
    Serial.println("[DHT] DHT20 detected at 0x38");
  } else {
    Serial.println("[DHT] DHT20 not found at 0x38");
  }

  bool lcdReady = false;
  if (isI2cDeviceReady(0x21)) {
    lcd.init();
    lcd.backlight();
    lcdReady = true;
    Serial.println("[DHT] LCD detected at 0x21");
  } else {
    Serial.println("[DHT] LCD not found at 0x21. Serial logging only.");
  }

  char lineBuf[32];
  while(1) {
    if (!isI2cDeviceReady(0x38)) {
      glob_temperature = -1.0f;
      Serial.println("[DHT] No I2C response from DHT20 (0x38)");
      vTaskDelay(pdMS_TO_TICKS(2000));
      continue;
    }

    if (!dhtReady) {
      dht20.begin();
      dhtReady = true;
      Serial.println("[DHT] DHT20 reinitialized");
    }

    dht20.read();
    float temperature = dht20.getTemperature();
    if (isnan(temperature)) {
      temperature = -1;
    }

    glob_temperature = temperature;

    Serial.printf("[DHT] Temp: %.1f C\n", temperature);

    if (lcdReady) {
      lcd.clear();
      lcd.setCursor(0, 0);
      snprintf(lineBuf, sizeof(lineBuf), "Temp: %4.1f C", temperature);
      lcd.print(lineBuf);
    }

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}