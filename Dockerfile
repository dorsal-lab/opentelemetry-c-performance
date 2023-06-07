FROM ghcr.io/dorsal-lab/lttng-otelcpp:main

RUN ldconfig

WORKDIR /code
COPY . .

CMD time ./run.sh
