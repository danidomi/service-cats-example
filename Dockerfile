FROM alpine:3.20 AS build

RUN apk add --no-cache \
        ca-certificates curl unzip make gcc musl-dev mariadb-connector-c-dev

# Install cdeps from source. (The upstream install.sh on HEAD is currently
# broken — `rm && curl` short-circuits when the .zip doesn't yet exist.)
RUN set -eux; \
    curl -fsSL -o /tmp/cdeps.zip \
        "https://github.com/danidomi/cdeps/archive/refs/heads/main.zip"; \
    unzip -q /tmp/cdeps.zip -d /tmp; \
    cd /tmp/cdeps-main; \
    gcc -o /usr/local/bin/cdeps main.c $(find src -type f -name "*.c") -w; \
    rm -rf /tmp/cdeps.zip /tmp/cdeps-main

WORKDIR /app
COPY . .

# Fetch framework dependencies declared in c.deps.
# cdeps unpacks releases under deps/<repo>/Linux_<arch>/; flatten that
# subdir so the include path <c-framework-service/...> resolves under -Ideps.
RUN set -eux; \
    rm -rf deps; \
    cdeps install; \
    for d in deps/*/Linux_* deps/*/Darwin_*; do \
        [ -d "$d" ] || continue; \
        mv "$d"/* "$(dirname "$d")/"; \
        rmdir "$d"; \
    done

RUN make clean all

FROM alpine:3.20
RUN apk add --no-cache \
        ca-certificates mariadb-connector-c
COPY --from=build /app/bin/service-cats-example /app/bin/service-cats-example

EXPOSE 8080
CMD ["/app/bin/service-cats-example"]
