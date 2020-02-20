clear
make
echo testing...
size=$(ls | grep -wc out*.*)

for ((i = 0 ; i < $size ; i++)); do
  echo
  echo test $i 
  time ./gnu.exe $i > myout$i
  echo diff ==============================
  diff myout$i out$i -y --suppress-common-lines --strip-trailing-cr
  echo valgrind===========================
  valgrind -q ./gnu.exe %i > valgrindOut.txt
  
done
