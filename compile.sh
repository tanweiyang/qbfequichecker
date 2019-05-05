#!/bin/sh

cd depqbf && make
cd ../qbfequichecker && make
cd ../unittest && make
cd ..

if [ ! -f testqbfequichecker ]; then
    ln -s unittest/bin/testqbfequichecker .
fi

