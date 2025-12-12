/ 
 test R server for Q

 lines prefixed with t) are tests, which equate to true if pass
 no output for test if it passes, otherwise test printed to stderr

 installs missing R libs on first run which causes output
 so may need to be run more than once to see clearer output
\
\l rinit.q
.t.e:{$[1b~value x;;-2 x];}

Ropen;    // pass 1 for verbose mode

/ set data type bool
Rset["a";0b];
t)"logical"~Rget"class(a)"
Rset["a";0101b];
t)"logical"~Rget"class(a)"
t)4i~first Rget"length(a)"

/ set data type guid
Rset["a";first 1?0Ng]
t)"character"~Rget"class(a)"
Rset["a";first 2?0Ng]
t)"character"~Rget"class(a)"
t)36i~first Rget"nchar(a)"

/ set data type byte
Rset["a";0x01];
t)"raw"~Rget"class(a)"
Rset["a";0x0102];
t)"raw"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type short
Rset["a";1h];
t)"integer"~Rget"class(a)"
Rset["a";1 2h];
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type int
Rset["a";1i];
t)"integer"~Rget"class(a)"
Rset["a";1 2i];
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type long
Rset["a";1];
t)"integer64"~Rget"class(a)"
Rset["a";1 2];
t)"integer64"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type real
Rset["a";1.1e];
t)"numeric"~Rget"class(a)"
Rset["a";1.1 2.2e];
t)"numeric"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type float
Rset["a";1.1];
t)"numeric"~Rget"class(a)"
Rset["a";1.1 2.2];
t)"numeric"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type char
Rset["a";"a"];
t)"character"~Rget"class(a)"
Rset["a";"ab"];
t)"character"~Rget"class(a)"
t)2i~first Rget"nchar(a)"

