#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>

void init(int, int, int, double, double, int, int***, int***);
void ng(int, int, int, int, int***, int***, int);
void rewire(int, int, int, int, int***, int***, int);
int is_game_over(int, int, int, int, int***, int***, int);
int num_components(int, int***);
void DFS(int, int, int**, int***);
void print_info(int, int, int***, int***);
void write_to_file(int, int, int***, int***, int);

//The planted partition model partitions a graph with n=c*k vertices
//in c communities with k vertices each. Two vertices within the same
//community share an edge with probability p, and two vertices within
//different communities share an edge with probability q. Here we want
//an assortative model where p > q.

void init(int c, int k, int n, double p, double q, int opinions, int*** adj_matrix, int*** opinion_matrix){

	int num_cross_connections = 0;
	int num_within_connections = 0;
	int opinion_divergence = 0;

	//create planted partition adjacency matrix
	for (int i = 0; i < n; i+=k){ //for each community
		for (int j = i; j < i+k; j++){ //for each member within the community
			for (int m = j+1; m < n; m++){ //for each vertex of the graph that is greater than yourself
				if (m < i+k){ //if the vertex is in your community
					if (((double)rand()/(double)(RAND_MAX)) < p){
						(*adj_matrix)[j][m] = 1;
						(*adj_matrix)[m][j] = 1;
						num_within_connections++;
					}
					else {
						(*adj_matrix)[j][m] = 0;
						(*adj_matrix)[m][j] = 0;
					}
				}
				else { //if the vertex is not in your community
					if (((double)rand()/(double)(RAND_MAX)) < q){
						(*adj_matrix)[j][m] = 1;
						(*adj_matrix)[m][j] = 1;
						num_cross_connections++;
					}
					else {
						(*adj_matrix)[j][m] = 0;
						(*adj_matrix)[m][j] = 0;
					}
				}
			}
			//set & encode opinions
			if (((double)rand()/(double)(RAND_MAX)) < p){
				(*opinion_matrix)[j][0] = i/k;
			}
			else{
				(*opinion_matrix)[j][0] = i/k;
				while ((*opinion_matrix)[j][0] == i/k){ //so that the opinion is NOT i/k
					(*opinion_matrix)[j][0] = rand() % opinions;
				}
				opinion_divergence++;
			}
		}
	}

	write_to_file(n, opinions, adj_matrix, opinion_matrix, 1);

	FILE *meta;
	meta = fopen("meta", "a");
	fprintf(meta, "\nnum_cross_connections = %d\n", num_cross_connections);
	fprintf(meta, "avg_num_cross_connections = %f\n", ((float)(num_cross_connections))/c);
	fprintf(meta, "avg_num_within_connections = %f\n", ((float)(num_within_connections))/c);
	fprintf(meta, "avg_opinion_divergence = %f\n", ((float)(opinion_divergence))/c);
	fprintf(meta, "\nadj_matrix_initial:\n");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			fprintf(meta, "%d ", (*adj_matrix)[i][j]);
		}
		fprintf(meta, "\n");
	}
	fprintf(meta, "\nopinion_matrix_initial:\n");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < opinions; j++){
			fprintf(meta, "%d ", (*opinion_matrix)[i][j]);
		}
		fprintf(meta, "\n");
	}
	fprintf(meta, "\nnum_components_initial = %d\n", num_components(n, adj_matrix));
	fclose(meta);

}

