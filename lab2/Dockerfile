FROM gcc:4.9
COPY . /usr/src/myapp
WORKDIR /usr/src/myapp
RUN apt-get update
RUN apt-get install -y --no-install-recommends valgrind
CMD ["make", "valgrind"]

