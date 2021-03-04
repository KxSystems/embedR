R64=`R.exe RHOME`/bin/x64
export PATH=$R64:"$PATH"
rm -f embedr.o
$R64/R.exe CMD SHLIB -o w64/embedr.dll embedr.c src/w64/q.a
cp w64/embedr.dll $QHOME/w64
