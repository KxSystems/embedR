\l ../init.q

basedir:`:.^hsym `$last -2 _ get{}
datafile:` sv first[` vs basedir],`pcd2014v1.csv
if[()~key datafile;'"Data file not found. See README.md"]

// switch parsing to british data
system"z 1"
cct:("***D*DE";enlist csv) 0:datafile
cct:(`$"_"^string cols cct) xcol cct

.rk.exec"library(lattice)"
.rk.new[]
.rk.set["tpd";select txn:count i by Transaction_Date from cct]
.rk.exec"plot1<-xyplot(txn~Transaction_Date,data=tpd,main='Payments per day')"
.rk.exec"print(plot1)"


.rk.set[`tpt;select Transaction_Date,spent:sums JV_Value from select sum JV_Value by Transaction_Date from cct]
.rk.new[]
.rk.exec"plot2<-xyplot(spent~Transaction_Date,data=tpt,main='Total spent')"
.rk.exec"print(plot2)"

.rk.install`plotly
.rk.exec"library(plotly)"
.rk.set[`catd;select sum JV_Value by Service_Area from cct]
.rk.exec"plot3<-plot_ly(catd, labels = ~Service_Area, values = ~JV_Value, type = 'pie')"
.rk.exec"print(plot3)"