void ng(int c, int k, int n, int opinions, int*** adj_matrix, int*** opinion_matrix, int rewire_strength){

	int timestep = 0;

	while (!is_game_over(c, k, n, opinions, adj_matrix, opinion_matrix, rewire_strength)){
		//play the naming game

		timestep++;

		int speaker = rand() % n; //pick a random vertex as speaker

		//select a random neighbour of the vertex as hearer
		//here we go through the speaker's neighbours, count them, then produce a rand int between 0 and number of neighbours inclusive
		//then go through the speaker's neighbours again, count them, stop at the rand int. O(n)
		int speaker_num_neighbours = 0;
		for (int i = 0; i < n; i++){
			if ((*adj_matrix)[speaker][i] == 1){
				speaker_num_neighbours++;
			}
		}

		if (speaker_num_neighbours == 0){
			rewire(c, k, n, opinions, adj_matrix, opinion_matrix, rewire_strength);
			continue; //if speaker has no neighbours, skip this round of ng, don't skip rewire
			//this way, at every time step:
			//we play the ng (if speaker has no neighbours, so be it)
			//we rewire (if rewiring is impossible, so be it)
		}

		int hearer = rand() % speaker_num_neighbours;

		int neighbour_count = 0;
		for (int i = 0; i < n; i++){
			if ((*adj_matrix)[speaker][i] == 1){
				neighbour_count++;
			}
			if ((neighbour_count - 1) == hearer){
				hearer = i;
			}
		}

		//speaker selects a random opinion from its set of opinions
		int speaker_num_ops = 0;
		int hearer_num_ops = 0;
		for (int i = 0; i < opinions; i++){
			if ((*opinion_matrix)[speaker][i] > -1){
				speaker_num_ops++; //count the number of opinions the speaker has
			}
			if ((*opinion_matrix)[hearer][i] > -1){
				hearer_num_ops++; //count the number of opinions the hearer has
			}
			if ((*opinion_matrix)[speaker][i] == -1 && (*opinion_matrix)[hearer][i] == -1) break;
			//opinions are arranged from left to right. if we hit a -1 then all subsequent opinions are -1
		}

		int op = (*opinion_matrix)[speaker][rand() % speaker_num_ops]; //select random opinion from non-negative opinions

		if (op == -1){
			printf("---- exit 02 ----\n");
			//opinion should not be -1 as -1 is representative of an empty opinion slot
			exit(0);
		}
		//check for success
		//update opinions based upon the success of the exchange
		int success = 0;
		for (int i = 0; i < opinions; i++){
			if ((*opinion_matrix)[hearer][i] == op){
				success = 1;
				(*opinion_matrix)[speaker][0] = op;
				(*opinion_matrix)[hearer][0] = op;
				for (int j = 1; j < opinions; j++){
					(*opinion_matrix)[speaker][j] = -1;
					(*opinion_matrix)[hearer][j] = -1;
				}
				break;
			}
		}
		if (success == 0){
			if (hearer_num_ops >= opinions){
				printf("---- exit 03 ----\n");
				//speaker_num_ops or hearer_num_ops should never be more than opinions
				//if we are in this if statement, it's because the hearer did not have the opinion in its opinion set which means
				//its number of opinions has to be at most (opinions-1)
				exit(0);
			}
			(*opinion_matrix)[hearer][hearer_num_ops] = op; //update hearer opinion set
		}

		rewire(c, k, n, opinions, adj_matrix, opinion_matrix, rewire_strength);
	}

	write_to_file(n, opinions, adj_matrix, opinion_matrix, 0);

	FILE *meta;
	meta = fopen("meta", "a");
	fprintf(meta, "\nadj_matrix_final:\n");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			fprintf(meta, "%d ", (*adj_matrix)[i][j]);
		}
		fprintf(meta, "\n");
	}
	fprintf(meta, "\nopinion_matrix_final:\n");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < opinions; j++){
			fprintf(meta, "%d ", (*opinion_matrix)[i][j]);
		}
		fprintf(meta, "\n");
	}
	fprintf(meta, "\nnum_components_final = %d\n", num_components(n, adj_matrix));
	fprintf(meta, "\ntimesteps = %d\n", timestep);
	fclose(meta);

}

