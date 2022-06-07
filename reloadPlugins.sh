#!/bin/sh

for i in windowck wckbuttons; do
    pkill --full "lib${i}.so"
done
