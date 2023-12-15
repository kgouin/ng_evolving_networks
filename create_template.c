#include<stdio.h>
#include<stdlib.h>
#include <time.h>
#include <sys/time.h>

int main(){

    FILE *template;
	template = fopen("template", "w");
	fprintf(template, "c k n p q num_opinions_initial num_components_initial avg_num_cross_connections avg_num_within_connections avg_opinion_divergence num_components_final num_successful num_unsuccessful num_neighbourless_speaker num_opinions_final num_timesteps\n");
    fclose(template);

    FILE *extra;
	extra = fopen("extra", "w");
    fprintf(extra, "\n");
    fclose(extra);
}