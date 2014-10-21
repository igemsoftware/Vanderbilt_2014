#!/bin/bash

mkdir -p ../cldoc_gen

cldoc generate -std=c11 "$(pkg-config --cflags glib-2.0)" \
    -Wall -Wextra -Werror -DSINGLE_START_CODON -DECOLI -DDEBUG -- \
    --output ../cldoc_gen --language c --basedir . src/*
