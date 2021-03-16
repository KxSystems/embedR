\l ../init.q

basedir:`:.^hsym `$last -2 _ get{}
datafile:` sv first[` vs basedir],`pcd2014v1.csv
if[()~key datafile;'"Data file not found. See README.md"]

// switch parsing to british data
system"z 1"
cct:("***D*DE";enlist csv) 0:datafile
cct:(`$"_"^string cols cct) xcol cct

.r.exec"library(lattice)"
.r.new[]
.r.set["tpd";select txn:count i by Transaction_Date from cct]
.r.exec"plot1<-xyplot(txn~Transaction_Date,data=tpd,main='Payments per day')"
.r.exec"print(plot1)"


.r.set[`tpt;select Transaction_Date,spent:sums JV_Value from select sum JV_Value by Transaction_Date from cct]
.r.new[]
.r.exec"plot2<-xyplot(spent~Transaction_Date,data=tpt,main='Total spent')"
.r.exec"print(plot2)"

.r.install`plotly
.r.exec"library(plotly)"
.r.set[`catd;select sum JV_Value by Service_Area from cct]
.r.exec"plot3<-plot_ly(catd, labels = ~Service_Area, values = ~JV_Value, type = 'pie')"
.r.exec"print(plot3)"
