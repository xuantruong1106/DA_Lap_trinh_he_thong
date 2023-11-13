# Sử dụng một hình ảnh cơ bản của hệ điều hành
FROM ubuntu:latest

# Sao chép mã nguồn C vào hình ảnh
COPY ./main.c /app/

# Thiết lập thư mục làm việc
WORKDIR /app

# Biên dịch chương trình
RUN apt-get update && \
    apt-get install -y gcc && \
    gcc -o my_program main.c

# Mặc định lệnh khi chạy container
CMD ["./my_program"]
