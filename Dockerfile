# ---------- build stage ----------
FROM ubuntu:24.04 AS build
RUN apt-get update && apt-get install -y --no-install-recommends \
        build-essential cmake git ca-certificates libgtest-dev \
    && rm -rf /var/lib/apt/lists/*

# Use the same FTXUI version that built locally (check with: git -C ~/FTXUI describe --tags)
ARG FTXUI_VERSION=v6.1.9
RUN git clone --depth 1 --branch ${FTXUI_VERSION} https://github.com/ArthurSonzogni/FTXUI /tmp/ftxui \
 && cmake -S /tmp/ftxui -B /tmp/ftxui/build \
        -DFTXUI_BUILD_EXAMPLES=OFF -DFTXUI_BUILD_DOCS=OFF -DFTXUI_BUILD_TESTS=OFF \
 && cmake --build /tmp/ftxui/build -j"$(nproc)" \
 && cmake --install /tmp/ftxui/build \
 && rm -rf /tmp/ftxui

WORKDIR /app
COPY . .
RUN make clean && make tividy

# ---------- runtime stage ----------
FROM ubuntu:24.04
WORKDIR /app
COPY --from=build /app/tividy /app/tividy
ENV TERM=xterm-256color
CMD ["./tividy"]

# How to run:
# docker build -t tividy .
# docker run -t --rm tividy