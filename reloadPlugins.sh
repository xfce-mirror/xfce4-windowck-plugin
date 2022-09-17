#!/bin/sh

for i in wckbuttons wckmenu windowck; do
    pkill --full "lib${i}.so"
done
