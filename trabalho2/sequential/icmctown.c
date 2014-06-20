/*TODO: ADD ERROR CHECKING IN THE MAIN FUNCTION
		AM I ALLOCATION USELESS MEMORY LIST OF POINTERS TO A VERTEX/EDGE??*/

/* ----------------------------------------------------------------- */
/*  Copyright (c) 2014                                               */
/*  Fernando Noveletto Candiani         7239131,                     */
/*  Marcius Leandro Junior              7277433,                     */
/*  Rafael Hiroki de Figueiroa Minami   7573187,                     */
/*                                                                   */
/*  This program is free software; you can redistribute it and/or    */
/*  modify it under the terms of the GNU General Public License as   */
/*  published by the Free Software Foundation; either version 3 of   */
/*  the License, or (at your option) any later version.  See the     */
/*  file LICENSE included with this distribution for more            */
/*  information.                                                     */
/*  email: fncandiani, marcius, rafahiroki @usp.br                   */
/* ----------------------------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>

/*Show debug messages*/
#define _DEBUG_ 0

/*Variables used to generate errors*/
#define _FAILURE_ 0
#define _SUCCESS_ 1
#define _MEM_ERR_ 2

/*Variables used to control the 'stack' of visited vertex*/
#define _EMPTY_ 0
#define _NOT_EMPTY_ 1

/*Variables used to control the viseted/unvisited vertex of the graph*/
#define _VISITED_ 1
#define _NOT_VISITED_ 2

/*Variable used as infinity for the dijkstra algorithm*/
#define _INF_ 1999999999

typedef struct vertex vertex;

struct vertex{
	unsigned int adjacents_size; /*size of the edge list*/
	struct edge **adjacents; /*all the edges that the vertex has*/
	unsigned int vertex_id; /*The vertex id*/
/*	char color;*/ /*Used for dijkstra algorithm*/
	/*unsigned int value;*/ /* Distance from the node to the path*/
};

typedef struct edge edge;

struct edge{
	unsigned int weight; /*The edge weight*/
	vertex *right; /*The vertex in the right side of the edge*/
	vertex *left; /*The vertex in the left side of the edge*/
	unsigned int edge_id; /*The edge id*/
};

typedef struct graph graph;

struct graph{
	unsigned int vertices_size; /*The number of vertices in the graph*/
	vertex **vertices; /*All the vertices in the graph*/
	unsigned int edges_size; /*The number of edges in the graph*/
	edge **edges; /*All the edges in the graph*/
};

/*Initializes the graph g with the initial values*/
void init_graph(graph *g){
	if(_DEBUG_) printf("Initializing a graph\n");
	if(NULL != g){
		g->vertices_size = 0;
		g->vertices = NULL;
		g->edges_size = 0;
		g->edges = NULL;
		if(_DEBUG_) printf("Initialization successful\n");
		return;
	}
	if(_DEBUG_) printf("Initialization failure\n");
}

/*Initializes the edge e with the initial values*/
void init_edge(edge *e, unsigned int id, unsigned weight){
	if(_DEBUG_) printf("Initializing an edge with id %u and weight %u\n", id, weight);
	if(NULL != e){
		e->weight = weight;
		e->right = NULL;
		e->left = NULL;
		e->edge_id = id;
		if(_DEBUG_) printf("Initialization successful\n");
		return;
	}
	if(_DEBUG_) printf("Initialization failure\n");
}

/*Initializes the vertex v with the initial values*/
void init_vertex(vertex *v, unsigned int id){
	if(_DEBUG_) printf("Initializing a vertex with id %u\n", id);
	if(NULL != v){
		v->adjacents_size = 0;
		v->adjacents =  NULL;
		v->vertex_id = id;
/*		v->color = 'w';*/
		if(_DEBUG_) printf("Initialization successful\n");
		return;
	}
	if(_DEBUG_) printf("Initialization failure\n");
}

/*Allocates a new edge and initialize it with an id and a weight*/
edge* alloc_edge(unsigned int id, unsigned int weight){
	if(_DEBUG_) printf("Allocating a new edge with id %u and weight %u\n", id, weight);
	edge *e = (edge*)malloc(sizeof(edge));
	if(NULL != e){
		init_edge(e, id, weight);
		if(_DEBUG_) printf("Allocation successful\n");
		return e;
	}
	if(_DEBUG_) printf("Allocation failure\n");
	return NULL;
}

/*Allocates a new vertex and initialize it with an id.*/
vertex* alloc_vertex(unsigned int id){
	if(_DEBUG_) printf("Allocating a new vertex with id %u\n", id);
	vertex *v = (vertex*)malloc(sizeof(vertex));
	if(NULL != v){
		init_vertex(v, id);
		if(_DEBUG_) printf("Allocation successful\n");
		return v;
	}
	if(_DEBUG_) printf("Allocation failure\n");
	return NULL;
}

