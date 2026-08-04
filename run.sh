gcc pme.c -o exes/pme
gcc pdump.c -o exes/pdump
gcc pmc.c -o exes/pmc
gcc asm/pasm.c -o exes/pasm
rm binary.pm
./exes/pasm asm/code/temp -o binary.pm
# ./exes/pdump binary.pm
# ./exes/pme binary.pm
./exes/pmc binary.pm > res/binary.s
# cat res/binary.s
yasm -f macho64 res/binary.s -o res/binary.o
clang -arch x86_64 res/binary.o -o res/binary -e _main -lSystem -Wl,-w -Wl,-platform_version,macos,11.0,11.0
./res/binary
echo "Returned" $?
