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

.r.close: LIBPATH (`rclose;1);
.r.open: LIBPATH (`ropen;1);
.r.exec: LIBPATH (`rexec;1);
.r.get: LIBPATH (`rget;1);
.r.set: LIBPATH (`rset;2)

/*-----------------------------------------------*/
/*              Additional Functions             */
/*-----------------------------------------------*/

.r.install:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];
  rcloud:"https://cloud.r-project.org";
  if[0i=first .r.get"is.element('",pkg,"',installed.packages()[,1])";
    .r.exec"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
.r.off:{.r.exec "dev.off()"}
.r.new:{.r.exec "dev.new(noRStudioGD=TRUE)"}

/*-----------------------------------------------*/
/*              Deprecated Function              */
/*-----------------------------------------------*/

// The following block of functions are aliases for the above
// these aliases will be removed in a future version of the code

Ropen   :.r.open;
Rclose  :.r.close;
Rcmd    :.r.exec;
Rget    :{[variable]
  result:.r.get[variable];
  $[
    // Foreign type
    112h ~ type result;
    result;
    // Table has extra column
    98h ~ type result;
    update row.names:1+i from result;
    // bool -> int, month -> int, datetime -> timestamp, timespan -> float, minute -> int, second -> int
    any 1 13 15 16 17 18h in key typemap:group abs type each result;
    [
      if[count typemap 1h;
        $[99h ~ type result;
          result:` _ @[(enlist[`]!enlist (::)), result; typemap 1h; `int$];
          result:1 _ @[(::), result; 1+typemap 1h; `int$]
        ]
      ];
      if[count typemap 13h;
        $[99h ~ type result;
          result:` _ @[(enlist[`]!enlist (::)), result; typemap 13h; `int$];
          result:1 _ @[(::), result; 1+typemap 13h; `int$]
        ]
      ];
      if[count typemap 15h;
        $[99h ~ type result;
          result:` _ @[(enlist[`]!enlist (::)), result; typemap 15h; `timestamp$];
          result:1 _ @[(::), result; 1+typemap 15h; `timestamp$]
        ]
      ];
      if[count typemap 16h;
        $[99h ~ type result;
          result:` _ @[(enlist[`]!enlist (::)), result; typemap 16h; {[timespan] 86400 * timespan % 1D}];
          result:1 _ @[(::), result; 1+typemap 16h; {[timespan] 86400 * timespan % 1D}]
        ]
      ];
      if[sum count each typemap 17 18h;
        $[99h ~ type result;
          result:` _ @[(enlist[`]!enlist (::)), result; raze typemap 17 18h; `int$];
          result:1 _ @[(::), result; 1+raze typemap 17 18h; `int$]
        ]
      ];
      result
    ];
    // Nothing to do
    result
  ]
 }
Rinstall:.r.install;
Rnew    :.r.new;
Roff    :.r.off;
Rset    :.r.set; 
