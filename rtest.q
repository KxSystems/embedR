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

Rcmd "a=array(1:24,c(2,3,4))"
t)(2 3 4i)~Rget "dim(a)"
t)((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))~Rget "a"

Rset["a";2?0Ng];
a:Rget "a";
t)(10 10h)~type each a

Rcmd "b= 2 == array(1:24,c(2,3,4))";
t)(2 3 4i)~Rget "dim(b)"
t)((0 0 0i;1 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i);(0 0 0i;0 0 0i))~Rget "b"

t)((1.1 3.3 5.5;2.2 4.4 6.6);(7.7 9.9 12.1;8.8 11 13.2);(14.3 16.5 18.7;15.4 17.6 19.8);(20.9 23.1 25.3;22 24.2 26.4))~Rget "1.1*array(1:24,c(2,3,4))"

Rset["xyz";1 2 3i];
t)(1 2 3i)~Rget "xyz"
t)(enlist 3.1415926535897931)~Rget "pi"
t)(enlist 5f)~Rget "2+3"
t)(enlist 11i)~Rget "11:11"
t)(11 12 13 14 15i)~Rget "11:15"
t)(1 3 5i;2 4 6i)~Rget "matrix(1:6,2,3)"
Rcmd "m=array(1:24,c(2,3,4))";
t)((1 3 5i;2 4 6i);(7 9 11i;8 10 12i);(13 15 17i;14 16 18i);(19 21 23i;20 22 24i))~Rget "m"
t)(enlist 24i)~Rget "length(m)"
t)(2 3 4i)~Rget "dim(m)"
t)(1 2 0w -0w 0n 0n)~Rget "c(1,2,Inf,-Inf,NaN,NA)"

