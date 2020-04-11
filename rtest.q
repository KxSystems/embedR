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
\l rinit.q

//Set seed 42
\S 42

//Set console width
\c 25 200

// set verbose mode
Ropen 1

//%% Test %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

//Numerical Array//-------------------------/

PROGRESS["Test Start!!"];

Rcmd "a=array(1:24,c(2,3,4))"

EQUAL[1; Rget "dim(a)"; 2 3 4i];
EQUAL[2; Rget "a"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];

if[3<=.z.K;Rset["a";2?0Ng]]
EQUAL[3; Rget "a"; ("84cf32c6-c711-79b4-2f31-6e85923decff";"22371003-8997-eed1-f4df-58fcdedd8376")];

Rcmd "b= 2 == array(1:24,c(2,3,4))"
EQUAL[4; Rget "dim(b)"; 2 3 4i];
EQUAL[5; Rget "b"; ((0 0 0i;1 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i))];

EQUAL[6; Rget "1.1*array(1:24,c(2,3,4))"; ((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11.0 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22.0 24.2 26.4))];

Rset["xyz";1 2 3i]
EQUAL[7; Rget "xyz"; 1 2 3i];

EQUAL[8; Rget "pi"; (), acos -1];
EQUAL[9; Rget "2+3"; (), 5f];
EQUAL[10; Rget "11:11"; (), 11i];
EQUAL[11; Rget "11:15"; 11 12 13 14 15i];
a:Rget "matrix(1:6,2,3)"
EQUAL[12; a[1]; 2 4 6i];
Rcmd "m=array(1:24,c(2,3,4))"
EQUAL[13; Rget "m"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];
EQUAL[14; Rget "length(m)"; (), 24i];
EQUAL[15; Rget "dim(m)"; 2 3 4i];
EQUAL[16; Rget "c(1,2,Inf,-Inf,NaN,NA)"; 1 2 0w -0w 0n 0n];

PROGRESS["Numeric Array Finished!!"];

//Plot Functionality//----------------------/

Rcmd "pdf(tempfile(\"t1\",fileext=\".pdf\"))"
Rcmd "plot(c(2,3,5,7,11))"
Rcmd "dev.off()"

//Function Test//---------------------------/

Rcmd "x=factor(c('one','two','three','four'))"
EQUAL[17; Rget "x"; `one`two`three`four];
EQUAL[18; Rget "mode(x)"; "numeric"];
EQUAL[19; Rget "typeof(x)"; "integer"];
EQUAL[20; Rget "c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"; 1 0 0N 1 1 0i];
Rcmd "foo <- function(x,y) {x + 2 * y}"
Rget "foo"
EQUAL[21; Rget "typeof(foo)"; "closure"];
EQUAL[22; Rget "foo (5,3)"; (), 11f];

PROGRESS["Function Test Finished!!"];

//Object//-----------------------------------/

Rget "wilcox.test(c(1,2,3),c(4,5,6))"
Rcmd "data(OrchardSprays)"
a:Rget "OrchardSprays"
a

// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
Rget"install.packages"
//'Broken R object.
Rget".GlobalEnv"
//"environment"
Rget"emptyenv()"
//"environment"
Rget".Internal"
//"special"
@[Rcmd;"typeof(";like[;"incomplete: *"]]
@[Rcmd;"typeof()";like[;"eval error*"]]
Rget each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External")
Rget "getGeneric('+')"

EQUAL[23; Rget"as.raw(10)"; (), 0x0a];
EQUAL[24; Rget"as.logical(c(1,FALSE,NA))"; 1 0 0Ni];

PROGRESS["Onject Test Finished!!"];

//Table//-----------------------------------/

// data.frame
EQUAL[25; Rget"data.frame(a=1:3, b=c('a','b','c'))"; flip `a`b!(1 2 3i;`a`b`c)];
EQUAL[26; Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"; flip `a`b!(1 2 3i;1#/:("a";"b";"c"))];
EQUAL[27; Rget"data.frame(a=1:3)"; flip enlist[`a]!enlist (1 2 3i)];
EQUAL[28; Rget"data.frame()"; ()];

PROGRESS["Table Test Finished!!"];

//Time//------------------------------------/

