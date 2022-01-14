import random

n = 1500000
m = 8

f = open("generated1_5m.csv", "w")
for i in range(m):
    f.write("att" + str(i) + (", " if i != m-1 else "\n"))
for i in range(n):
    for j in range(m):
        f.write(str(round(random.uniform(-999, 999), 3)) +
                (", " if j != m-1 else "\n"))
f.close()
