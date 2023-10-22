FROM ubuntu as build-env

RUN apt update && apt install -y make zip curl gcc wget libmysqlclient-dev

# Create the /app directory
RUN mkdir -p /app

WORKDIR /app

# Run the install.sh script with the source command to modify the current shell's environment
RUN /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/danidomi/cdeps/HEAD/install.sh)"

# Add the directory containing cdeps to the system's PATH
ENV PATH="${PATH}:/root/.local/bin"

# Continue with your build process
COPY . .

# Compile the dependencies
RUN cdeps install

# Compile the binaries
RUN make

FROM ubuntu

RUN apt update && apt install -y libmysqlclient-dev

COPY --from=build-env /app/bin/service-cats-example /app/bin/service-cats-example

CMD ["/app/bin/service-cats-example"]