void rewire(int c, int k, int n, int opinions, int*** adj_matrix, int*** opinion_matrix, int rewire_strength){

	//if rewire_strength=1 then this is 'weak' rewire, if rewire_strength = opinions then this is 'strong' rewire
	//if rewire_strength=0 then no rewire is executed
	//maybe we could define 'weak' and 'strong' rewire as a function of the number of opinions
	//(like if rewire_strength < opinions/4 then 'weak' if rewire_strength > 3*opinions/4 then 'strong')

	//allocate memory for eligible_for_edge_removal array
	int** eligible_for_edge_removal = (int**)malloc(n*n * sizeof(int*));
	for (int i = 0; i < n*n; i++){
		eligible_for_edge_removal[i] = (int*)malloc(2 * sizeof(int));
	}

	//allocate memory for eligible_for_edge_removal array
	int** eligible_for_edge_addition = (int**)malloc(n*n * sizeof(int*));
	for (int i = 0; i < n*n; i++){
		eligible_for_edge_addition[i] = (int*)malloc(2 * sizeof(int));
	}

	//declare some variables
	int num_common_ops = 0;
	int num_eligible_pairs_removal = 0;
	int num_eligible_pairs_addition = 0;

	//I've had the idea of rewiring an edge having an endpoint whose opinion(s) recently changed
	//not sure whether this would be interesting...

	for (int i = 0; i < n; i++){ //for each vertex
		for (int j = i+1; j < n; j++){ //for each potential neighbour of the vertex, without looking at the same pair twice
			//count the number of shared opinions
			num_common_ops = 0;
			for (int k = 0; k < opinions; k++){
				for (int m = 0; m < opinions; m++){
					if ((*opinion_matrix)[i][k] == (*opinion_matrix)[j][m] && (*opinion_matrix)[i][k] > -1){
						//if we find i and j share a same opinion (excluding -1 opinions)
						num_common_ops++;
					}
				}
			}
			if ((*adj_matrix)[i][j] == 1){ //if the pair are neighbours
				if (num_common_ops < rewire_strength){ //if vertices i and j share less than rewire_strength opinions
					//pair is eligible for edge removal
					//add pair to eligible_for_edge_removal array
					eligible_for_edge_removal[num_eligible_pairs_removal][0] = i;
					eligible_for_edge_removal[num_eligible_pairs_removal][1] = j;
					num_eligible_pairs_removal++;
				}
			}
			else{ //if the pair are not neighbours
				if (num_common_ops >= rewire_strength){ //if vertices i and j share rewire_strength opinions or more
					//pair is eligible for edge addition
					//add pair to eligible_for_edge_addition array
					eligible_for_edge_addition[num_eligible_pairs_addition][0] = i;
					eligible_for_edge_addition[num_eligible_pairs_addition][1] = j;
					num_eligible_pairs_addition++;
				}
			}
		}
	}

	//num_eligible_pairs_removal & num_eligible_pairs_addition have to be at least 1 each for us to be able to rewire
	if (num_eligible_pairs_removal > 0 && num_eligible_pairs_addition > 0){
		//rewire is possible

		//pick a random pair from eligible_for_edge_removal
		int pair_to_remove = rand() % num_eligible_pairs_removal;
		//remove the edge by changing adj_matrix at the two appropriate indices
		(*adj_matrix)[eligible_for_edge_removal[pair_to_remove][0]][eligible_for_edge_removal[pair_to_remove][1]] = 0;
		(*adj_matrix)[eligible_for_edge_removal[pair_to_remove][1]][eligible_for_edge_removal[pair_to_remove][0]] = 0;

		//pick a random pair from eligible_for_edge_addition
		int pair_to_add = rand() % num_eligible_pairs_addition;
		//add the edge by changing adj_matrix at the two appropriate indices
		(*adj_matrix)[eligible_for_edge_addition[pair_to_add][0]][eligible_for_edge_addition[pair_to_add][1]] = 1;
		(*adj_matrix)[eligible_for_edge_addition[pair_to_add][1]][eligible_for_edge_addition[pair_to_add][0]] = 1;
	}

	//if rewire is not possible, do nothing

	//we'll have to think of a way to handle vertices with number of non -1 opinions which is less than rewire_strength
	//this is handled above. it'll just fall under the "eligible for edge removal" category
	//it's a bit weird to have it set up like that though... maybe speak to Alan about it

	//step 1: remove an existing edge between two vertices which share less than rewire_strength opinions
	//go through all vertices one by one, check if they share less than rewire_strength opinions with each of their neighbours
	//each pair that shares less than rewire_strength opinions between them is eligible for edge removal
	//store these in a 2D array eligible_for_edge_removal
	//each pair that shares rewire_strength or more opinions between them is NOT eligible for edge removal

	//step 2: add a non-existing edge between two vertices which share rewire_strength opinions or more
	//go through all vertices one by one, check if they share rewire_strength opinions or more with each of their non-neighbours
	//each pair that shares rewire_strength opinions or more between them is eligible for edge addition
	//store these in a 2D array eligible_for_edge_addition
	//each pair that shares less than rewire_strength opinions between them is NOT eligible for edge addition

	//as long as (there exists a pair of vertices that share less than rewire_strength opinions between them AND share an edge) AND
	//(a pair of vertices that share rewire_strength or more opinions between them AND do not share an edge) then we can rewire
	//make sure to check this before proceeding

	//pick a random pair from eligible_for_edge_removal
	//remove the edge by changing adj_matrix at the two appropriate indices

	//pick a random pair from eligible_for_edge_addition
	//add the edge by changing adj_matrix at the two appropriate indices

	//free memory for eligible_for_edge_removal & eligible_for_edge_addition arrays
	for (int i = 0; i < n*n; i++){
		free(eligible_for_edge_removal[i]);
		free(eligible_for_edge_addition[i]);
	}
	free(eligible_for_edge_removal);
	free(eligible_for_edge_addition);

}

