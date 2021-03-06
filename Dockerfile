FROM gcc:8.4.0 AS stage

ENV DEBIAN_FRONTEND noninteractive

RUN apt-get update && apt-get install -y cmake libgtest-dev libboost-test-dev libjellyfish-2.0-dev libboost-iostreams-dev libboost-system-dev libstdc++6 && rm -rf /var/lib/apt/lists/* 

RUN mkdir /app

WORKDIR /app

COPY . .

RUN cmake . || echo 0

RUN make

RUN mkdir /workspace/

RUN adduser -q workflow

RUN chown workflow /workspace/

WORKDIR /workflow/

USER workflow



ENTRYPOINT [ "/app/covidjellyfish" ]
