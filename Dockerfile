FROM ghcr.io/augustinsangam/lttng-otelcpp:main

RUN ldconfig

WORKDIR /code
COPY . .

CMD time ./run.sh
