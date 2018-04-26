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
RUN mkdir -p /build/

# Build
WORKDIR /build/
COPY . /build/
RUN ls /build
RUN cd /build/Q && git branch
RUN echo "branch is: $BRANCH"
RUN cd /build && git clone -b "$BRANCH" https://github.com/KjellKod/Q.git  && ls -alh
RUN cd /build/Q/3rdparty/ && unzip gtest-1.7.0.zip
RUN cd /build/Q && mkdir build
RUN cd /build/Q/build && cmake .. && make -j && ./UnitTestRunner
