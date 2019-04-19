FROM ubuntu:19.04

ENV DEBIAN_FRONTEND noninteractive
ENV CMAKE_BUILD_PARALLEL_LEVEL 4

RUN apt-get update
RUN apt-get install -y apt-utils
RUN apt-get install -y build-essential
RUN apt-get install -y git
RUN apt-get install -y sudo pkg-config
RUN apt-get install -y cmake
RUN apt-get install -y libboost1.67-all-dev
RUN apt-get install -y openssl libssl-dev
RUN apt-get install -y libmsgpack-dev

RUN mkdir build
COPY . /service
WORKDIR /build
RUN cmake /service
RUN make

EXPOSE 3388
ENTRYPOINT ["/build/beast-http"]
