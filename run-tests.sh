#!/bin/bash
# This file runs ALL tests

./fusion tests/for.fn
./fusion tests/foreach.fn
./fusion tests/if.fn
./fusion tests/import.fn
echo "Hello" | ./fusion tests/input.fn
./fusion tests/print.fn