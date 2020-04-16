\d .rk

LIBPATH:`:embedr 2:

funcs:(
  (`rclose;1);
  (`ropen;1);
  (`rexec;1);
  (`rget;1);
  (`rset;2)
  )
.rk,:(`$1_'string funcs[;0])!LIBPATH@/:funcs

install:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];rcloud:"https://cloud.r-project.org";
  if[0i=first .rk.get"is.element('",pkg,"',installed.packages()[,1])";
    .rk.exec"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
off:{.rk.exec "dev.off()"}
new:{.rk.exec "dev.new(noRStudioGD=TRUE)"}

\d .

// The following block of functions are aliases for the above
// these aliases will be removed in a future version of the code

Rclose  :.rk.close
Rcmd    :.rk.exec
Rget    :.rk.get
Rinstall:.rk.install
Rnew    :.rk.new
Roff    :.rk.off
Rset    :.rk.set  

setenv[`R_HOME;first @[system;@[.z.o like "w*";"call";"env"]," R RHOME";enlist""]]