/ set data type symbol
Rset["a";`aa];
t)"character"~Rget"class(a)"
Rset["a";`aa`bb];
t)"character"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type timestamp
Rset["a";2025.07.26D23:59:59.998999999]
t)"nanotime"~last Rget"class(a)"
Rset["a";2025.07.26D23:59:59.998999999 2025.07.26D23:59:59.998999988]
t)"nanotime"~last Rget"class(a)"

/ set data type month
Rset["a";2001.01m]
t)"integer"~Rget"class(a)"
Rset["a";2001.01 2001.02m]
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type date
Rset["a";2000.01.01]
t)"Date"~Rget"class(a)"
Rset["a";2000.01.01 2000.01.02]
t)"Date"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type datetime (depreciated)
Rset["a";2025.07.26T00:00:00.000]
t)("POSIXt";"POSIXct")~Rget"class(a)"
Rset["a";2025.07.26T00:00:00.001 2025.07.26T00:00:00.002]
t)("POSIXt";"POSIXct")~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type timespan
Rset["a";00:00:00.000000001]
t)"numeric"~Rget"class(a)"
Rset["a";00:00:00.000000001 00:00:00.000000002]
t)"numeric"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type minute
Rset["a";00:01]
t)"integer"~Rget"class(a)"
Rset["a";00:01 00:02]
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type second
Rset["a";00:00:01]
t)"integer"~Rget"class(a)"
Rset["a";00:00:01 00:00:02]
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ set data type time
Rset["a";00:00:00.002]
t)"integer"~Rget"class(a)"
Rset["a";00:00:00.002 00:00:00.003]
t)"integer"~Rget"class(a)"
t)2i~first Rget"length(a)"

/ null
Rset["a";0Ng];         / guid
t)"00000000-0000-0000-0000-000000000000"~Rget"a"
Rset["a";("G"$"8c680a01-5a49-5aab-5a65-d4bfddb6a661";0Ng;"G"$"8c680a01-5a49-5aab-5a65-d4bfddb6a661")];
t)("8c680a01-5a49-5aab-5a65-d4bfddb6a661";"00000000-0000-0000-0000-000000000000";"8c680a01-5a49-5aab-5a65-d4bfddb6a661")~Rget"a"
Rset["a";0Nh];         / short
t)0Ni~first Rget"a"
Rset["a";0 0N 2h];
t)(0 0N 2i)~Rget"a"
Rset["a";0Ni];         / int
t)0Ni~first Rget"a"
Rset["a";1 0N 2i];
t)(1 0N 2i)~Rget"a"
Rset["a";0N];         / long
t)0N~first Rget"a"
Rset["a";1 0N 2];
t)(1 0N 2)~Rget"a"
Rset["a";0Ne];         / real
t)0Nf~first Rget"a"
Rset["a";0 0N 2e];
t)(0 0N 2f)~Rget"a"
Rset["a";0Nf];         / float
t)0Nf~first Rget"a"
Rset["a";1 0N 2f];
t)(1 0N 2f)~Rget"a"
Rset["a";0Np];         / timestamp
t)0Np~first Rget"a"
Rset["a";2025.07.26D23:59:59.998999999 0N 2025.07.26D23:59:59.998999999];
t)(2025.07.26D23:59:59.998999999;0Np;2025.07.26D23:59:59.998999999)~Rget"a"
Rset["a";0Nm];         / TODO month
t)0Ni~first Rget"a"
Rset["a";2001.01 0N 2001.02m];
t)(12 0N 13i)~Rget"a"
Rset["a";0Nd];         / date
t)0Nd~first Rget"a"
Rset["a";2000.01.01 0N 2000.01.02];
t)(2000.01.01 0N 2000.01.02)~Rget"a"
Rset["a";0Nz];         / TODO datetime
t)0Np~first Rget"a"
Rset["a";2025.07.26T00:00:00.000 0N 2025.07.26T00:00:00.001];
t)(2025.07.26D00:00:00.000000000 0N 2025.07.26D00:00:00.001000192)~Rget"a"
Rset["a";0Nn];         / TODO timespan
/ t)(-9.223372e+09)~first Rget"a"
Rset["a";00:00:00.000000001 0N 00:00:00.000000002];
/ t)Rget"a"
Rset["a";0Nu];         / TODO minute
t)0Ni~first Rget"a"
Rset["a";00:01 0N 00:02];
t)(1 0N 2i)~Rget"a"
Rset["a";0Nv];         / TODO second
t)0Ni~first Rget"a"
Rset["a";00:00:01 0N 00:00:02];
t)(1 0N 2i)~Rget"a"
Rset["a";0Nt];         / TODO time
t)0Ni~first Rget"a"
Rset["a";00:00:00.002 0N 00:00:00.003];
t)(2 0N 3i)~Rget"a"

/ inf
Rset["a";0Wh];        / TODO short
t)32767i~first Rget"a"
Rset["a";0 0W 2h];
t)(0 32767 2i)~Rget"a"
Rset["a";0Wi];        / int
t)0Wi~first Rget"a"
Rset["a";1 0W 2i];
t)(1 0W 2i)~Rget"a"
Rset["a";0W];        / long
t)0W~first Rget"a"
Rset["a";1 0W 2];
t)(1 0W 2)~Rget"a"
Rset["a";0We];        / real
t)0w~first Rget"a"
Rset["a";0 0W 2e];
t)(0 0W 2f)~Rget"a"
Rset["a";0w];         / float
t)0w~first Rget"a"
Rset["a";1 0W 2f];
t)(1 0W 2f)~Rget"a"
Rset["a";0Wp];        / timestamp
t)0Wp~first Rget"a"
Rset["a";2025.07.26D23:59:59.998999999 0W 2025.07.26D23:59:59.998999999];
t)(2025.07.26D23:59:59.998999999 0W 2025.07.26D23:59:59.998999999)~Rget"a"
Rset["a";0Wm];        / TODO month
t)0Wi~first Rget"a"
Rset["a";2001.01 0W 2001.02m];
t)(12 0W 13i)~Rget"a"
Rset["a";0Wd];        / date
t)0Wd~first Rget"a"
Rset["a";2000.01.01 0W 2000.01.02];
t)(2000.01.01 0W 2000.01.02)~Rget"a"
Rset["a";0Wz];        / TODO datetime
t)2262.04.11D23:47:16.854775807~first Rget"a"
Rset["a";2025.07.26T00:00:00.000 0W 2025.07.26T00:00:00.001];
t)(2025.07.26D00:00:00.000000000 2262.04.11D23:47:16.854775807 2025.07.26D00:00:00.001000192)~Rget"a"
Rset["a";0Wn];        / TODO timespan
/ t)(9.223372e+09)~first Rget"a"
Rset["a";00:00:00.000000001 0W 00:00:00.000000002];
/ t)Rget"a"
Rset["a";0Wu];        / TODO minute
t)0Wi~first Rget"a"
Rset["a";00:01 0W 00:02];
t)(1 0W 2i)~Rget"a"
Rset["a";0Wv];        / TODO second
t)0Wi~first Rget"a"
Rset["a";00:00:01 0W 00:00:02];
t)(1 0W 2i)~Rget"a"
Rset["a";0Wt];        / TODO time
t)0Wi~first Rget"a"
Rset["a";00:00:00.002 0W 00:00:00.003];
t)(2 0W 3i)~Rget"a"

/ set data type dict
Rset["a";(`a`b`c!1 2 3)]
t)"integer64"~Rget"class(a)"

