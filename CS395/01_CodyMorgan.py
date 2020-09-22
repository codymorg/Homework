print("Q4")
sqaures = []
cubes = []
fifths = []
for i in range(1,20000):
  sqaures.append(i*i*2)
  cubes.append(i*i*i*3)
  fifths.append(i*i*i*i*5)
print("a.")
for i in sqaures:
  if i in cubes:
    print(i)