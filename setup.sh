#!/bin/bash

git clone git@github.com:lagerdata/python-ft4222.git 

cd python-ft4222
python setup.py build --verbose
python setup.py install --verbose
cd -
