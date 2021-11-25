#FROM ubuntu:20.04
FROM mongo:latest

RUN apt-get update -y
RUN apt-get upgrade -ys

RUN apt-get install build-essential -y
RUN apt-get install manpages-dev -y

RUN DEBIAN_FRONTEND="noninteractive" apt-get -y install tzdata

RUN apt-get install -y pkg-config

RUN apt-get install libmongoc-dev -yq
RUN apt-get install libbson-dev -yq
RUN apt-get install libbson-1.0-0 -yq

RUN mkdir /code

COPY ./src /code/ 

RUN gcc -g -o /code/main /code/main.c $(pkg-config --libs --cflags libmongoc-1.0) -lm

#RUN sudo ystemctl start mongod
#CMD /code/main

EXPOSE 27017
