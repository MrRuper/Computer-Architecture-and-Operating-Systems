#!/bin/bash

if [[ $# -eq 0 ]]; then
    echo -e "Please provide an argument.\nValid arguments are:\n\trun_example - runs example tests,\n\tval - validates the program on a pre-made test set (including overflow exceptions),\n\tval_rand - validates the program on randomly generated tests (excluding overflow exceptions),\n\tmeasure_et - measures execution time of the program on a pre-made test set."
    exit 1
fi

if [[ "$1" == "run_example" ]]; then
    ./run_example.sh
elif [[ "$1" == "val" ]]; then
    ./validate.sh

elif [[ "$1" == "val_rand" ]]; then
    ./validate_random.sh
elif [[ "$1" == "measure_et" ]]; then
    ./measure_et.sh
else
    echo "Incorrect argument!"
fi
