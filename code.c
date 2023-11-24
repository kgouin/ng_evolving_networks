#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>

void init(int, int, int, double, double, int, int***, int***);
void ng(int, int, int, int, int***, int***, int);
void rewire(int, int, int, int, int***, int***, int);
//void modify(int, int, int, int, int***, int***, double, double, double);
void write_to_file(int, int, int***, int***);

//The planted partition model partitions a graph with n=c*k vertices
//in c communities with k vertices each. Two vertices within the same
//community share an edge with probability p, and two vertices within
//different communities share an edge with probability q. Here we want
//an assortative model where p > q.

void init(int c, int k, int n, double p, double q, int opinions, int*** adj_matrix, int*** opinion_matrix){

	//create planted partition adjacency matrix
	for (int i = 0; i < n; i+=k){ //for each community
		for (int j = i; j < i+k; j++){ //for each member within the community
			for (int m = j+1; m < n; m++){ //for each vertex of the graph that is greater than yourself
				if (m < i+k){ //if the vertex is in your community
					if (((double)rand()/(double)(RAND_MAX)) < p){
						(*adj_matrix)[j][m] = 1;
						(*adj_matrix)[m][j] = 1;
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
				(*opinion_matrix)[j][0] = rand() % opinions;
			}
		}
	}

	write_to_file(n, opinions, adj_matrix, opinion_matrix);

}

void ng(int c, int k, int n, int opinions, int*** adj_matrix, int*** opinion_matrix, int rewire_strength){

	for (k = 0; k < n*n; k++){
		//play the naming game
		int speaker = rand() % n;	//pick a random vertex as speaker (uniformity is not perfect, but it's good enough for our purposes)

		//select a random neighbour of the vertex as hearer
		//here we go through the speaker's neighbours, count them, then produce a rand int between 0 and number of neighbours inclusive
		//then go through the speaker's neighbours again, count them, stop at the rand int. O(n)
		int speaker_num_neighbours = 0;
		for (int i = 0; i < n; i++){
			if ((*adj_matrix)[speaker][i] == 1){
				speaker_num_neighbours++;
			}
		}

		if (speaker_num_neighbours == 0) continue; //if speaker has no neighbours, skip this round of ng, skip rewire
		//should we really skip rewire?

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

		write_to_file(n, opinions, adj_matrix, opinion_matrix);
	}

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
		(*adj_matrix)[eligible_for_edge_addition[pair_to_add][0]][eligible_for_edge_addition[pair_to_add][1]] = 0;
		(*adj_matrix)[eligible_for_edge_addition[pair_to_add][1]][eligible_for_edge_addition[pair_to_add][0]] = 0;
	}

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

/*void modify(int c, int k, int n, int opinions, int*** adj_matrix, int*** opinion_matrix, double prob_link, double prob_drop, double prob_none){

	//modify the given adjacency matrix by adding or dropping a link between two vertices based upon their shared opinions

	int v = rand() % n; //pick a random vertex

	int possible_candidates = 0;
	int disqualified = 0;
	int qualified = 0;

	//printf("RAND_MAX = %d\n", RAND_MAX); //these look good
	//printf("(double)RAND_MAX = %f\n", (double)RAND_MAX); //these look good

	double prob = (double)rand()/(double)(RAND_MAX); //random number between 0 and 1
	//printf("prob = %f\n", prob); //these look good
	//printf("prob_link = %f\n", prob_link); //these look good
	//printf("prob_drop = %f\n", prob_drop); //these look good

	if (prob > 1 || prob < 0){
		printf("---- exit 04 ----\n");
		//prob should be between 0 and 1
		exit(0);
	}
	
	//printf("prob_link < prob_drop: %f < %f\n", prob_link, prob_drop); //these look good
	//printf("prob_drop < prob_link: %f < %f\n", prob_drop, prob_link); //these look good

	double case_link = -1;
	double case_drop = -1;
	double case_none = -1;

	//setup for probability check
	int possibility = -1;
	if (prob_link < prob_drop < prob_none){
		possibility = 1;
		case_link = prob_link;
		case_drop = prob_link + prob_drop;
		case_none = 1;
	}
	else if (prob_drop < prob_link < prob_none){
		possibility = 2;
		case_drop = prob_drop;
		case_link = prob_drop + prob_link;
		case_none = 1;
	}
	else if (prob_link < prob_none < prob_drop){
		possibility = 3;
		case_link = prob_link;
		case_none = prob_link + prob_none;
		case_drop = 1;
	}
	else if (prob_drop < prob_none < prob_link){
		possibility = 4;
		case_drop = prob_drop;
		case_none = prob_drop + prob_none;
		case_link = 1;
	}
	else if (prob_none < prob_link < prob_drop){
		possibility = 5;
		case_none = prob_none;
		case_link = prob_none + prob_link;
		case_drop = 1;
	}
	else if (prob_none < prob_drop < prob_link){
		possibility = 6;
		case_none = prob_none;
		case_drop = prob_none + prob_drop;
		case_link = 1;
	}
	else {
		printf("---- exit 05 ----\n");
		//there shouldn't be any more cases
		exit(0);
	}

	if (case_link == -1 || case_drop == -1 || case_none == -1 || possibility == -1){
		printf("---- exit 06 ----\n");
		//these values should have changed
		exit(0);
	}

	if ((possibility == 1 && prob < case_link) || (possibility == 2 && prob > case_drop && prob < case_link) ||
		(possibility == 3 && prob < case_link) || (possibility == 4 && prob > case_drop && prob > case_none) ||
		(possibility == 5 && prob > case_none && prob < case_link) || (possibility == 6 && prob > case_none && prob > case_drop)){
		//with probability prob_link, we add a link between previously unlinked nodes iff they share at least 1 common opinion
		printf("modify: add a link\n");
		for (int i = 0; i < n; i++){ //go through v's neighbours
			if ((*adj_matrix)[v][i] == 0){ //if we find a non-neighbour
				for (int j = 0; j < opinions; j++){ //look through v's opinions
					for (int k = 0; k < opinions; k++){ //look through v's non-neighbour's opinions
						if ((*opinion_matrix)[v][j] == (*opinion_matrix)[v][k]){ //if they share an opinion
							qualified = 1;
						}
					}
				}
				if (qualified){
					possible_candidates++;
					qualified = 0;
				}
			}
		}
		//now we have a count of possible candidates
		if (possible_candidates == 0){
			printf("modify: no eligible candidates to add a link\n");
			exit(0);
			return;
		}
		int neighbour = rand() % possible_candidates;
		int count = 0;
		for (int i = 0; i < n; i++){ //go through v's neighbours
			if ((*adj_matrix)[v][i] == 0){ //if we find a non-neighbour
				for (int j = 0; j < opinions; j++){ //look through v's opinions
					for (int k = 0; k < opinions; k++){ //look through v's non-neighbour's opinions
						if ((*opinion_matrix)[v][j] == (*opinion_matrix)[v][k]){ //if they share an opinion
							qualified = 1;
						}
					}
				}
				if (qualified){
					count++;
					qualified = 0;
				}
				if ((count - 1) == neighbour){
					if ((*adj_matrix)[v][i] == 1 || (*adj_matrix)[i][v] == 1){
						printf("---- exit 07 ----\n");
						exit(0);
					}
					(*adj_matrix)[v][i] = 1;
					(*adj_matrix)[i][v] = 1;
					return;
				}
			}
		}
	}

	else if ((possibility == 1 && prob > case_link && prob < case_drop) || (possibility == 2 && prob < case_drop) ||
		(possibility == 3 && prob > case_link && prob > case_none) || (possibility == 4 && prob < case_drop) ||
		(possibility == 5 && prob > case_none && prob > case_link) || (possibility == 6 && prob > case_none && prob < case_drop)){
		//with probability prob_drop, we remove a link between previously linked nodes iff they share no opinion in common
		printf("modify: drop a link\n");
		for (int i = 0; i < n; i++){ //go through v's neighbours
			if ((*adj_matrix)[v][i] == 1){ //if we find a neighbour
				for (int j = 0; j < opinions; j++){ //look through v's opinions
					for (int k = 0; k < opinions; k++){ //look through v's neighbour's opinions
						if ((*opinion_matrix)[v][j] == (*opinion_matrix)[v][k]){ //if they share an opinion
							disqualified = 1;
						}
					}
				}
				if (!disqualified){
					possible_candidates++;
					disqualified = 0;
				}
			}
		}
		//now we have a count of possible candidates
		if (possible_candidates != 0){
			printf("*******************************************\n");
			exit(0);
		}
		if (possible_candidates == 0){
			printf("modify: no eligible candidates to drop a link\n"); //pas 100% confiante qu'il n'y a pas de candidats possibles...
			for (int i = 0; i < n; i++){
				for (int j = 0; j < n; j++){
					printf("%d ", (*adj_matrix)[i][j]);
				}
				printf("\n");
			}
			printf("v = %d\n", v);
			for (int i = 0; i < n; i++){
				for (int j = 0; j < opinions; j++){
					printf("%d ", (*opinion_matrix)[i][j]);
				}
				printf("\n");
			}
			exit(0); //temporary
			return;
		}
		int neighbour = rand() % possible_candidates;
		printf("modify: before drop a link 03\n");
		int count = 0;
		for (int i = 0; i < n; i++){ //go through v's neighbours
			if ((*adj_matrix)[v][i] == 1){ //if we find a neighbour
				for (int j = 0; j < opinions; j++){ //look through v's opinions
					for (int k = 0; k < opinions; k++){ //look through v's neighbour's opinions
						if ((*opinion_matrix)[v][j] == (*opinion_matrix)[v][k]){ //if they share an opinion
							disqualified = 1;
						}
					}
				}
				if (!disqualified){
					count++;
					disqualified = 0;
				}
				if ((count - 1) == neighbour){
					if ((*adj_matrix)[v][i] == 0 || (*adj_matrix)[i][v] == 0){
						printf("---- exit 08 ----\n");
						exit(0);
					}
					(*adj_matrix)[v][i] = 0;
					(*adj_matrix)[i][v] = 0;
					return;
				}
			}
		}
	}

	else { //with probability prob_none, nothing happens
		printf("modify: do nothing\n");
	}

}*/

void write_to_file(int n, int opinions, int*** adj_matrix, int*** opinion_matrix){

	//write adjacency matrix and corresponding opinion matrix to file for later analysis or count of opinions

	//print adjacency matrix to file for later visualization
	FILE *adj_matrix_file;
	adj_matrix_file = fopen("adj_matrix", "a");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < n; j++){
			fprintf(adj_matrix_file, "%d ", (*adj_matrix)[i][j]);
		}
		fprintf(adj_matrix_file, "\n");
	}
	fclose(adj_matrix_file);

	//print opinion matrix to file for later visualization
	FILE *opinion_matrix_file;
	opinion_matrix_file = fopen("opinion_matrix", "a");
	for (int i = 0; i < n; i++){
		for (int j = 0; j < opinions; j++){
			fprintf(opinion_matrix_file, "%d ", (*opinion_matrix)[i][j]);
		}
		fprintf(opinion_matrix_file, "\n");
	}
	fclose(opinion_matrix_file);

}


int main(){

	srand(time(NULL));

	//set partition model parameters
	int c = 4;
	int k = 6;
	double p = 0.8;
	double q = 0.04;
	int n = c*k;
	int opinions = c; //total number of opinions in the system

	//set rewire_strength parameter
	int rewire_strength = 1;

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

	//make sure these files don't exist before we begin a new simulation
	FILE *adj_matrix_file;
	adj_matrix_file = fopen("adj_matrix", "w");
	fprintf(adj_matrix_file, "%d\n", n);
	fclose(adj_matrix_file);

	FILE *opinion_matrix_file;
	opinion_matrix_file = fopen("opinion_matrix", "w");
	fprintf(opinion_matrix_file, "%d\n", n);
	fclose(opinion_matrix_file);

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
