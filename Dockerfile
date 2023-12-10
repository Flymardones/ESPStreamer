FROM ubuntu:22.04 AS esp_base

ENV DEBIAN_FRONTEND=noninteractive

# Use /bin/bash instead of /bin/sh
SHELL ["/bin/bash", "-c"]

RUN echo "Installing necessary prerequisites for ESP-IDF Toolchain..."

RUN apt-get update && apt-get install -y --no-install-recommends \
    git \
    wget \
    flex \ 
    bison \
    gperf \
    python3 \ 
    python3-pip \
    python3-venv \
    cmake \
    ninja-build \
    ccache \
    libffi-dev \
    libssl-dev \
    dfu-util \
    libusb-1.0-0 \
    pigz


RUN echo "Cloning ESP-IDF repo..."
    
RUN mkdir -p ~/esp && \
    cd ~/esp && \
    git clone -b v5.1.1 --recursive https://github.com/espressif/esp-idf.git esp-idf

RUN echo "Setting up the tools..."

RUN cd ~/esp/esp-idf && \
    ./install.sh esp32
