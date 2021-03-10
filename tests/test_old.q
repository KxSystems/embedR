/ 
* test R server for Q
* These tests are for checking compatability with v1.2.
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
Ropen 1   // set verbose mode

//%% Test %%//vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv/

//Numerical Array//-------------------------/

PROGRESS["Test Start!!"];

Rcmd "a=array(1:24,c(2,3,4))"
EQUAL[1; Rget "dim(a)"; 2 3 4i];
EQUAL[2; Rget "a"; ((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))];

if[3<=.z.K; Rset["a";2?0Ng]]
EQUAL[3; Rget "a"; ("84cf32c6-c711-79b4-2f31-6e85923decff";"22371003-8997-eed1-f4df-58fcdedd8376")];

Rcmd "b= 2 == array(1:24,c(2,3,4))"
EQUAL[4; Rget "dim(b)"; 2 3 4i];
// Type mapping issue of v1.2
EQUAL[5; Rget "b"; ((0 0 0i;1 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i))];

EQUAL[6; Rget "1.1*array(1:24,c(2,3,4))"; ((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22 24.2 26.4))];

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
// Type mapping issue of v1.2
EQUAL[20; Rget "c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"; 1 0 0N 1 1 0i];
Rcmd "foo <- function(x,y) {x + 2 * y}"
Rget "foo"
EQUAL[21; Rget "typeof(foo)"; "closure"];
EQUAL[22; Rget "foo (5,3)"; (), 11f];

PROGRESS["Function Test Finished!!"];

//Object//-----------------------------------/

// Old output is:
// ((`names;`class)!(("statistic";"parameter";"p.value";"null.value";"alternative";"method";"data.name");"htest");((,`names!,,"W";,0f);();,0.1;(,`names!,"location shift";,0f);"two.sided";"Wilcoxon rank sum exact test";"c(1, 2, 3) and c(4, 5, 6)"))
// New output is:
// `statistic`parameter`p.value`null.value`alternative`method`data.name!((,`W)!,0f;();,0.1;(,`location shift)!,0f;"two.sided";"Wilcoxon rank sum exact test";"c(1, 2, 3) and c(4, 5, 6)")
Rget "wilcox.test(c(1,2,3),c(4,5,6))"
Rcmd "data(OrchardSprays)"
a:Rget "OrchardSprays"
a

// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
Rget"install.packages"
//'Broken R object.
EQUAL[23; Rget".GlobalEnv"; "environment"];
//"environment"
EQUAL[24; Rget"emptyenv()"; "environment"];
//"environment"
EQUAL[25; Rget".Internal"; "special"];
//"special"
EQUAL[26; @[Rcmd; "typeof("; like[;"incomplete: *"]]; 1b];
EQUAL[27; @[Rcmd; "typeof()"; like[;"eval error*"]]; 1b];
EQUAL[28; Rget each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External"); ("builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin")];
Rget "getGeneric('+')"

EQUAL[29; Rget"as.raw(10)"; (), 0x0a];
// Type mapping issue of v1.2
EQUAL[30; Rget"as.logical(c(1,FALSE,NA))"; 1 0 0Ni];

PROGRESS["Object Test Finished!!"];

//Table//-----------------------------------/

// data.frame
EQUAL[31; Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=TRUE)"; flip `a`b`row.names!(1 2 3i;`a`b`c;1 2 3)];
EQUAL[32; Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"; flip `a`b`row.names!(1 2 3i;(enlist "a";enlist "b";enlist "c");1 2 3)];
EQUAL[33; Rget"data.frame(a=1:3)"; flip `a`row.names!(1 2 3i;1 2 3)];
EQUAL[34; Rget"data.frame()"; ()];

PROGRESS["Table Test Finished!!"];

//Dictionary//------------------------------/

Rset["dictI"; `a`b`c!1 2 3i];
// Type mapping issue in v1.2
// Key has type 0
EQUAL[35; Rget"dictI"; (enlist[`names]!enlist 1#/:("a"; "b"; "c"); 1 2 3i)];
Rset["dictJ"; `a`b`c!1 2 3];
// Type mapping issue in v1.2
// Key has type 0
EQUAL[36; Rget"dictJ"; (enlist[`names]!enlist 1#/:("a"; "b"; "c"); 1 2 3f)];
Rset["dictB"; `a`b`c!101b];
// Type mapping issue in v1.2
// Key has type 0
EQUAL[37; Rget"dictB"; (enlist[`names]!enlist 1#/:("a"; "b"; "c"); 1 0 1i)];
Rset["dictP"; `a`b`c!(2020.04.13D06:08:03.712336000; 2020.04.13D06:08:03.712336001; 2020.04.13D06:08:03.712336002)];
EQUAL[38; Rget"dictP"; 2020.04.13D06:08:03.712336128 2020.04.13D06:08:03.712336128 2020.04.13D06:08:03.712336128];

PROGRESS["Dictionary Test Finished!!"];

//Time//------------------------------------/

// timestamp
Rset["tmstp"; 2020.03.16D17:30:45.123456789];
// Calculation is not acculate in v1.2.
EQUAL[39; Rget"tmstp"; (), 2020.03.16D17:30:45.123456768];

// month
Rset["mnth"; `month$/:2020.04.02 2010.01.29]
EQUAL[40; Rget"mnth"; 243 120i];

// dates
EQUAL[41; Rget"as.Date('2005-12-31')"; (), 2005.12.31];
EQUAL[42; Rget"as.Date(NA)"; (), 0Nd];
EQUAL[43; Rget"rep(as.Date('2005-12-31'),2)"; 2005.12.31 2005.12.31];

// datetime
Rcmd["Sys.setenv(TZ='UTC')"];
// Type mapping issue in v1.2
EQUAL[44; Rget"as.POSIXct(\"2018-02-18 04:00:01\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC')"; (), 2018.02.18D04:00:01.000000000];
// Type mapping issue in v1.2
EQUAL[45; Rget"c(as.POSIXct(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXct(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; 2015.03.16D17:30:00.000000000 1978.06.01D12:30:59.000000000];
// POSIXlt is not supported
EQUAL[46; Rget"c(as.POSIXlt(\"2015-03-16 17:30:00\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'), as.POSIXlt(\"1978-06-01 12:30:59\", format=\"%Y-%m-%d %H:%M:%S\", tz='UTC'))"; (2015.03.16T17:30:00.000z; 1978.06.01T12:30:59.000z)];
Rset["dttm"; 2018.02.18T04:00:01.000z];
// Type mapping issue in v1.2
EQUAL[47; Rget"dttm"; (), 2018.02.18D04:00:01.000000000];

// days
Rset["days"; 1D 2D];
// Type mapping issue in v1.2
EQUAL[48; Rget"days"; 86400 172800f];
// Type mapping issue in v1.2
// Key is type 0.
EQUAL[49; Rget"as.difftime(c(1, 2), units=\"days\")"; ((`class;`units)!("difftime";"days");1 2f)];

// timespan
Rset["tmspans"; 0D12 0D04:20:17.123456789 0D00:00:00.000000012]
// Type mapping issue in v1.2
EQUAL[50; Rget"tmspans"; (43200f; 86400 * 0D04:20:17.123456789 % 1D; 1.2e-08)];

// minute
Rset["mnt"; `minute$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
// Type mapping issue in v1.2
EQUAL[51; Rget "mnt"; 720 750i];
// Type mapping issue in v1.2
// Key is type 0.
EQUAL[52; Rget"as.difftime(c(1, 2), units=\"mins\")"; ((`class;`units)!("difftime";"mins");1 2f)];

// second
Rset["scnd"; `second$(2019.04.01D12:00:30; 2019.04.01D12:30:45)];
// Type mapping issue in v1.2
EQUAL[53; Rget"scnd"; 43230 45045i];
// Type mapping issue in v1.2
// Key is type 0.
EQUAL[54; Rget"as.difftime(c(1, 2), units=\"secs\")"; ((`class;`units)!("difftime";"secs");1 2f)];

PROGRESS["Time Test Finished!!"];

//List//---------------------------------------/

//lang
EQUAL[55; Rget "as.pairlist(1:10)"; (enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;())];
// Type mapping issue of v1.2
EQUAL[56; Rget "as.pairlist(TRUE)"; (enlist 1i; ())];
EQUAL[57; Rget "as.pairlist(as.raw(1))"; (enlist 0x01; ())];
EQUAL[58; Rget "pairlist('rnorm', 10L, 0.0, 2.0 )"; ("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())];
Rget "list(x ~ y + z)"
EQUAL[59; Rget "list( c(1, 5), c(2, 6), c(3, 7) )"; (1 5f;2 6f;3 7f)];
EQUAL[60; Rget "matrix( 1:16+.5, nc = 4 )"; (1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)];

// Old output:
// ((`className;`package;`generator;`class)!((,`package!,".GlobalEnv";"Instrument");".GlobalEnv";((`.xData;`class)!("environment";(,`package!,"methods";"refGeneratorSlot"));"S4");(,`package!,"methods";"refObjectGenerator"));((`;`...);(`new;(,`package!,".GlobalEnv";"Instrument");`...)))
// New output:
// ((((".GlobalEnv";`package);"Instrument");`className;".GlobalEnv";`package;(("environment";`.xData;(("methods";`package);"refGeneratorSlot");`class);"S4");`generator;(("methods";`package);"refObjectGenerator");`class);((`;`...);(`new;((".GlobalEnv";`package);"Instrument");`...)))
Rget "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
// Old output:
// (,`names!,("getId";"setId";"getDescription";"setDescription");((();`id);((`;`value);(`{;(`<<-;`id;`value);(`invisible;`value)));(();`description);((`;`value);(`{;(`<<-;`description;`value);(`invisible;`value)))))
// New output:
// `getId`setId`getDescription`setDescription!((();`id);((`;`value);(`{;(`<<-;`id;`value);(`invisible;`value)));(();`description);((`;`value);(`{;(`<<-;`description;`value);(`invisible;`value))))
Rget "Instrument$accessors(c('id', 'description'))"
// Old output:
// ((`.xData;`class)!("environment";(,`package!,".GlobalEnv";"Instrument"));"S4")
// New output:
// (("environment";`.xData;((".GlobalEnv";`package);"Instrument");`class);"S4")
Rget "Instrument$new(id='AAPL', description='Apple')"
EQUAL[61; Rget "(1+1i)"; "complex"];
EQUAL[62; Rget "(0:9)^2"; 0 1 4 9 16 25 36 49 64 81f];
EQUAL[63; Rget"expression(rnorm, rnorm(10), mean(1:10))"; "expression"];
EQUAL[64; Rget"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"; (0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)];
EQUAL[65; Rget"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"; 1 2 1 1 0n 0n -0w 0w];

//Q-Like R Interface//--------------------------/

// long vectors
Rcmd"x<-c(as.raw(1))"
//Rcmd"x[2147483648L]<-as.raw(1)"
EQUAL[66; count Rget`x; 1];

EQUAL[67; .[Rset;("x[0]";1); "nyi"~]; 1b];
EQUAL[68; Rget["c()"]; Rget"NULL"];
EQUAL[69; Rget"c()"; ()];
EQUAL[70; {@[Rget;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3)); 111111b];
Rset[`x;1]
EQUAL[71; Rget each ("x";enlist "x";`x;`x`x); 1#/:(1f;1f;1f;1f)]; // ("x";"x")?

PROGRESS["Q-Like R Command Test Finished!!"];

//Genral Test//----------------------------------/

Rcmd"rm(x)"

// run gc
Rget"gc()"

Rset["a";`sym?`a`b`c]
`:x set string 10?`4;
Rset["a";get `:x]
hdel `:x;
hdel `$":x#";

// Finish testing if `test_data_frame` is not "true".
if[not "true" ~ COMMANDLINE_ARGS `test_data_frame;
  PROGRESS["Completed!!"];
  exit 0
 ];

Rinstall`data.table
Rcmd"library(data.table)"
Rcmd"a<-data.frame(a=c(1,2))"
EQUAL[72; Rget`a; flip `a`row.names!(1 2f;1 2)];
Rcmd "b<-data.table(a=c(1,2))"
EQUAL[73; Rget`b; flip `a`row.names!(1 2f;1 2)];
Rcmd"inspect <- function(x, ...) .Internal(inspect(x,...))"
Rget`inspect
Rget"substitute(log(1))"

EQUAL[74; Rget"data.frame(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"),stringsAsFactors=TRUE)"; flip `a`b`row.names!(`1`2`1;`a`b`b;1 2 3)];
EQUAL[75; Rget"data.table(a=as.factor(c(1,2,1)), b=c(\"a\",\"b\",\"b\"))"; flip `a`b`row.names!(`1`2`1;1#/:("a";"b";"b");1 2 3)];
EQUAL[76; Rget"data.table(a=as.factor(c(1,2,1)), b=as.factor(c(10,20,30)))"; flip `a`b`row.names!(`1`2`1;`10`20`30;1 2 3)];

// Finish testing if slave thread is not used.
if[0i ~ system "s";
 PROGRESS["Completed!!"];
 exit 0
 ];

EQUAL[77; all {.[Rset;("x";0N!x);"main thread only"~]} peach 2#enlist ([]1 2); 1b];
PROGRESS["Completed!!"];

exit 0
