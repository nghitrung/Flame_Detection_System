#include "main.c"
#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char* topic, byte* payload, unsigned int length) {
    String message = "";
    for (int i = 0; i < length; i++) message += (char)payload[i];

    Serial.printf("Message arrived [%s]: %s\n", topic, message.c_str());

    if (String(topic) == TOPIC_ALARM) {
        if (message == "ON") {
            Serial.println(">>> ĐANG BẬT BÁO ĐỘNG!");
            // digitalWrite(PIN_BUZZER, HIGH); 
        } else {
            Serial.println(">>> TẮT BÁO ĐỘNG.");
            // digitalWrite(PIN_BUZZER, LOW);
        }
    }
}

// --- Hàm kết nối WiFi ---
void setup_wifi() {
    Serial.print("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500); Serial.print(".");
    }
    Serial.println("\nWiFi Connected. IP: " + WiFi.localIP().toString());
}

// --- Hàm kết nối/Kết nối lại MQTT ---
void reconnect() {
    while (!client.connected()) {
        Serial.print("Attempting MQTT connection...");
        // Tạo Client ID duy nhất cho ESP32-S3
        String clientId = "YoloUno-Client-" + String(random(0xffff), HEX);
        
        if (client.connect(clientId.c_str())) {
            Serial.println("CONNECTED to Mosquitto");
            // Đăng ký nhận lệnh từ topic báo động
            client.subscribe(TOPIC_ALARM);
        } else {
            Serial.print("FAILED, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 5 seconds");
            delay(5000);
        }
    }
}

void setup() {
    Serial.begin(115200);
    setup_wifi();
    
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}

void loop() {
    if (!client.connected()) {
        reconnect();
    }
    client.loop(); 

    static unsigned long lastMsg = 0;
    if (millis() - lastMsg > 5000) {
        lastMsg = millis();
        
        int flameValue = digitalRead(Smoke); 
        
        String payload = (flameValue == LOW) ? "FIRE_DETECTED" : "SAFE";
        Serial.print("Publishing: "); Serial.println(payload);
        
        // Gửi dữ liệu lên Broker
        client.publish(TOPIC_FLAME, payload.c_str());
    }
}