/*Add a edge to a vertex, i.e. allocates a new edge to the list of adjacents of a vertex*/
int add_edge_vertex(graph *g, unsigned int id, edge *e){
	if(_DEBUG_) printf("Adding an edge (%u)to vertex (%u)\n", e->edge_id, id);
	if(NULL != e){
		if(NULL == g->vertices[id]){
			if(_DEBUG_) printf("Addition failure - vertex does not exist.\n");
			return _MEM_ERR_;
		}
		g->vertices[id]->adjacents = (struct edge **)realloc(g->vertices[id]->adjacents,
									(g->vertices[id]->adjacents_size+1)*sizeof(edge));
		if(NULL == g->vertices[id]->adjacents){
			if(_DEBUG_) printf("Addition failure - Not possible to realloc the edges for the given vertex.\n");
			return _MEM_ERR_;
		}
		/*Add the edge to the vertex*/
		g->vertices[id]->adjacents[g->vertices[id]->adjacents_size] = (struct edge *)e;
		g->vertices[id]->adjacents_size++;

		if(_DEBUG_) printf("Creation successful\n");
		return _SUCCESS_;
	}
	return _FAILURE_;
}

/*Creates an edge in the graph g from the vertex origin to vertex destination with a weight weight.*/
int create_edge(graph *g, unsigned int origin, unsigned int destination, unsigned int weight){
	if(_DEBUG_) printf("Creating an edge from %u to %u with weight %u\n", origin, destination, weight);
	if(NULL != g){
		g->edges = (edge **)realloc(g->edges,(g->edges_size+1)*sizeof(edge));
		if(NULL == g-> edges){
			if(_DEBUG_) printf("Creation failure - Not possible to realloc the edges\n");
			/*When there is an error, do I lose all the data?? - realloc*/
			return _MEM_ERR_;
		}

		/*Allocates the edge*/
		g->edges[g->edges_size] = alloc_edge(g->edges_size, weight);
		if(NULL == g->edges[g->edges_size]){
			if(_DEBUG_) printf("Creation failure - Not possible to alloc a new edge\n");
			return _MEM_ERR_;
		}

		/*Add the vertex to the edge*/
		g->edges[g->edges_size]->right = g->vertices[origin];
		g->edges[g->edges_size]->left = g->vertices[destination];

		/*Add the edge to the vertex*/
		add_edge_vertex(g, origin, g->edges[g->edges_size]);
		add_edge_vertex(g, destination, g->edges[g->edges_size]);

		g->edges_size++;
		if(_DEBUG_) printf("Creation successful\n");
		return _SUCCESS_;
	}
	if(_DEBUG_) printf("Creation failure\n");
	return _FAILURE_;
}

/*Create all the vertex needed in the graph*/
int create_vertices(graph *g, int size){

	int i;
	for(i = 0; i < size; i++){
		g->vertices = (vertex**)realloc(g->vertices, (g->vertices_size+1)*sizeof(vertex));
		if(NULL == g->vertices){
			if(_DEBUG_) printf("Creation failure - Not possible to realloc the vertices\n");
			return _MEM_ERR_;
		}

		g->vertices[i] = alloc_vertex(i);
		if(NULL == g->vertices[i]){
			if(_DEBUG_) printf("Creation failure - Not possible to alloc a new vertex\n");
			return _MEM_ERR_;
		}
		g->vertices_size++;
	}

	return _SUCCESS_;

}

/* Free a vertex and all its edges*/
void free_vertex(vertex *v){

	if(NULL != v){

		if(_DEBUG_) printf("Free vertex %u\n", v->vertex_id);
		unsigned int i;

		if(_DEBUG_) printf("Free all its edges\n");
		for(i = 0; i < v->adjacents_size; i++){
			v->adjacents[i] = NULL;
			free(v->adjacents[i]);
		}

		if(_DEBUG_) printf("Free edges successful\n");
		free(v->adjacents);
		free(v);
		if(_DEBUG_) printf("Free vertex successful\n");
	}
}

/*Free all the graph structure*/
void free_all(graph *g){

	unsigned int i;
	for(i = 0; i < g->vertices_size; i++){
		free_vertex(g->vertices[i]);
	}
	free(g->vertices);


	for(i = 0; i < g->edges_size; i++){
		free(g->edges[i]);
	}
	free(g->edges);
}

/*Check wether the graph is all 'black', i.e., all the nodes has been visited*/
int is_empty(unsigned int *unvisited, unsigned int size){

	unsigned int i;
	for(i = 0; i < size; i++){
		if(_VISITED_ != (int) unvisited[i]){
			return _NOT_EMPTY_;
		}
	}
	return _EMPTY_;
}


