# ==========================================
# 0. BAZA
# ==========================================
# tag 'latest' na Docker Hub zawsze wskazuje najnowsze Ubuntu LTS
# przypięcie konkretnej wersji (reprodukowalny build):
#   docker build --build-arg UBUNTU_TAG=26.04 ...
ARG UBUNTU_TAG=latest

# ==========================================
# 1. WARSTWA: RUNTIME BASE (common ground)
# ==========================================
FROM ubuntu:${UBUNTU_TAG} AS runtime-base
ENV DEBIAN_FRONTEND=noninteractive \
    NEEDRESTART_MODE=a \
    TERM=xterm-256color

RUN apt-get update -y && apt-get upgrade -y
RUN apt-get install -y --no-install-recommends ca-certificates
RUN apt-get install -y --no-install-recommends libssl3
RUN apt-get install -y --no-install-recommends libcurl4
# nazwa runtime-owego protobuf-a zawiera SONAME (22.04 -> libprotobuf23, 26.04 -> libprotobuf32t64),
# więc bierzemy ją z zależności pakietu -dev zamiast wpisywać na sztywno
RUN apt-get install -y --no-install-recommends \
    "$(apt-cache depends libprotobuf-dev | awk '/Depends: libprotobuf[0-9]/{print $2; exit}')"
RUN apt-get install -y --no-install-recommends libgomp1
RUN apt-get install -y --no-install-recommends zlib1g
RUN rm -rf /var/lib/apt/lists/*

# ==========================================
# 2. WARSTWA: DEV
# ==========================================
FROM runtime-base AS dev-env

RUN apt-get update -y && apt-get upgrade -y
RUN apt-get install -y --no-install-recommends tar
RUN apt-get install -y --no-install-recommends make
RUN apt-get install -y --no-install-recommends cmake
RUN apt-get install -y --no-install-recommends ninja-build
RUN apt-get install -y --no-install-recommends build-essential
RUN apt-get install -y --no-install-recommends gcc
RUN apt-get install -y --no-install-recommends g++
RUN apt-get install -y --no-install-recommends clang
# libstdc++-*-dev celowo nie jest wpisany - g++ ciągnie wersję zgodną ze swoim GCC
RUN apt-get install -y --no-install-recommends libomp-dev
RUN apt-get install -y --no-install-recommends libbenchmark-dev
RUN apt-get install -y --no-install-recommends libcurl4-openssl-dev
RUN apt-get install -y --no-install-recommends libssl-dev
RUN apt-get install -y --no-install-recommends libprotobuf-dev
RUN apt-get install -y --no-install-recommends protobuf-compiler
RUN apt-get install -y --no-install-recommends git
RUN apt-get install -y --no-install-recommends ca-certificates
# python + matplotlib tylko po to, żeby rysować wykresy z JSON-a Google Benchmark
# (z apt-a, nie z pip-a - Ubuntu blokuje instalacje do systemowego pythona, PEP 668)
RUN apt-get install -y --no-install-recommends python3
RUN apt-get install -y --no-install-recommends python3-matplotlib
RUN rm -rf /var/lib/apt/lists/*

# kontener chodzi jako użytkownik hosta i nie ma swojego $HOME w obrazie,
# więc matplotlib musi dostać jawnie zapisywalny katalog na cache
ENV MPLCONFIGDIR=/tmp/matplotlib

WORKDIR /workspace

# ==========================================
# 3. ETAP: RUNNER (wariant izolowany)
# ==========================================
# Wszystko wkopiowane do obrazu, więc odpala się gdziekolwiek -> docker/run_isolated.sh
# Wariant "na repo" (mount jak w dev-env) nie ma tu swojego etapu,
# bo używa gołego runtime-base -> docker/run.sh
FROM runtime-base AS runner
WORKDIR /app

RUN mkdir -p exe log output input run_time_config build
COPY ./input /app/input
COPY ./run_time_config /app/run_time_config
COPY ./build/*.exe /app/build/
COPY ./build/*.a /app/build/
COPY ./build/*.so /app/build/
COPY ./build/*.dylib /app/build/
ENV LD_LIBRARY_PATH=/app/build
CMD ["/bin/sh", "-c", "exec /app/build/*.exe"]
