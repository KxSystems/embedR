/
* @file embedr.q
* @overview
* Define interface functions of embedR.
\

/*-----------------------------------------------*/
/*                Initial Setting                */
/*-----------------------------------------------*/

setenv[`R_HOME;first @[system;$[.z.o like "w*";"call";"env"]," R RHOME";enlist""]];

LIBPATH:`:embedr 2:

/*-----------------------------------------------*/
/*                Load Libraries                 */
/*-----------------------------------------------*/

.rk.close: LIBPATH (`rclose;1);
.rk.open: LIBPATH (`ropen;1);
.rk.exec: LIBPATH (`rexec;1);
.rk.get: LIBPATH (`rget;1);
.rk.set: LIBPATH (`rset;2)

/*-----------------------------------------------*/
/*              Additional Functions             */
/*-----------------------------------------------*/

.rk.install:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];
  rcloud:"https://cloud.r-project.org";
  if[0i=first .rk.get"is.element('",pkg,"',installed.packages()[,1])";
    .rk.exec"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
.rk.off:{.rk.exec "dev.off()"}
.rk.new:{.rk.exec "dev.new(noRStudioGD=TRUE)"}

/*-----------------------------------------------*/
/*              Deprecated Function              */
/*-----------------------------------------------*/

// The following block of functions are aliases for the above
// these aliases will be removed in a future version of the code

Rclose  :.rk.close;
Rcmd    :.rk.exec;
Rget    :.rk.get;
Rinstall:.rk.install;
Rnew    :.rk.new;
Roff    :.rk.off;
Rset    :.rk.set; 

// Some time conversion from R -> q do not work without doing .rk.set first.
("kdbtimestamp_"; "kdbtimespan_"; "kdbdate_"; "kdbmonth_"; "kdbminute_"; "kdbsecond_") .rk.set' `timestamp`timespan`date`month`minute`second$/: 2000.01.01D00:00:00.000000000;
