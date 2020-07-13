rm account.dat credential.dat
touch account.dat credential.dat
gcc UpdateDetails.c
./a.out
gcc main.c -lpthread
gcc client_bank.c -o c -lpthread
