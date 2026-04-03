FROM eclipse-mosquitto:latest

# Bưng trọn ổ config (có cả pwfile) từ thư mục mqtt con vào container
COPY ./mosquitto/config/ /mosquitto/config/

RUN mosquitto_passwd -U /mosquitto/config/pwfile

RUN chown -R mosquitto:mosquitto /mosquitto