/ set data type table
Rset["a";([]a:1 2 3;b:1 2 3)]
t)"data.frame"~Rget"class(a)"

a:Rget "wilcox.test(c(1,2,3),c(4,5,6))"
t)"c(1, 2, 3) and c(4, 5, 6)"~last last a
Rcmd "data(OrchardSprays)"
a:Rget "OrchardSprays"
t)flip[`decrease`rowpos`colpos`treatment`row.names!(57 95 8 69 92 90 15 2 84 6 127 36 51 2 69 71 87 72 5 39 22 16 72 4 130 4 114 9 20 24 10 51 43 28 60 5 17 7 81 71 12 29 44 77 4 27 47 76 8 72 13 57 4 81 20 61 80 114 39 14 86 55 3 19f;1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8f;1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6 7 7 7 7 7 7 7 7 8 8 8 8 8 8 8 8f;`D`E`B`H`G`F`C`A`C`B`H`D`E`A`F`G`F`H`A`E`D`C`G`B`H`A`E`C`F`G`B`D`E`D`G`A`C`B`H`F`A`C`F`G`B`D`E`H`B`G`C`F`A`H`D`E`G`F`D`B`H`E`A`C;1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64)]~a 

// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
Rget"install.packages";
//"special"
t)"bytecode"~last last Rget "getGeneric('+')"  
t)(1 2 3 4 5 6 7 8 9 10i)~Rget"1:10"
// data.frame
t)flip[`a`b`row.names!(1 2 3i;(enlist "a";enlist "b";enlist "c");1 2 3)]~Rget"data.frame(a=1:3, b=c('a','b','c'))"
t)flip[`a`b`row.names!(1 2 3i;(enlist "a";enlist "b";enlist "c");1 2 3)]~Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"
t)flip[`a`row.names!(1 2 3i;1 2 3)]~Rget"data.frame(a=1:3)"

//lang
t)"formula"~first first first Rget "list(x ~ y + z)" // TODO
t)"Instrument"~last first first Rget "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
t)"getId"~first first first Rget "Instrument$accessors(c('id', 'description'))"
t)"environment"~(first Rget "Instrument$new(id='AAPL', description='Apple')")`.xData
t)"Instrument"~last (first Rget "Instrument$new(id='AAPL', description='Apple')")`class

// long vectors
t)all {@[Rget;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3))

// run gc
Rget"gc()";

Rset["a";`aa`bb`cc];
t)("aa";"bb";"cc")~Rget`a
Rset["a";("aa";"bb";"cc")];
t)("aa";"bb";"cc")~Rget`a

Rinstall`data.table;
Rcmd"library(data.table)";
Rcmd"a<-data.frame(a=c(1,2))";
t)flip[`a`row.names!(1 2f;1 2)]~Rget`a
Rcmd "b<-data.table(a=c(1,2))";
t)flip[`a`row.names!(1 2f;1 2)]~Rget`b
Rcmd"inspect <- function(x, ...) .Internal(inspect(x,...))"
t)(`...)~last last last Rget`inspect
t)(`log;enlist 1f)~Rget"substitute(log(1))"

