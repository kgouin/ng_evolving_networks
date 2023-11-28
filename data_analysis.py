import numpy as np
import matplotlib.pyplot as plt
import networkx as nx

adj_file = open("adj_matrix", "r") #open adj_matrix file for reading
opinion_file = open("opinion_matrix", "r") #open opinion_matrix file for reading

n = int(adj_file.readline()) #read first line of either file, which contains the value of n
num_iterations = int(adj_file.readline()) #read second line of either file, which contains the number of iterations performed in the ng

opinion_file.readline() #read first (redundant) line of opinion_file
opinion_file.readline() #read second (redundant) line of opinion_file
opinions = int(opinion_file.readline()) #read third line of opinion_file, which contains the number of possible opinions in the system

adj_matrix = [[0]*n]*n
opinion_matrix = [[0]*opinions]*n

g = nx.Graph() #create graph

for i in range(n): #set graph vertices
    g.add_node(i)

#pos = nx.spring_layout(g, seed=3113794652)
#nx.spring_layout(g, pos=pos, fixed=g.nodes)
#nx.draw_networkx_nodes(g, pos, node_color='k', node_size=2)

for i in range(0, num_iterations, n):

    dictionary = {}

    for j in range(n):
        adj_matrix[j] = [int(num) for num in adj_file.readline().split(' ')[:-1]] #encode adjacency matrix
        opinion_matrix[j] = [int(num) for num in opinion_file.readline().split(' ')[:-1]] #encode opinion matrix

    for j in range(n):
        temp_array = []
        for k in range(opinions):
            if opinion_matrix[j][k] != -1:
                temp_array.append(opinion_matrix[j][k]) #for each vertex, look at its non-empty opinions
        dictionary[j] = temp_array #add these opinions to a dictionary for correct labeling of graph vertices
        for k in range(i, n):
            if adj_matrix[j][k] == 1:
                g.add_edge(j,k) #add edges of the graph

    #nx.draw_networkx_edges(g, pos, width=0.1, edge_color='k')
    #nx.draw_networkx_labels(g, pos, labels=dictionary, font_size=10, font_color='k', font_family='sans-serif', font_weight='normal', alpha=None, bbox=None, horizontalalignment='center', verticalalignment='center', ax=None, clip_on=True)
    nx.draw(g, with_labels=True, labels=dictionary, node_color='white', width=0.1, node_size=4, font_size=6)
    plt.savefig('fig_iteration_'+str(i)+'.png', dpi=1000, bbox_inches='tight')

    g.clear_edges()

adj_file.close()
opinion_file.close()