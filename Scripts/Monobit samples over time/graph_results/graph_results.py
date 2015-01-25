import matplotlib.pyplot as plot

f = open("deviations.csv")

values = []
for (i, line) in enumerate(f):
    if i >= 1:
        value = line.split(",")[1].strip()
        values.append(value)
    if i > 100000:
        break

plot.plot(values)
plot.show()
