/ test R server for Q


//%% Define Test Function/Variable %%//vvvvvvvvvvvvvvvvvvvvvvvvv/

HRULE:40#"+-";
TESTCASE:0i;
SUCCESS:0i;
FAILURE:0i;

PROGRESS:{[checkpoint] 
  -1 "";
  -1 HRULE;
  -1 "\t",checkpoint;
  -1 "\tScore:\t",string[SUCCESS],"/",string TESTCASE;
  -1 "\tFail:\t",string[FAILURE],"/",string TESTCASE;
  -1 HRULE;
  -1 "";
 };

EQUAL:{[id;x;y]
  TESTCASE+:1;
  $[x~y;
    SUCCESS+:1;
    [FAILURE+:1; -1 "[",string[id],"] Fail:", -3!x]
  ];
 };

//%% System Setting %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

//Load embedR file
\l ../init.q

//Set seed 42
\S 42

//Set console width
\c 25 200

// set verbose mode
.rk.open 1

//%% Test %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

//Numerical Array//-------------------------/

PROGRESS["Test Start!!"];

.rk.exec "a=array(1:24,c(2,3,4))"

EQUAL[1; .rk.get "dim(a)"; 2 3 4i];
EQUAL[2; .rk.get "a"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];

if[3<=.z.K;.rk.set["a";2?0Ng]]
EQUAL[3; .rk.get "a"; ("84cf32c6-c711-79b4-2f31-6e85923decff";"22371003-8997-eed1-f4df-58fcdedd8376")];

.rk.exec "b= 2 == array(1:24,c(2,3,4))"
EQUAL[4; .rk.get "dim(b)"; 2 3 4i];
EQUAL[5; .rk.get "b"; ((000b;100b);(000b;000b);(000b;000b);(000b;000b))];

EQUAL[6; .rk.get "1.1*array(1:24,c(2,3,4))"; ((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11.0 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22.0 24.2 26.4))];

.rk.set["xyz";1 2 3i]
EQUAL[7; .rk.get "xyz"; 1 2 3i];

EQUAL[8; .rk.get "pi"; (), acos -1];
EQUAL[9; .rk.get "2+3"; (), 5f];
EQUAL[10; .rk.get "11:11"; (), 11i];
EQUAL[11; .rk.get "11:15"; 11 12 13 14 15i];
a:.rk.get "matrix(1:6,2,3)"
EQUAL[12; a[1]; 2 4 6i];
.rk.exec "m=array(1:24,c(2,3,4))"
EQUAL[13; .rk.get "m"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];
EQUAL[14; .rk.get "length(m)"; (), 24i];
EQUAL[15; .rk.get "dim(m)"; 2 3 4i];
EQUAL[16; .rk.get "c(1,2,Inf,-Inf,NaN,NA)"; 1 2 0w -0w 0n 0n];

PROGRESS["Numeric Array Finished!!"];

//Plot Functionality//----------------------/

.rk.exec "pdf(tempfile(\"t1\",fileext=\".pdf\"))"
.rk.exec "plot(c(2,3,5,7,11))"
.rk.exec "dev.off()"

//Function Test//---------------------------/

.rk.exec "x=factor(c('one','two','three','four'))"
EQUAL[17; .rk.get "x"; `one`two`three`four];
EQUAL[18; .rk.get "mode(x)"; "numeric"];
EQUAL[19; .rk.get "typeof(x)"; "integer"];
EQUAL[20; .rk.get "c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"; 100110b];
.rk.exec "foo <- function(x,y) {x + 2 * y}"
.rk.get "foo"
EQUAL[21; .rk.get "typeof(foo)"; "closure"];
EQUAL[22; .rk.get "foo (5,3)"; (), 11f];

PROGRESS["Function Test Finished!!"];

//Object//-----------------------------------/

show .rk.get "wilcox.test(c(1,2,3),c(4,5,6))"
.rk.exec "data(OrchardSprays)"
show .rk.get "OrchardSprays"


// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
.rk.get"install.packages"
//'Broken R object.
EQUAL[23; .rk.get".GlobalEnv"; "environment"];
//"environment"
EQUAL[24; .rk.get"emptyenv()"; "environment"];
//"environment"
EQUAL[25; .rk.get".Internal"; "special"];
//"special"
EQUAL[26; @[.rk.exec; "typeof("; like[;"incomplete: *"]]; 1b];
EQUAL[27; @[.rk.exec; "typeof()"; like[;"eval error*"]]; 1b];
EQUAL[28; .rk.get each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External"); ("builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin")];
.rk.get "getGeneric('+')"

EQUAL[29; .rk.get"as.raw(10)"; (), 0x0a];
EQUAL[30; .rk.get"as.logical(c(1,FALSE,NA))"; 100b];

PROGRESS["Onject Test Finished!!"];

//Table//-----------------------------------/

// data.frame
EQUAL[31; .rk.get"data.frame(a=1:3, b=c('a','b','c'))"; flip `a`b!(1 2 3i;`a`b`c)];
EQUAL[32; .rk.get"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"; flip `a`b!(1 2 3i;1#/:("a";"b";"c"))];
EQUAL[33; .rk.get"data.frame(a=1:3)"; flip enlist[`a]!enlist (1 2 3i)];
EQUAL[34; .rk.get"data.frame()"; ()];

PROGRESS["Table Test Finished!!"];

//Dictionary//------------------------------/

.rk.set["dictI"; `a`b`c!1 2 3i];
EQUAL[35; .rk.get"dictI"; `a`b`c!1 2 3i];
.rk.set["dictJ"; `a`b`c!1 2 3];
EQUAL[36; .rk.get"dictJ"; `a`b`c!1 2 3];
.rk.set["dictB"; `a`b`c!101b];
EQUAL[37; .rk.get"dictB"; `a`b`c!101b];
.rk.set["dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];
EQUAL[38; .rk.get"dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];

//Time//------------------------------------/

// dates
EQUAL[39; .rk.get"as.Date('2005-12-31')"; (), 2005.12.31];
EQUAL[40; .rk.get"as.Date(NA)"; (), 0Nd];
EQUAL[41; .rk.get"rep(as.Date('2005-12-31'),2)"; 2005.12.31 2005.12.31];

// datetime
.rk.exec["Sys.setenv(TZ='UTC')"];
EQUAL[42; .rk.get"as.POSIXct(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[43; .rk.get"as.POSIXlt(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[44; .rk.get"c(as.POSIXct(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXct(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
EQUAL[45; .rk.get"c(as.POSIXlt(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXlt(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
.rk.set["dttm"; 2018.02.18T04:00:01.000z];
EQUAL[46; .rk.get"dttm"; (), 2018.02.18T04:00:01.000z];

// timestamp
.rk.set["tmstp"; 2020.03.16D17:30:45.123456789];
EQUAL[47; .rk.get"tmstp"; (), 2020.03.16D17:30:45.123456789];

// second
.rk.set["scnd"; `second$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
EQUAL[48; .rk.get"scnd"; 12:00:30 12:30:45];
EQUAL[49; .rk.get"as.difftime(c(1, 2), units=\"mins\")"; 00:01 00:02];

// minute
.rk.set["mnt"; `minute$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
EQUAL[50; .rk.get "mnt"; 12:00 12:30];
EQUAL[51; .rk.get"as.difftime(c(1, 2), units=\"secs\")"; 00:00:01 00:00:02];

// days
.rk.set["days"; 1D 2D];
EQUAL[52; .rk.get"days"; 1D 2D];
EQUAL[53; .rk.get"as.difftime(c(1, 2), units=\"days\")"; 1D 2D];

// month
.rk.set["mnth"; `month$/:2020.04.02 2010.01.29]
EQUAL[54; .rk.get"mnth"; 2020.04 2010.01m];

// timespan
.rk.set["tmspans"; 0D12 0D04:20:17.123456789]
EQUAL[55; .rk.get"tmspans"; 0D12 0D04:20:17.123456789];

PROGRESS["Time Test Finished!!"];

//List//---------------------------------------/

//lang
EQUAL[56; .rk.get "as.pairlist(1:10)"; (enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;())];
EQUAL[57; .rk.get "as.pairlist(TRUE)"; (enlist 1b; ())];
EQUAL[58; .rk.get "as.pairlist(as.raw(1))"; (enlist 0x01; ())];
EQUAL[59; .rk.get "pairlist('rnorm', 10L, 0.0, 2.0 )"; ("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())];
.rk.get "list(x ~ y + z)"
EQUAL[60; .rk.get "list( c(1, 5), c(2, 6), c(3, 7) )"; (1 5f;2 6f;3 7f)];
EQUAL[61; .rk.get "matrix( 1:16+.5, nc = 4 )"; (1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)];
.rk.get "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
.rk.get "Instrument$accessors(c('id', 'description'))"
.rk.get "Instrument$new(id='AAPL', description='Apple')"
EQUAL[62; .rk.get "(1+1i)"; "complex"];
EQUAL[63; .rk.get "(0:9)^2"; 0 1 4 9 16 25 36 49 64 81f];
EQUAL[64; .rk.get"expression(rnorm, rnorm(10), mean(1:10))"; "expression"];
EQUAL[65; .rk.get"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"; (0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)];
EQUAL[66; .rk.get"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"; 1 2 1 1 0n 0n -0w 0w];

PROGRESS["List Test Finished!!"];

//Q-Like R Interface//--------------------------/

// long vectors
.rk.exec"x<-c(as.raw(1))"
//.rk.exec"x[2147483648L]<-as.raw(1)"
EQUAL[67; count .rk.get`x; 1];

EQUAL[68; .[.rk.set;("x[0]";1); "nyi"~]; 1b];
EQUAL[69; .rk.get["c()"]; .rk.get"NULL"];
EQUAL[70; (); .rk.get"c()"];
EQUAL[71; {@[.rk.get;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3)); 111111b];
.rk.set[`x;1]
EQUAL[72; .rk.get each ("x";enlist "x";`x;`x`x); 1#/:(1;1;1;1)]; // ("x";"x")?

PROGRESS["Q-Like R Command Test Finished!!"];

//Genral Test//----------------------------------/

.rk.exec"rm(x)"

// run gc
.rk.get"gc()"

//.rk.set["a";`sym?`a`b`c]
//`:x set string 10?`4
//.rk.set["a";get `:x]
//hdel `:x;

.rk.install`data.table
.rk.exec"library(data.table)"
.rk.exec"a<-data.frame(a=c(1,2))"
EQUAL[73; .rk.get`a; flip enlist[`a]!enlist (1 2f)];
.rk.exec "b<-data.table(a=c(1,2))"
EQUAL[74; .rk.get`b; flip enlist[`a]!enlist (1 2f)];
.rk.exec"inspect <- function(x, ...) .Internal(inspect(x,...))"
.rk.get`inspect
.rk.get"substitute(log(1))"

EQUAL[75; flip[`a`b!(`1`2`1;`a`b`b)]; .rk.get"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"];
EQUAL[76; flip[`a`b!(`1`2`1;1#/:("a";"b";"b"))]; .rk.get"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"];
EQUAL[77; flip[`a`b!(`1`2`1;`10`20`30)]; .rk.get"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"];

PROGRESS["Completed!!"];
// all {.[.rk.set;("x";0N!x);"main thread only"~]} peach 2#enlist ([]1 2)
