#!/bin/bash
set -e

make clean
make
echo "=== Output ==="
./memory
