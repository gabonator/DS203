set PATH=%PATH%;C:/Programs/Devel/Gcc/arm-2011.03/bin/;C:/Programs/Devel/Makefile/

if not exist ..\..\bin mkdir ..\..\bin
cd ../../bin
make.exe -f ../Build/arm_linux/makefile
