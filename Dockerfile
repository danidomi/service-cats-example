FROM ubuntu as build-env
RUN apt update && apt install make gcc libmysqlclient-dev mysql-server -y
WORKDIR /app
COPY . .
# Compile the binaries
RUN make

FROM ubuntu
COPY --from=build-env /app/bin/service-cats-example /app/bin/service-cats-example
WORKDIR /app
CMD ["/app/bin/service-cats-example"]