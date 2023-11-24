import matplotlib.pyplot as plt
import seaborn as sns
import numpy as np

file = open("adj_matrix", "r") #open adj_matrix file for reading

n = int(file.readline()) #read first line of file, which contains the value of n (aka. the number of vertices in the graph)

NUM_ITERATIONS = n*n #this will help us determine the length of the file. this value comes from code.c

adj_matrix = [[0]*n]*n

for k in range(0,NUM_ITERATIONS*n,n*n):
    for i in range(n):
        adj_matrix[i] = list(file.readline().replace(" ", "").replace("\n", ""))
        for j in range(n):
            adj_matrix[i][j] = int(adj_matrix[i][j])

    ax = sns.heatmap(adj_matrix, linewidth=0.5, cbar=False)
    plt.savefig('{0}.png'.format(k/(n*n)), dpi=1000, bbox_inches='tight')

# generate 2 2d grids for the x & y bounds
#y, x = np.meshgrid(np.linspace(-3, 3, 100), np.linspace(-3, 3, 100))

#z = (1 - x / 2. + x ** 5 + y ** 3) * np.exp(-x ** 2 - y ** 2)
# x and y are bounds, so z should be the value *inside* those bounds.
# Therefore, remove the last value from the z array.
#z = z[:-1, :-1]
#z_min, z_max = -np.abs(z).max(), np.abs(z).max()

#fig, ax = plt.subplots()

#c = ax.pcolormesh(x, y, z, cmap='RdBu', vmin=z_min, vmax=z_max)
#ax.set_title('pcolormesh')
# set the limits of the plot to the limits of the data
#ax.axis([x.min(), x.max(), y.min(), y.max()])
#fig.colorbar(c, ax=ax)

file.close()

#plt.savefig('test_fig.png', dpi=1000, bbox_inches='tight')
