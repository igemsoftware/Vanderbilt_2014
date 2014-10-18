#!/bin/bash
cldoc generate -std=c11 -I/usr/local/include/glib-2.0 -I/usr/local/lib/glib-2.0/include -Wall -Wextra -Werror -DSINGLE_START_CODON -DECOLI -DDEBUG -- --output ../cldoc_gen --language c --basedir . src/*
