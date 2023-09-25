FROM ubuntu as build-env
RUN apt update && apt install make gcc libmysqlclient-dev mysql-server -y
WORKDIR /app
COPY . .
# Compile the binaries
RUN make

FROM ubuntu
COPY --from=build-env /app/bin/Server /app/bin/Server
WORKDIR /app
CMD ["/app/bin/Server"]