// dates
EQUAL[29; Rget"as.Date('2005-12-31')"; (), 2005.12.31];
EQUAL[30; Rget"as.Date(NA)"; (), 0Nd];
EQUAL[31; Rget"rep(as.Date('2005-12-31'),2)"; 2005.12.31 2005.12.31];

// datetime
Rcmd["Sys.setenv(TZ='UTC')"];
EQUAL[32; Rget"as.POSIXct(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[33; Rget"as.POSIXlt(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[34; Rget"c(as.POSIXct(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXct(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
EQUAL[35; Rget"c(as.POSIXlt(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXlt(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
Rset["dttm"; 2018.02.18T04:00:01.000z];
EQUAL[36; Rget"dttm"; (), 2018.02.18T04:00:01.000z];

//timestamp
Rset["tmstp"; 2020.03.16D17:30:45.123456789];
EQUAL[37; Rget"tmstp"; (), 2020.03.16D17:30:45.123456789];

PROGRESS["Time Test Finished!!"];

//List//---------------------------------------/

//lang
EQUAL[38; Rget "as.pairlist(1:10)"; (enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;())];
EQUAL[39; Rget "as.pairlist(TRUE)"; (enlist 1i; ())];
EQUAL[40; Rget "as.pairlist(as.raw(1))"; (enlist 0x01; ())];
EQUAL[41; Rget "pairlist('rnorm', 10L, 0.0, 2.0 )"; ("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())];
Rget "list(x ~ y + z)"
EQUAL[42; Rget "list( c(1, 5), c(2, 6), c(3, 7) )"; (1 5f;2 6f;3 7f)];
EQUAL[43; Rget "matrix( 1:16+.5, nc = 4 )"; (1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)];
Rget "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
Rget "Instrument$accessors(c('id', 'description'))"
Rget "Instrument$new(id='AAPL', description='Apple')"
EQUAL[44; Rget "(1+1i)"; "complex"];
EQUAL[45; Rget "(0:9)^2"; 0 1 4 9 16 25 36 49 64 81f];
EQUAL[46; Rget"expression(rnorm, rnorm(10), mean(1:10))"; "expression"];
EQUAL[47; Rget"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"; (0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)];
EQUAL[48; Rget"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"; 1 2 1 1 0n 0n -0w 0w];

PROGRESS["List Test Finished!!"];

//Q-Like R Interface//--------------------------/

// long vectors
Rcmd"x<-c(as.raw(1))"
//Rcmd"x[2147483648L]<-as.raw(1)"
EQUAL[49; count Rget`x; 1];

EQUAL[50; .[Rset;("x[0]";1); "nyi"~]; 1b];
EQUAL[51; Rget["c()"]; Rget"NULL"];
EQUAL[52; (); Rget"c()"];
EQUAL[53; {@[Rget;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3)); 111111b];
Rset[`x;1]
EQUAL[54; Rget each ("x";enlist "x";`x;`x`x); 1#/:(1;1;1;1)]; // ("x";"x")?

PROGRESS["Q-Like R Command Test Finished!!"];

//Genral Test//----------------------------------/

Rcmd"rm(x)"

// run gc
Rget"gc()"

//Rset["a";`sym?`a`b`c]
//`:x set string 10?`4
//Rset["a";get `:x]
//hdel `:x;

Rinstall`data.table
Rcmd"library(data.table)"
Rcmd"a<-data.frame(a=c(1,2))"
EQUAL[55; Rget`a; flip enlist[`a]!enlist (1 2f)];
Rcmd "b<-data.table(a=c(1,2))"
EQUAL[56; Rget`b; flip enlist[`a]!enlist (1 2f)];
Rcmd"inspect <- function(x, ...) .Internal(inspect(x,...))"
Rget`inspect
Rget"substitute(log(1))"

EQUAL[57; flip[`a`b!(`1`2`1;`a`b`b)]; Rget"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"];
EQUAL[58; flip[`a`b!(`1`2`1;1#/:("a";"b";"b"))]; Rget"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"];
EQUAL[59; flip[`a`b!(`1`2`1;`10`20`30)]; Rget"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"];

PROGRESS["Completed!!"];
// all {.[Rset;("x";0N!x);"main thread only"~]} peach 2#enlist ([]1 2)
