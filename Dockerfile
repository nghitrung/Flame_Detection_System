FROM eclipse-mosquitto:latest

# 1. Chép config vào (trong này có thể chứa pwfile cũ)
COPY ./mosquitto/config/ /mosquitto/config/

# 2. XÓA file pwfile cũ nếu nó lỡ tồn tại để lệnh dưới không bị lỗi
RUN rm -f /mosquitto/config/pwfile

# 3. Tự động tạo và băm mật khẩu chuẩn xác
RUN mosquitto_passwd -c -b /mosquitto/config/pwfile Flame_Detection_System 123

# 4. Cấp quyền
RUN chown -R mosquitto:mosquitto /mosquitto