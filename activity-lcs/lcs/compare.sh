#/bin/bash

make clean
make

echo "Sequential"
./../sequential/lcs_seq 10000 10000

echo ""
echo "Parallel"
./lcs 10000 10000 16