Rcmd "x=factor(c('one','two','three','four'))"
t)(`one`two`three`four)~Rget "x"
t)"numeric"~Rget "mode(x)"
t)"integer"~Rget "typeof(x)"
t)(1 0 0N 1 1 0i)~Rget "c(TRUE,FALSE,NA,TRUE,TRUE,FALSE)"
Rcmd "foo <- function(x,y) {x + 2 * y}"
t)"closure"~Rget "typeof(foo)"
t)(enlist 11f)~Rget "foo (5,3)"
a:Rget "wilcox.test(c(1,2,3),c(4,5,6))"
t)"c(1, 2, 3) and c(4, 5, 6)"~last last a
Rcmd "data(OrchardSprays)"
\c 100000 1000000
a:Rget "OrchardSprays"
t)flip[`decrease`rowpos`colpos`treatment`row.names!(57 95 8 69 92 90 15 2 84 6 127 36 51 2 69 71 87 72 5 39 22 16 72 4 130 4 114 9 20 24 10 51 43 28 60 5 17 7 81 71 12 29 44 77 4 27 47 76 8 72 13 57 4 81 20 61 80 114 39 14 86 55 3 19f;1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8 1 2 3 4 5 6 7 8f;1 1 1 1 1 1 1 1 2 2 2 2 2 2 2 2 3 3 3 3 3 3 3 3 4 4 4 4 4 4 4 4 5 5 5 5 5 5 5 5 6 6 6 6 6 6 6 6 7 7 7 7 7 7 7 7 8 8 8 8 8 8 8 8f;`D`E`B`H`G`F`C`A`C`B`H`D`E`A`F`G`F`H`A`E`D`C`G`B`H`A`E`C`F`G`B`D`E`D`G`A`C`B`H`F`A`C`F`G`B`D`E`H`B`G`C`F`A`H`D`E`G`F`D`B`H`E`A`C;1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31 32 33 34 35 36 37 38 39 40 41 42 43 44 45 46 47 48 49 50 51 52 53 54 55 56 57 58 59 60 61 62 63 64)]~a 

// to install package in non-interactive way
// install.packages("zoo", repos="http://cran.r-project.org")
Rget"install.packages";
//'Broken R object.
t)"environment"~Rget".GlobalEnv"
//"environment"
t)"environment"~Rget"emptyenv()"
//"environment"
t)"special"~Rget".Internal"
//"special"
t)@[Rcmd;"typeof(";like[;"incomplete: *"]]
t)@[Rcmd;"typeof()";like[;"eval error*"]]
t)("builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin";"builtin")~Rget each ("cos";".C";"floor";"Im";"cumsum";"nargs";"proc.time";"dim";"length";"names";".External")
t)"bytecode"~last last Rget "getGeneric('+')"  
t)(enlist 0x0a)~Rget"as.raw(10)"
t)(1 0 0Ni)~Rget"as.logical(c(1,FALSE,NA))"
t)(1 2 3 4 5 6 7 8 9 10i)~Rget"1:10"
// data.frame
t)flip[`a`b`row.names!(1 2 3i;(enlist "a";enlist "b";enlist "c");1 2 3)]~Rget"data.frame(a=1:3, b=c('a','b','c'))"
t)flip[`a`b`row.names!(1 2 3i;(enlist "a";enlist "b";enlist "c");1 2 3)]~Rget"data.frame(a=1:3, b=c('a','b','c'),stringsAsFactors=FALSE)"
t)flip[`a`row.names!(1 2 3i;1 2 3)]~Rget"data.frame(a=1:3)"
t)()~Rget"data.frame()"
// dates
t)(enlist 2005.12.31)~Rget"as.Date('2005-12-31')"
t)(enlist 0Nd)~Rget"as.Date(NA)"
t)(2005.12.31 2005.12.31)~Rget"rep(as.Date('2005-12-31'),2)"

//lang
t)(enlist 1i;();enlist 2i;();enlist 3i;();enlist 4i;();enlist 5i;();enlist 6i;();enlist 7i;();enlist 8i;();enlist 9i;();enlist 10i;())~Rget "as.pairlist(1:10)"
t)(enlist 1i;())~Rget "as.pairlist(TRUE)"
t)(enlist 0x01;())~Rget "as.pairlist(as.raw(1))"
t)("rnorm";();enlist 10i;();enlist 0f;();enlist 2f;())~Rget "pairlist('rnorm', 10L, 0.0, 2.0 )"
t)"formula"~first first first Rget "list(x ~ y + z)" // TODO
t)(1 5f;2 6f;3 7f)~Rget "list( c(1, 5), c(2, 6), c(3, 7) )"
t)(1.5 5.5 9.5 13.5;2.5 6.5 10.5 14.5;3.5 7.5 11.5 15.5;4.5 8.5 12.5 16.5)~Rget "matrix( 1:16+.5, nc = 4 )"
t)"Instrument"~last first first Rget "Instrument <- setRefClass(Class='Instrument',fields=list('id'='character', 'description'='character'))"
t)"getId"~first first first Rget "Instrument$accessors(c('id', 'description'))"
t)"environment"~(first Rget "Instrument$new(id='AAPL', description='Apple')")`.xData
t)"Instrument"~last (first Rget "Instrument$new(id='AAPL', description='Apple')")`class
t)"complex"~Rget "(1+1i)"
t)(0 1 4 9 16 25 36 49 64 81f)~Rget "(0:9)^2"
t)"expression"~Rget"expression(rnorm, rnorm(10), mean(1:10))"
t)(0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n 0n;0n 0n 0n 0n 0n 0n)~Rget"list( rep(NA_real_, 20L), rep(NA_real_, 6L) )"
t)(1 2 1 1 0n 0n -0w 0w)~Rget"c(1, 2, 1, 1, NA, NaN, -Inf, Inf)"

// long vectors
Rcmd"x<-c(as.raw(1))"
//Rcmd"x[2147483648L]<-as.raw(1)"
t)1~count Rget`x

t).[Rset;("x[0]";1);"nyi"~]
t)Rget["c()"]~Rget"NULL"
t)()~Rget"c()"
t)all {@[Rget;x;"type"~]}each (.z.p;0b;1;1f;{};([1 2 3]1 2 3))
Rset[`x;1]
t)(enlist 1f;enlist 1f;enlist 1f;enlist 1f)~Rget each ("x";enlist "x";`x;`x`x)  // ("x";"x")?
Rcmd"rm(x)"

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


// all {.[Rset;("x";0N!x);"main thread only"~]} peach 2#enlist ([]1 2)
\\
