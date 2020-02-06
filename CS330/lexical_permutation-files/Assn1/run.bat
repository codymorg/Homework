make
./gnu.exe 1 > myout
echo ::::: Running Diff :::::
diff myout out1 -y --suppress-common-lines --strip-trailing-cr
echo ::::: End of Diff :::::
