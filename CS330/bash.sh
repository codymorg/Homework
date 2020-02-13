clear
make
echo testing...
size=$(ls | grep -wc out*.*)

for ((i = 0 ; i <= $size ; i++)); do
  echo
  echo
  echo test $i 
  time ./gnu.exe $i > myout$i
  echo end test ==========================
  echo diff ==============================
  diff myout$i out$i
  echo end diff ==========================
  
done
