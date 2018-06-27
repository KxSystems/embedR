/ R server for Q

Rclose:`embedr 2:(`rclose;1)
Ropen:`embedr 2:(`ropen;1)
Rcmd0:`embedr 2:(`rcmd;1)
Rget0:`embedr 2:(`rget;1)
Rset0:`embedr 2:(`rset;2)
Rcmd:{Rcmd0 x}
Rget:{r:Rget0 x;Rconv r}
Rset:{Rset0[x;y]}

Rconvmap:()!()
Rconvmap[enlist "Date"]:{-10957+`date$last x}
Rconvmap[enlist "POSIXt"]:{(-10957D)+`timestamp$1e9*last x}
Rconvmap[enlist "data.frame"]:{
  a:first x;if[0=count a`names;:last x];
  r:flip ((),`$a`names)!Rconv each  last x;
  r[`row.names]:$[null first rn:a`row.names;1+til last neg rn;rn];
  r}
Rconvmap[enlist "factor"]:{`$x[0;`levels] -1+last x}
Rconv:{
  if[(2<>count x) or 99<>type first x;:x]; // no attrs
  c:first[x]`class;
  if[10=type c;c:enlist c];c:c where 10h=type each c;
  first[asc (),Rconvmap[c]] x
  }
Rinstall:{[pkg] 
  pkg:$[-11=type pkg;string pkg;pkg];rcloud:"https://cloud.r-project.org";
  if[0i=first Rget"is.element('",pkg,"',installed.packages()[,1])";
    Rcmd"install.packages('",pkg,"',repos='",rcloud,"',dependencies = TRUE)"];
  }
Roff:{Rcmd "dev.off()"}
Rnew:{Rcmd "dev.new(noRStudioGD=TRUE)"}
setenv[`R_HOME;first @[system;@[.z.o like "w*";"call";"env"]," R RHOME";enlist""]]
