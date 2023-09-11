FROM ubuntu:latest

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    build-essential \
    cmake \
    git \
    software-properties-common

RUN add-apt-repository ppa:savoury1/qt-6-2 && apt-get update -y

RUN apt-get install -y \
    qt6-base-dev \
    qt6-pdf-dev

# WORKDIR /opt/qt
# RUN wget https://download.qt.io/archive/qt/6.3/6.3.2/single/qt-everywhere-src-6.3.2.tar.xz
# RUN tar xvf qt-everywhere-src-6.3.2.tar.xz
#
# WORKDIR /opt/qt/qt-everywhere-src-6.3.2
#
# RUN ./configure
# RUN make -j

# RUN chmod +x qt-opensource-linux-x64-6.3.2.run
# RUN ./qt-opensource-linux-x64-6.3.2.run


COPY ./droidforge /app

WORKDIR /app

RUN cmake -S . -B build
RUN cmake --build build
