clear
make
echo testing...
size=$(ls | grep -wc out*.*)

for ((i = 0 ; i <= $size ; i++)); do
  echo
  echo
  echo test $i
  rm myout$i
  time ./gnu.exe $i > myout$i
  echo end test ==========================
  echo diff  [mine]  [target]=============
  diff myout$i out$i --strip-trailing-cr -y --suppress-common-lines
  echo end diff ==========================
  
done
