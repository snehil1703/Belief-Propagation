#!/bin/bash

make clean
make

./segment images/cardinal.png images/cardinal-seeds.png
./segment images/nara.png images/nara-seeds.png
./segment images/parakeet.png images/parakeet-seeds.png
./segment images/woodpeckers.png images/woodpeckers-seeds.png