/*Extract the minimal value of the values vector only if a vertex has not been visited yet.*/
unsigned int extract_min(graph *g, unsigned int *unvisited, unsigned int* values){

	unsigned int i, value = _INF_, vertex_id;

	for(i = 0; i < g->vertices_size; i++){
		if((_VISITED_ != (int) unvisited[i]) && value > values[i]){
			value = values[i];
			vertex_id = i;
			if(_DEBUG_) printf("Changing node %d = %d\n", vertex_id, g->vertices[i]->vertex_id);
		}
	}

	return vertex_id;
}

unsigned int *dijkstra(graph *g, unsigned int source){

	if(_DEBUG_) printf("Executing Dijkstra\n");

	unsigned int i, j, current_vertex, adjacent;

	/*Create a set of the unvisited nodes called the unvisited set,*/
	/*consisting of all the nodes.*/
	if(_DEBUG_) printf("Vertices size %u\n", g->vertices_size);
	unsigned int *unvisited = (unsigned int *) malloc(sizeof(unsigned int)*g->vertices_size);
	unsigned int *values = (unsigned int *) malloc(sizeof(unsigned int)*g->vertices_size);

	/*Assign to every node a tentative distance value: set it to zero for */
	/*our initial node and to infinity for all other nodes.*/
	/*Mark all nodes unvisited.*/
	for(i = 0; i < g->vertices_size; i++){
		unvisited[i] = _NOT_VISITED_;
		values[i] = _INF_;
	}

	values[source] = 0;

	/*Until all the vertex has been visited execute the algorithm*/
	while(_EMPTY_ != is_empty(unvisited, g->vertices_size)){
		/*Extract the minimal value of the values vector and get the vertex id*/
		current_vertex = extract_min(g, unvisited, values);
		/*Set the node to be viseted*/
		unvisited[current_vertex] = _VISITED_;
		if(_DEBUG_) printf("Vertex min %u\n", current_vertex);

		/*Check wether the current vertex is accessible from source.*/
		if(_INF_ == values[current_vertex]){
			if(_DEBUG_) printf("Breaking, vertex not accessible from source.\n");
			break;
		}

		if(_DEBUG_) printf("Adjacents %u\n", g->vertices[current_vertex]->adjacents_size);
		/*For all the edges of the current_vertex check the less weighted*/
		for(j = 0; j < g->vertices[current_vertex]->adjacents_size; j++){
			struct edge *e = g->vertices[current_vertex]->adjacents[j];
			if(_DEBUG_) printf("edge id %u\n", e->edge_id);
			/*Check the side of the vertex (right or left)*/
			(e->right->vertex_id == current_vertex) ? (adjacent = e->left->vertex_id) :
													  (adjacent = e->right->vertex_id);
			if(_DEBUG_) printf("adjacent %u\n", adjacent);

			/*Change the value of the weight for the adjacent vertex*/
			if(_INF_ == values[adjacent]){
				values[adjacent] = values[current_vertex] + e->weight;
			}else if(values[adjacent] > (values[current_vertex] + e->weight)){
				values[adjacent] = values[current_vertex] + e->weight;
			}

			if(_DEBUG_) printf("Adjacent Value %u\n", values[adjacent]);
		}

	}

	free(unvisited);
	if(_DEBUG_) printf("Dijkstra execution finished\n");
	return values;
}

float best_vertex(unsigned int **M, unsigned int n){
	unsigned int max, shortest = _INF_, i, j;//, vertex;

	for (i = 0; i < n; i++){
		max = 0;
		for (j = 0; j < n; ++j){
			if(M[i][j] > max)
				max = M[i][j];
		}
		if(max < shortest){
			shortest = max;
/*			vertex = i;*/
		}
	}

/*	printf("%d\n",vertex+1);*/
	return (float) shortest;
}

int main(void){

	unsigned int n, m, i, j;
	graph g;
	init_graph(&g);

	/*Start reading the data from the stdin*/
	scanf("%u%u",&n,&m);
	if(_DEBUG_) printf("%u\t%u\n",n,m);

	create_vertices(&g, n);

	for(i = 0; i < m; i++){
		unsigned int orig, dest, weight;
		scanf("%u%u%u", &orig, &dest, &weight);
		if(_DEBUG_) printf("%u\t%u\t%u\n", orig, dest, weight);
		create_edge(&g, orig-1, dest-1, weight);
	}

	/*Compute the dijkstra algorithm for all the vertex in the graph*/
	unsigned int *results[n];
	for(i = 0; i < g.vertices_size; i++){
		results[i] = dijkstra(&g,i);
		if(_DEBUG_){
			for(j = 0; j < g.vertices_size; j++){
				printf("%u\t", results[i][j]);
			}
			printf("\n");
		}
	}

	printf("%.6f\n", best_vertex(results, n));

	for(i = 0; i < g.vertices_size; i++){
		free(results[i]);
	}

	free_all(&g);

	return EXIT_SUCCESS;
}
