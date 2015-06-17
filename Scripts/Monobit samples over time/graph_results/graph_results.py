import matplotlib.pyplot as plot
import matplotlib.dates as md
from matplotlib.dates import date2num
import datetime
# from pylab import *
from numpy import polyfit
import numpy as np

f = open("20150315_deviations.csv")

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

# countArray = np.arange(0.0, len(timestamps))
floatValues = np.array(map(float, values))
fit = polyfit(timestamps,floatValues,1)
fit_fn = np.poly1d(fit) # fit_fn is now a function which takes in x and returns an estimate for y

# plot(x,y, 'yo', x, fit_fn(x), '--k')

plot.plot(timestamps, values, timestamps, fit_fn(timestamps), '--k')
#plot.plot(timestamps, values)
plot.show()
