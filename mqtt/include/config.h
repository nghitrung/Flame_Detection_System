#ifndef CONFIG_H
#define CONFIG_H

#define ssid "Thành's iPhone"
#define password "khongbiet"
// Use broker host/IP only (without ":port").
#define mqtt_server "172.20.10.2"
#define mqtt_port 1883
#define mqtt_user "Flame_Detection_System"
#define mqtt_password "123"

// === TOPICS ===
#define TOPIC_FLAME "yolo_uno/sensors/flame"
#define TOPIC_DHT20 "yolo_uno/sensors/dht20"
#define TOPIC_SMOKE "yolo_uno/sensors/smoke"
#define TOPIC_CONTROL "yolo_uno/control"
#define TOPIC_BUZZER
#define TOPIC_ALARM   
#define TOPIC_PUMP

#endif