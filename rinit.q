/ R server for Q

Rclose:`embedr 2:(`rclose;1)
Ropen:`embedr 2:(`ropen;1)
Rcmd0:`embedr 2:(`rcmd;1)
Rget0:`embedr 2:(`rget;1)
Rset0:`embedr 2:(`rset;2)
Rcmd:Rcmd0;
Rget:Rget0;
Rset:Rset0;

Rinstall:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];rcloud:"https://cloud.r-project.org";
  if[0i=first Rget"is.element('",pkg,"',installed.packages()[,1])";
    Rcmd"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
Roff:{Rcmd "dev.off()"}
Rnew:{Rcmd "dev.new(noRStudioGD=TRUE)"}
setenv[`R_HOME;first @[system;@[.z.o like "w*";"call";"env"]," R RHOME";enlist""]]
