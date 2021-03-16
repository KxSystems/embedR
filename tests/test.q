/
* test R server for Q.
* # Note
* - When testing on travis CI `.r.install` should not be run; therefore commandline argument
*  `test_data_frame` must be passed with its value `true`, e.g.,
*  $ q tests/test.q -test_data_frame true
* - `-s` flag must be passed to test limtation of main thread only. If `\s` is 0, Final test will be ignored.
\

//%% Commandline arguments %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

COMMANDLINE_ARGS: .Q.opt .z.x;
if[`test_data_frame in key COMMANDLINE_ARGS; @[`COMMANDLINE_ARGS; `test_data_frame; {lower first x}]];

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
\l q/embedr.q

//Set seed 42
\S 42

//Set console width
\c 25 300

// set verbose mode
.r.open 1

//%% Test %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

//Numerical Array//-------------------------/

PROGRESS["Test Start!!"];

.r.exec "a=array(1:24,c(2,3,4))"

EQUAL[1; .r.get "dim(a)"; 2 3 4i];
EQUAL[2; .r.get "a"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];

if[3<=.z.K;.r.set["a";2?0Ng]]
EQUAL[3; .r.get "a"; ("84cf32c6-c711-79b4-2f31-6e85923decff";"22371003-8997-eed1-f4df-58fcdedd8376")];

.r.exec "b= 2 == array(1:24,c(2,3,4))"
EQUAL[4; .r.get "dim(b)"; 2 3 4i];
EQUAL[5; .r.get "b"; ((000b;100b);(000b;000b);(000b;000b);(000b;000b))];

EQUAL[6; .r.get "1.1*array(1:24,c(2,3,4))"; ((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11.0 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22.0 24.2 26.4))];

.r.set["xyz";1 2 3i]
EQUAL[7; .r.get "xyz"; 1 2 3i];

EQUAL[8; .r.get "pi"; (), acos -1];
EQUAL[9; .r.get "2+3"; (), 5f];
EQUAL[10; .r.get "11:11"; (), 11i];
EQUAL[11; .r.get "11:15"; 11 12 13 14 15i];
a:.r.get "matrix(1:6,2,3)"
EQUAL[12; a[1]; 2 4 6i];
.r.exec "m=array(1:24,c(2,3,4))"
EQUAL[13; .r.get "m"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];
EQUAL[14; .r.get "length(m)"; (), 24i];
EQUAL[15; .r.get "dim(m)"; 2 3 4i];
EQUAL[16; .r.get "c(1,2,Inf,-Inf,NaN,NA)"; 1 2 0w -0w 0n 0n];

PROGRESS["Numeric Array Finished!!"];

//Plot Functionality//----------------------/

.r.exec "pdf(tempfile(\"t1\",fileext=\".pdf\"))"
.r.exec "plot(c(2,3,5,7,11))"
.r.exec "dev.off()"

//Function Test//---------------------------/

.r.exec "x=factor(c('one','two','three','four'))"
EQUAL[17; .r.get "x"; `one`two`three`four];
EQUAL[18; .r.get "mode(x)"; "numeric"];
EQUAL[19; .r.get "typeof(x)"; "integer"];
EQUAL[20; .r.get "c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"; 100110b];
.r.exec "foo <- function(x,y) {x + 2 * y}"
.r.get "foo"
EQUAL[21; .r.get "typeof(foo)"; "closure"];
EQUAL[22; .r.get "foo (5,3)"; (), 11f];

PROGRESS["Function Test Finished!!"];

//Object//-----------------------------------/

show .r.get "wilcox.test(c(1,2,3),c(4,5,6))"
.r.exec "data(OrchardSprays)"
show .r.get "OrchardSprays"

// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
.r.get"install.packages"
//'Broken R object.
EQUAL[23; .r.get".GlobalEnv"; "environment"];
//"environment"
EQUAL[24; .r.get"emptyenv()"; "environment"];
//"environment"
EQUAL[25; .r.get".Internal"; "special"];
//"special"
EQUAL[26; @[.r.exec; "typeof("; like[;"incomplete: *"]]; 1b];
EQUAL[27; @[.r.exec; "typeof()"; like[;"eval error*"]]; 1b];
EQUAL[28; .r.get each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External"); ("builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin")];
.r.get "getGeneric('+')"

EQUAL[29; .r.get"as.raw(10)"; (), 0x0a];
EQUAL[30; .r.get"as.logical(c(1,FALSE,NA))"; 100b];

PROGRESS["Object Test Finished!!"];

//Table//-----------------------------------/

// data.frame
EQUAL[31; .r.get"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=TRUE)"; flip `a`b!(1 2 3i;`a`b`c)];
EQUAL[32; .r.get"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"; flip `a`b!(1 2 3i;1#/:("a";"b";"c"))];
EQUAL[33; .r.get"data.frame(a=1:3)"; flip enlist[`a]!enlist (1 2 3i)];
EQUAL[34; .r.get"data.frame()"; ()];

PROGRESS["Table Test Finished!!"];

//Dictionary//------------------------------/

.r.set["dictI"; `a`b`c!1 2 3i];
EQUAL[35; .r.get"dictI"; `a`b`c!1 2 3i];
.r.set["dictJ"; `a`b`c!1 2 3];
EQUAL[36; .r.get"dictJ"; `a`b`c!1 2 3];
.r.set["dictB"; `a`b`c!101b];
EQUAL[37; .r.get"dictB"; `a`b`c!101b];
.r.set["dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];
EQUAL[38; .r.get"dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];

PROGRESS["Dictionary Test Finished!!"];

//Time//------------------------------------/

// timestamp
.r.set["tmstp"; 2020.03.16D17:30:45.123456789];
EQUAL[39; .r.get"tmstp"; (), 2020.03.16D17:30:45.123456789];

// month
.r.set["mnth"; `month$/:2020.04.02 2010.01.29]
EQUAL[40; .r.get"mnth"; 2020.04 2010.01m];

// dates
EQUAL[41; .r.get"as.Date('2005-12-31')"; (), 2005.12.31];
EQUAL[42; .r.get"as.Date(NA)"; (), 0Nd];
EQUAL[43; .r.get"rep(as.Date('2005-12-31'),2)"; 2005.12.31 2005.12.31];

// datetime
.r.exec["Sys.setenv(TZ='UTC')"];
EQUAL[44; .r.get"as.POSIXct(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[45; .r.get"as.POSIXlt(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (),2018.02.18T04:00:01.000z];
EQUAL[46; .r.get"c(as.POSIXct(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXct(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
EQUAL[47; .r.get"c(as.POSIXlt(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXlt(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
.r.set["dttm"; 2018.02.18T04:00:01.000z];
EQUAL[48; .r.get"dttm"; (), 2018.02.18T04:00:01.000z];

// days
.r.set["days"; 1D 2D];
EQUAL[49; .r.get"days"; 1D 2D];
EQUAL[50; .r.get"as.difftime(c(1, 2), units=\"days\")"; 1D 2D];

// timespan
.r.set["tmspans"; 0D12 0D04:20:17.123456789 0D00:00:00.000000012]
EQUAL[51; .r.get"tmspans"; 0D12 0D04:20:17.123456789 0D00:00:00.000000012];

// minute
.r.set["mnt"; `minute$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
EQUAL[52; .r.get "mnt"; 12:00 12:30];
EQUAL[53; .r.get"as.difftime(c(1, 2), units=\"mins\")"; 00:01 00:02];

// second
.r.set["scnd"; `second$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
EQUAL[54; .r.get"scnd"; 12:00:30 12:30:45];
EQUAL[55; .r.get"as.difftime(c(1, 2), units=\"secs\")"; 00:00:01 00:00:02];

PROGRESS["Time Test Finished!!"];

//List//---------------------------------------/

//lang
EQUAL[56; .r.get "as.pairlist(1:10)"; (enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;())];
EQUAL[57; .r.get "as.pairlist(TRUE)"; (enlist 1b; ())];
EQUAL[58; .r.get "as.pairlist(as.raw(1))"; (enlist 0x01; ())];
EQUAL[59; .r.get "pairlist('rnorm', 10L, 0.0, 2.0 )"; ("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())];
.r.get "list(x ~ y + z)"
EQUAL[60; .r.get "list( c(1, 5), c(2, 6), c(3, 7) )"; (1 5f;2 6f;3 7f)];
EQUAL[61; .r.get "matrix( 1:16+.5, nc = 4 )"; (1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)];
.r.get "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
.r.get "Instrument$accessors(c('id', 'description'))"
.r.get "Instrument$new(id='AAPL', description='Apple')"
EQUAL[62; .r.get "(1+1i)"; "complex"];
EQUAL[63; .r.get "(0:9)^2"; 0 1 4 9 16 25 36 49 64 81f];
EQUAL[64; .r.get"expression(rnorm, rnorm(10), mean(1:10))"; "expression"];
EQUAL[65; .r.get"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"; (0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)];
EQUAL[66; .r.get"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"; 1 2 1 1 0n 0n -0w 0w];

PROGRESS["List Test Finished!!"];

//Q-Like R Interface//--------------------------/

// long vectors
.r.exec"x<-c(as.raw(1))"
//.r.exec"x[2147483648L]<-as.raw(1)"
EQUAL[67; count .r.get`x; 1];

EQUAL[68; .[.r.set;("x[0]";1); "nyi"~]; 1b];
EQUAL[69; .r.get["c()"]; .r.get"NULL"];
EQUAL[70; .r.get"c()"; ()];
EQUAL[71; {@[.r.get;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3)); 111111b];
.r.set[`x;1]
EQUAL[72; .r.get each ("x";enlist "x";`x;`x`x); 1#/:(1;1;1;1)]; // ("x";"x")?

PROGRESS["Q-Like R Command Test Finished!!"];

//Genral Test//----------------------------------/

.r.exec"rm(x)"

// run gc
.r.get"gc()"

.r.set["a";`sym?`a`b`c];
`:x set string 10?`4;
.r.set["a";get `:x];
hdel `:x;
hdel `$":x#";

// Finish testing if `test_data_frame` is not "true".
if[not "true" ~ COMMANDLINE_ARGS `test_data_frame;
  PROGRESS["Completed!!"];
  exit 0
 ];

.r.install `data.table
.r.exec"library(data.table)"
.r.exec"a<-data.frame(a=c(1,2))"
EQUAL[73; .r.get`a; flip enlist[`a]!enlist (1 2f)];
.r.exec "b<-data.table(a=c(1,2))"
EQUAL[74; .r.get`b; flip enlist[`a]!enlist (1 2f)];
.r.exec"inspect <- function(x, ...) .Internal(inspect(x,...))"
.r.get`inspect
.r.get"substitute(log(1))"

EQUAL[75; flip[`a`b!(`1`2`1;`a`b`b)]; .r.get"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"),stringsAsFactors=TRUE)"];
EQUAL[76; flip[`a`b!(`1`2`1;1#/:("a";"b";"b"))]; .r.get"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"];
EQUAL[77; flip[`a`b!(`1`2`1;`10`20`30)]; .r.get"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"];

// Finish testing if slave thread is not used.
if[0i ~ system "s";
 PROGRESS["Completed!!"];
 exit 0
 ];

EQUAL[78; all {.[.r.set;("x"; x);"main thread only"~]} peach 2#enlist ([]1 2); 1b];
PROGRESS["Completed!!"];

exit 0