int is_game_over(int c, int k, int n, int opinions, int*** adj_matrix, int*** opinion_matrix, int rewire_strength){

	//the game is over if:
	//for every vertex with at least one neighbour (i.e. this doesn't apply to disconnected vertices)
	//that vertex has to have a single opinion AND
	//that vertex's neighbours must all have that same opinion

	for (int i = 0; i < n; i++){ //for every vertex
		for (int j = 0; j < n; j++){ //for every potential neighbour of vertex
			if ((*adj_matrix)[i][j] == 1){ //if they are neighbours
				if (((*opinion_matrix)[i][0] != (*opinion_matrix)[j][0]) || ((*opinion_matrix)[i][1] != -1 || (*opinion_matrix)[j][1] != -1)){
					//if opinions of vertex i and j are the not the same OR
					//if someone has more than one opinion (which are alway arranged from left to right)
					//then the game is not over
					return 0;
				}
			}
		}
	}

	//if we're here, it's because the game is not not over
	return 1;

}

int num_components(int n, int*** adj_matrix){

	int* visited = (int*)malloc(n * sizeof(int)); //create visited array
	for (int i = 0; i < n; i++){ //set all slots to false
		visited[i] = 0;
	}

	int num_components = 0;

	for (int i = 0; i < n; i++){
		if (visited[i] == 0){
			DFS(n, i, &visited, adj_matrix);
			num_components++;
		}
	}

	return num_components;

}

void DFS(int n, int v, int** visited, int*** adj_matrix){

	(*visited)[v] = 1;
	for (int j = 0; j < n; j++){
		if ((*adj_matrix)[v][j] == 1 && !(*visited)[j]){
			DFS(n, j, visited, adj_matrix);
		}
	}

}

void print_info(int n, int opinions, int*** adj_matrix, int*** opinion_matrix){

	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			printf("%d ", (*adj_matrix)[i][j]);
		}
		printf("\n");
	}
	for (int i = 0; i < n; i++){
		for (int j = 0; j < opinions; j++){
			printf("%d ", (*opinion_matrix)[i][j]);
		}
		printf("\n");
	}

}

