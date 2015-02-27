import numpy
import matplotlib.pyplot as plt

data=numpy.genfromtxt('pset1_out_tallies_0001.csv', delimiter=",")

plt.plot(data[:,0], data[:,1], label='')

plt.xlabel('Domain Length (cm)')
plt.ylabel('Collision Rate Average #/cm^2')

plt.title('Collision Rate')

plt.show()

plt.clf()
plt.cla()



plt.plot(data[:,0], data[:,2], label='')

plt.xlabel('Domain Length (cm)')
plt.ylabel('#/cm^2')

plt.title('Flux')

plt.show()

plt.clf()
plt.cla()




plt.plot(data[:,0], data[:,3], label='')

plt.xlabel('Domain Length (cm)')
plt.ylabel('Average #/cm')

plt.title('Mean')

plt.show()

plt.clf()
plt.cla()





plt.plot(data[:,0], data[:,4], label='')

plt.xlabel('Domain Length (cm)')
plt.ylabel('Variance')

plt.title('Variance')

plt.show()

plt.clf()
plt.cla()
