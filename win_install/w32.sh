R32=`R.exe RHOME`/bin/i386
export PATH=$R32:"$PATH"
rm -f embedr.o
mkdir -p w32
$R32/R.exe CMD SHLIB -o w32/embedr.dll ../embedr.c ../src/w32/q.a
cp w32/embedr.dll $QHOME/w32
