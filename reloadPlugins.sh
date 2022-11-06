#!/bin/sh

for i in wckbuttons wckmenu wcktitle; do
    pkill --full "lib${i}.so"
done
