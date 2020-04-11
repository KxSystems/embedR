/ R server for Q

.rk.close:`embedr 2:(`rclose;1)
.rk.open:`embedr 2:(`ropen;1)
.rk.exec:`embedr 2:(`rcmd;1)
.rk.get:`embedr 2:(`rget;1)
.rk.set:`embedr 2:(`rset;2)
//Rcmd:Rcmd0;
//Rget:Rget0;
//Rset:Rset0;

.rk.install:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];rcloud:"https://cloud.r-project.org";
  if[0i=first .rk.get"is.element('",pkg,"',installed.packages()[,1])";
    .rk.exec"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
Roff:{.rk.exec "dev.off()"}
Rnew:{.rk.exec "dev.new(noRStudioGD=TRUE)"}
setenv[`R_HOME;first @[system;@[.z.o like "w*";"call";"env"]," R RHOME";enlist""]]