t)flip[`a`b`row.names!(`1`2`1;(enlist "a";enlist"b";enlist"b");1 2 3)]~Rget"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"
t)flip[`a`b`row.names!(`1`2`1;(enlist "a";enlist "b";enlist "b");1 2 3)]~Rget"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"
t)flip[`a`b`row.names!(`1`2`1;`10`20`30;1 2 3)]~Rget"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"

Rset[`tmp;0x0101];
t)0x0101~Rget`tmp
Rset["tmp";1f];
t)0x580a000000030004050100030500000000055554462d380000000e000000013ff0000000000000~Rget"serialize(tmp,NULL)"
Rset["tmp";0x580a0000000300040403000305000000000e414e53495f58332e342d313936380000000e000000013ff0000000000000];
t)(enlist 1f)~Rget"unserialize(tmp,NULL)"


// all {.[Rset;("x";0N!x);"main thread only"~]} peach 2#enlist ([]1 2)

// Numerical Array Tests 

Rcmd"a=array(1:24,c(2,3,4))";
t)(2 3 4i)~Rget"dim(a)"
t)((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))~Rget"a"
t)(enlist 24i)~Rget"length(a)"
Rset["a";2?0Ng];
t)(10 10h)~type each Rget"a"
Rcmd"b= 2 == array(1:24,c(2,3,4))";
t)(2 3 4i)~Rget"dim(b)"
t)((000b;100b);(000b;000b);(000b;000b);(000b;000b))~Rget"b"
t)((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11.0 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22.0 24.2 26.4))~Rget"1.1*array(1:24,c(2,3,4))"
Rset["xyz";1 2 3i];
t)(1 2 3i)~Rget"xyz"
t)(enlist acos -1)~Rget"pi"
t)(enlist 5f)~Rget"2+3" 
t)not 11i~get"11:11"        / TODO!!!!!!
t)(11 12 13 14 15i)~Rget"11:15"
a:Rget"matrix(1:6,2,3)";
t)(2 4 6i)~a[1]
t)(1 2 0w -0w 0n 0n)~Rget"c(1,2,Inf,-Inf,NaN,NA)"

// Function Test

Rcmd"x=factor(c('one','two','three','four'))";
t)(`one`two`three`four)~Rget"x"
t)"numeric"~Rget"mode(x)"
t)"integer"~Rget"typeof(x)"
t)(100110b)~Rget"c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"
Rcmd"foo <- function(x,y) {x + 2 * y}";
t)"closure"~Rget"typeof(foo)"
t)(enlist 11f)~Rget"foo (5,3)"

// Object Test
t)"environment"~Rget".GlobalEnv"
t)"environment"~Rget"emptyenv()"
t)"special"~Rget".Internal"
t)@[Rcmd; "typeof("; like[;"incomplete: *"]]
t)@[Rcmd; "typeof()"; like[;"eval error*"]]
t)("builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin")~Rget each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External")
t)(enlist 0x0a)~Rget"as.raw(10)"
t)(100b)~Rget"as.logical(c(1,FALSE,NA))"

// Table Test

t)not (flip `a`b!(1 2 3i;`a`b`c))~Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=TRUE)"     / TODO !!!!!!
t)not (flip `a`b!(1 2 3i;1#/:("a";"b";"c")))~Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"  / TODO !!!!!!
t)not (flip enlist[`a]!enlist (1 2 3i))~Rget"data.frame(a=1:3)"           / TODO !!!!!!
t)()~Rget"data.frame()"

// Dictionary Test

Rset["dictI";`a`b`c!1 2 3i];
t)not (`a`b`c!1 2 3i)~Rget"dictI"        / TODO !!!!!!!
Rset["dictJ"; `a`b`c!1 2 3];
t)not (`a`b`c!1 2 3)~Rget"dictJ"          / TODO !!!!!
Rset["dictB"; `a`b`c!101b];
t)not (`a`b`c!101b)~Rget"dictB"           / TODO !!!!!!
Rset["dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];
t)not (`a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002))~Rget"dictP"   / TODO !!!!!

// Time Test

// timestamp
Rset["tmstp"; 2020.03.16D17:30:45.123456789];
t)(enlist 2020.03.16D17:30:45.123456789)~Rget"tmstp"