void write_to_file(int n, int opinions, int*** adj_matrix, int*** opinion_matrix, int initial){

	if (initial){
		FILE *adj_matrix_file_initial;
		adj_matrix_file_initial = fopen("adj_matrix_initial", "a");
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				fprintf(adj_matrix_file_initial, "%d ", (*adj_matrix)[i][j]);
			}
			fprintf(adj_matrix_file_initial, "\n");
		}
		fclose(adj_matrix_file_initial);

		FILE *opinion_matrix_file_initial;
		opinion_matrix_file_initial = fopen("opinion_matrix_initial", "a");
		for (int i = 0; i < n; i++){
			for (int j = 0; j < opinions; j++){
				fprintf(opinion_matrix_file_initial, "%d ", (*opinion_matrix)[i][j]);
			}
			fprintf(opinion_matrix_file_initial, "\n");
		}
		fclose(opinion_matrix_file_initial);
	}

	else{
		FILE *adj_matrix_file_final;
		adj_matrix_file_final = fopen("adj_matrix_final", "a");
		for (int i = 0; i < n; i++){
			for (int j = 0; j < n; j++){
				fprintf(adj_matrix_file_final, "%d ", (*adj_matrix)[i][j]);
			}
			fprintf(adj_matrix_file_final, "\n");
		}
		fclose(adj_matrix_file_final);

		FILE *opinion_matrix_file_final;
		opinion_matrix_file_final = fopen("opinion_matrix_final", "a");
		for (int i = 0; i < n; i++){
			for (int j = 0; j < opinions; j++){
				fprintf(opinion_matrix_file_final, "%d ", (*opinion_matrix)[i][j]);
			}
			fprintf(opinion_matrix_file_final, "\n");
		}
		fclose(opinion_matrix_file_final);
	}

}


int main(){

	srand(time(NULL));

	//set parameters
	int c = 2; //number of communities
	int k = 4; //number of vertices within each community
	double p = 0.8; //probability of an edge between members of the same community
	double q = 0.04; //probability of an edge between members of different communities
	int n = c*k; //total number of vertices
	int opinions = c; //total number of opinions in the system
	int rewire_strength = 1; //rewire_strength will always be set to 1

	//allocate memory for adjacency matrix
	int** adj_matrix = (int**)malloc(n * sizeof(int*));
	for (int i = 0; i < n; i++){
		adj_matrix[i] = (int*)malloc(n * sizeof(int));
	}

	//allocate memory for agent opinions and initialize opinions to -1
	int** opinion_matrix = (int**)malloc(n * sizeof(int*));
	for (int i = 0; i < n; i++){
		opinion_matrix[i] = (int*)malloc(opinions * sizeof(int));
		for (int j = 0; j < opinions; j++){
			opinion_matrix[i][j] = -1;
		}
	}

	FILE *meta;
	meta = fopen("meta", "w");
	fprintf(meta, "c = %d\n", c);
	fprintf(meta, "k = %d\n", k);
	fprintf(meta, "p = %f\n", p);
	fprintf(meta, "q = %f\n", q);
	fprintf(meta, "n = %d\n", n);
	fprintf(meta, "opinions = %d\n", opinions);
	fprintf(meta, "rewire_strength = %d\n", rewire_strength);
	fclose(meta);

	FILE *adj_matrix_file_initial;
	adj_matrix_file_initial = fopen("adj_matrix_initial", "w");
	fprintf(adj_matrix_file_initial, "%d\n", n);
	fclose(adj_matrix_file_initial);

	FILE *opinion_matrix_file_initial;
	opinion_matrix_file_initial = fopen("opinion_matrix_initial", "w");
	fprintf(opinion_matrix_file_initial, "%d\n", n);
	fprintf(opinion_matrix_file_initial, "%d\n", opinions);
	fclose(opinion_matrix_file_initial);

	FILE *adj_matrix_file_final;
	adj_matrix_file_final = fopen("adj_matrix_final", "w");
	fprintf(adj_matrix_file_final, "%d\n", n);
	fclose(adj_matrix_file_final);

	FILE *opinion_matrix_file_final;
	opinion_matrix_file_final = fopen("opinion_matrix_final", "w");
	fprintf(opinion_matrix_file_final, "%d\n", n);
	fprintf(opinion_matrix_file_final, "%d\n", opinions);
	fclose(opinion_matrix_file_final);

	init(c, k, n, p, q, opinions, &adj_matrix, &opinion_matrix);

	ng(c, k, n, opinions, &adj_matrix, &opinion_matrix, rewire_strength);

	//free memory
	for (int i = 0; i < n; i++){
		free(adj_matrix[i]);
	}
	free(adj_matrix);
	for (int i = 0; i < opinions; i++){
		free(opinion_matrix[i]);
	}
	free(opinion_matrix);

}