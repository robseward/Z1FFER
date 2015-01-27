import matplotlib.pyplot as plot
import matplotlib.dates as md
from matplotlib.dates import date2num
import datetime

f = open("deviations2.csv")

values = []
timestamps = []
for (i, line) in enumerate(f):
    if i >= 1:
    	lineArray = line.split(",")
    	date = datetime.datetime.strptime(lineArray[0], '%Y-%m-%d %H:%M:%S')
    	timestamps.append(date2num(date))
        value = lineArray[1].strip()
        values.append(value)
    if i > 100000:
        break
plot.subplots_adjust(bottom=0.2)
plot.xticks( rotation=25 )
ax=plot.gca()
xfmt = md.DateFormatter('%Y-%m-%d %H:%M:%S')
ax.xaxis.set_major_formatter(xfmt)
plot.plot(timestamps, values)
plot.show()