// month 
Rset["mnth"; `month$/:2020.04.02 2010.01.29];
t)not (2020.04 2010.01m)~Rget"mnth"    / TODO !!!!!!!

// dates
t)(enlist 2005.12.31)~Rget"as.Date('2005-12-31')"
t)(enlist 0Nd)~Rget"as.Date(NA)"
t)(2005.12.31 2005.12.31)~Rget"rep(as.Date('2005-12-31'),2)"

// datetime
Rcmd"Sys.setenv(TZ='UTC')";
t)not (enlist 2018.02.18T04:00:01.000z)~Rget"as.POSIXct(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"   / TODO !!!!!
t)not (enlist 2018.02.18T04:00:01.000z)~Rget"as.POSIXlt(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"   / TODO !!!!!
t)not (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)~Rget"c(as.POSIXct(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXct(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"   / TODO !!!!!
t)not (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)~Rget"c(as.POSIXlt(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXlt(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"   / TODO !!!!!
Rset["dttm"; 2018.02.18T04:00:01.000z];  
t)not (enlist 2018.02.18T04:00:01.000z)~Rget"dttm"      / TODO !!!!!

// days
Rset["days"; 1D 2D]
t)not (1D 2D)~Rget"days"                                    / TODO !!!!!
t)not (1D 2D)~Rget"as.difftime(c(1, 2), units=\"days\")"    / TODO !!!!!

// timespan
Rset["tmspans"; 0D12 0D04:20:17.123456789 0D00:00:00.000000012];
t)not (0D12 0D04:20:17.123456789 0D00:00:00.000000012)~Rget"tmspans"   / TODO !!!!!

// minute
Rset["mnt"; `minute$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
t)not (12:00 12:30)~Rget"mnt"           / TODO !!!!!
t)not (00:01 00:02)~Rget"as.difftime(c(1, 2), units=\"mins\")"  / TODO !!!!!

// second
Rset["scnd"; `second$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
t)not (12:00:30 12:30:45)~Rget"scnd"              / TODO !!!!!
t)not (00:00:01 00:00:02)~Rget"as.difftime(c(1, 2), units=\"secs\")"   / TODO !!!!!

// List Test
t)((enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;()))~Rget"as.pairlist(1:10)"
t)(enlist 1b; ())~Rget"as.pairlist(TRUE)" 
t)(enlist 0x01; ())~Rget"as.pairlist(as.raw(1))"
t)("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())~Rget"pairlist('rnorm', 10L, 0.0, 2.0 )"
t)(1 5f;2 6f;3 7f)~Rget"list( c(1, 5), c(2, 6), c(3, 7) )"
t)(1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)~Rget"matrix( 1:16+.5, nc = 4 )"
t)"complex"~Rget"(1+1i)"
t)(0 1 4 9 16 25 36 49 64 81f)~Rget"(0:9)^2"
t)"expression"~Rget"expression(rnorm, rnorm(10), mean(1:10))"
t)(0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)~Rget"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"
t)(1 2 1 1 0n 0n -0w 0w)~Rget"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"

// Q-Like R Test
Rcmd"x<-c(as.raw(1))";
t)1~count Rget`x
t).[Rset;("x[0]";1); "nyi"~]
t)(Rget"NULL")~Rget["c()"]
t)()~Rget"c()"
t)(111111b)~{@[Rget;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3))
Rset[`x;1];
t)(enlist 1;enlist 1;enlist 1;enlist 1)~Rget each ("x";enlist "x";`x;`x`x)

// General Test
Rcmd"a<-data.frame(a=c(1,2))"
t)not (flip enlist[`a]!enlist (1 2f))~Rget"a"   / TODO !!!!!!
Rcmd"b<-data.table(a=c(1,2))"
t)not (flip enlist[`a]!enlist (1 2f))~Rget"b"   / TODO !!!!!!
t)not (flip[`a`b!(`1`2`1;`a`b`b)])~Rget"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"),stringsAsFactors=TRUE)"  / TODO !!!!!!
t)not (flip[`a`b!(`1`2`1;1#/:("a";"b";"b"))])~Rget"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"      / TODO !!!!!!
t)not (flip[`a`b!(`1`2`1;`10`20`30)])~Rget"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"           / TODO !!!!!!
/ t)all {.[Rset;("x"; x);"main thread only"~]} peach 2#enlist ([]1 2)   / TODO !!!!!!

\\
