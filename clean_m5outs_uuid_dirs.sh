#!/bin/bash

# https://stackoverflow.com/questions/34057047/delete-all-directories-except-one
shopt -s extglob
cd m5outs
rm -rf !(default-save)