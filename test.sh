#!/bin/bash

rm -f simulation_results.csv

for arr in 50.0 25.0 16.67 12.5 10.0 8.33 7.14 6.25 5.88 5.56 5.26; do
    ./sim_data $arr 5.0 500000 M M simulation_results.csv
done