all: configure build run

configure:
    #!/usr/bin/env bash
    mkdir -p build
    conan install . -if build -pr default -pr:b=default
    conan build -c . -bf build

build:
    conan build -b . -bf build

run:
    build/image_main

bench:
    build/image_bench

clean:
    rm -rf build