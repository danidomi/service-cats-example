FROM ubuntu as build-env

RUN apt update && apt install -y make zip curl libmysqlclient-dev gcc mysql-server wget

# Create the /app directory
RUN mkdir -p /app

# Download the install.sh script
RUN wget https://raw.githubusercontent.com/danidomi/cdeps/HEAD/install.sh -O /app/install.sh && chmod +x /app/install.sh

WORKDIR /app

# Make the install.sh script executable

# Run the install.sh script with the source command to modify the current shell's environment
RUN /bin/bash -c "source install.sh"

# Continue with your build process
COPY . .

# Add the directory containing cdeps to the system's PATH
ENV PATH="${PATH}:/root/.local/bin"

# Compile the dependencies
RUN cdeps install

# Compile the binaries
RUN make

FROM ubuntu
COPY --from=build-env /app/bin/service-cats-example /app/bin/service-cats-example
WORKDIR /app
CMD ["/app/bin/service-cats-example"]