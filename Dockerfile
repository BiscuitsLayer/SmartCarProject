FROM ubuntu:24.04 AS build

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        build-essential \
        cmake \
        libavcodec-dev \
        libavformat-dev \
        libavutil-dev \
        libfreetype-dev \
        libgl1-mesa-dev \
        libswscale-dev \
        libx11-dev \
        libxrandr-dev \
        pkg-config \
        zlib1g-dev \
    && rm -rf /var/lib/apt/lists/*

COPY . /src
RUN cmake -S /src -B /build \
        -DCMAKE_BUILD_TYPE=Release \
        -DBUILD_SHARED_LIBS=OFF \
        -DASSIMP_BUILD_ASSIMP_TOOLS=OFF \
        -DASSIMP_BUILD_SAMPLES=OFF \
        -DASSIMP_INSTALL=OFF \
    && cmake --build /build --parallel 2

FROM ubuntu:24.04

RUN apt-get update && DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
        fonts-dejavu-core \
        libavcodec60 \
        libavformat60 \
        libavutil58 \
        libfreetype6 \
        libgl1 \
        libopengl0 \
        libswscale7 \
        libx11-6 \
        libxrandr2 \
        novnc \
        websockify \
        x11vnc \
        x11-utils \
        xdotool \
        xvfb \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app/build
COPY --from=build /build/SmartCarMain .
COPY assets /app/assets
COPY configs /app/configs
COPY shader /app/shader
COPY config.json /app/config.json
COPY docker/entrypoint.sh /usr/local/bin/smartcar

EXPOSE 6080
ENTRYPOINT ["smartcar"]
