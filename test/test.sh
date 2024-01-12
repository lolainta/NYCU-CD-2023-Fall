if [ $# -ne 1 ]; then
    echo "Usage: $0 <testcase>"
    exit 1
fi
make -j
if [ $? -ne 0 ]; then
    echo "Compilation failed"
    exit 1
fi
echo Testing $1
FNAME=$(basename $1)
SFILE=${FNAME%.*}.S
./src/compiler $1 --dump-ast
if [ $? -ne 0 ]; then
    echo "Compiler execution failed"
    exit 1
fi
echo "Compiling $SFILE"
riscv32-unknown-elf-gcc ./test/io.c $SFILE -o a.out
if [ $? -ne 0 ]; then
    echo "RISCV compilation failed"
    exit 1
fi
echo "Running $SFILE"
spike --isa=RV32 /risc-v/riscv32-unknown-elf/bin/pk a.out

