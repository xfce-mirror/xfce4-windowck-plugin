#!/bin/sh

for i in wckbuttons wckmenu wcktitle wckspacer; do
    pkill --full "lib${i}.so"
done
