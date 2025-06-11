TEMPLATE = subdirs

# Указываем директорию сборки
CONFIG += shadow_build
SHADOW_BUILD_DIR = $$PWD/build

SUBDIRS += \
    client \
    server \
    common

client.depends = common
server.depends = common

OTHER_FILES += \
    README.md \
    docker/Dockerfile \
    docker/docker-compose.yml 