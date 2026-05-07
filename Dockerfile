FROM ubuntu:22.04 AS build

ARG TARGETARCH
ARG FRAMEWORK_VERSION=v0.2.0

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates curl unzip make gcc default-libmysqlclient-dev \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

# Download the framework release artifact for this container's architecture.
RUN set -eux; \
    case "$TARGETARCH" in \
        amd64) FW_ZIP="Linux_x86_64.zip" ;; \
        arm64) FW_ZIP="Linux_aarch64.zip" ;; \
        arm)   FW_ZIP="Linux_armv7l.zip" ;; \
        *) echo "unsupported TARGETARCH: $TARGETARCH" >&2; exit 1 ;; \
    esac; \
    rm -rf deps; mkdir -p deps; \
    curl -fsSL -o /tmp/fw.zip \
      "https://github.com/danidomi/c-framework-service/releases/download/${FRAMEWORK_VERSION}/${FW_ZIP}"; \
    unzip -q /tmp/fw.zip -d deps; \
    mv deps/Linux_* deps/c-framework-service; \
    rm -f /tmp/fw.zip

RUN make clean all

FROM ubuntu:22.04
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y --no-install-recommends \
        ca-certificates libmysqlclient21 \
    && rm -rf /var/lib/apt/lists/*

COPY --from=build /app/bin/service-cats-example /app/bin/service-cats-example

EXPOSE 8080
CMD ["/app/bin/service-cats-example"]
