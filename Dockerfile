FROM ubuntu:16.04

# Install
RUN apt-get update -y
RUN apt-get install -y apt-utils | true
RUN apt-get install -y software-properties-common | true
RUN apt-get install -y python-software-properties
RUN apt-get update -y
RUN add-apt-repository -y ppa:jonathonf/gcc-7.1
RUN apt-get update -y 
RUN apt-get install -y cmake software-properties-common git make
RUN apt-get install -y gcc-7 g++-7 
RUN update-alternatives --install /usr/bin/gcc gcc /usr/bin/gcc-7 90
RUN update-alternatives --install /usr/bin/g++ g++ /usr/bin/g++-7 90 
RUN apt-get install -y unzip zlib1g-dev
RUN apt-get install -y libboost-all-dev

# Build Preparation
RUN mkdir -p /src/
RUN mkdir -p /src/build/

# Build
WORKDIR /src/
COPY . /src/
RUN cd /src


RUN git clone https://github.com/KjellKod/q.git -b setup
RUN cd /src/q/3rdParty/gtest && unzip gtest-1.7.0.zip
RUN cd /src/q && mkdir build
RUN cd /src/q/build && cmake .. && make -j && && ./UnitTestRunner
