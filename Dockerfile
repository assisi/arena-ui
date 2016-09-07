FROM ubuntu:16.04
MAINTAINER Mirko Kokot

## essential
RUN ln -snf /bin/bash /bin/sh
RUN apt-get update
RUN apt-get -y install make build-essential

## dependencies
RUN apt-get -y install qt5-default \
	qt5-qmake \
	libqt5gstreamer-dev \
	libzmq3-dev \
	libprotobuf-dev \
	protobuf-compiler \
	libyaml-dev \
	libyaml-cpp-dev \
	libboost-dev

COPY . /root/assisi-ui

## build
RUN cd /root/assisi-ui; sh compile_msgs.sh
RUN cd /root/assisi-ui; qmake
RUN cd /root/assisi-ui; make -j12
