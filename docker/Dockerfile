FROM ubuntu:20.04

# Отключаем интерактивные запросы при установке пакетов
ENV DEBIAN_FRONTEND=noninteractive

# Устанавливаем только необходимые пакеты одной командой
RUN apt-get update && apt-get install -y --no-install-recommends \
    build-essential \
    qt5-default \
    qtbase5-dev \
    libqt5sql5-sqlite \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Создание рабочей директории
WORKDIR /app

# Сначала копируем только файлы .pro для установки зависимостей
COPY server/server.pro /app/server/
COPY common/common.pro /app/common/
COPY .qmake.conf /app/

# Копируем общие компоненты
COPY common/ /app/common/

# Копируем только серверную часть
COPY server/ /app/server/

# Сначала собираем общую библиотеку, а затем сервер
RUN mkdir -p build/common && \
    cd build/common && \
    qmake ../../common/common.pro && \
    make -j$(nproc) && \
    cd .. && \
    mkdir -p server && \
    cd server && \
    qmake ../../server/server.pro && \
    make -j$(nproc)

# Открываем порт для сервера
EXPOSE 12345

# Запуск сервера при старте контейнера
CMD ["/app/build/server/server"] 