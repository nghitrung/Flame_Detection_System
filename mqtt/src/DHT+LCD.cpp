#include "DHT+LCD.h"

DHT20 dht20;
LiquidCrystal_I2C lcd(0x21, 16, 2);

void temp_humi_monitor(void *pvParameters) {
  Wire.begin(SDA_PIN, SCL_PIN);
  dht20.begin();
  lcd.init();
  lcd.backlight();

  char lineBuf[32];

  while(1) {
    dht20.read();
    float temperature = dht20.getTemperature();
    float humidity    = dht20.getHumidity();

    if (isnan(temperature) || isnan(humidity)) {
      temperature = humidity = -1;
    }

    glob_temperature = temperature;
    glob_humidity    = humidity;

    Serial.printf("Humidity: %.1f%%  Temp: %.1f C\n", humidity, temperature);

    lcd.clear();
    lcd.setCursor(0, 0);
    snprintf(lineBuf, sizeof(lineBuf), "Temp: %4.1f C", temperature);
    lcd.print(lineBuf);
    lcd.setCursor(0, 1);
    snprintf(lineBuf, sizeof(lineBuf), "Humid: %4.1f %%", humidity);
    lcd.print(lineBuf);

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}