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

#define _DEBUG_ 0

#define _FAILURE_ 0
#define _SUCCESS_ 1
#define _MEM_ERR_ 2

#define _EMPTY_ 0
#define _NOT_EMPTY_ 1

#define _VISITED_ -1

#define _INF_ 99999999
/*if(_DEBUG_) printf("\n");*/

typedef struct vertex vertex;

struct vertex{
	unsigned int adjacents_size; /*size of the edge list*/
	struct edge **adjacents; /*all the edges that the vertex has*/
	unsigned int vertex_id; /*The vertex id*/
/*	char color;*/ /*Used for dijkstra algorithm*/
	unsigned int value; /* Distance from the node to the path*/
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
		g->vertices[id]->adjacents[g->vertices[id]->adjacents_size] = (struct edge *)e;
		g->vertices[id]->adjacents_size++;
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

int create_vertices(graph *g, int size){
	int i;
	for(i = 0; i < size; i++){
		g->vertices = (vertex**)realloc(g->vertices, (g->vertices_size+1)*sizeof(vertex));
		if(NULL == g->vertices){
			if(_DEBUG_) printf("Creation failure - Not possible to realloc the vertices\n");
			/*When there is an error, do I lose all the data?? - realloc (change it??)*/
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
		if(_VISITED_ != (int) unvisited[i])
			return _NOT_EMPTY_;
	}
	return _EMPTY_;
}

unsigned int extract_min(graph *g, unsigned int *unvisited){

	unsigned int i, value = _INF_, vertex_id;

	for(i = 0; i < g->vertices_size; i++){
		if((_VISITED_ != (int) unvisited[i]) && value > g->vertices[i]->value){
			value = g->vertices[i]->value;
			/*This has to be iqual to g->vertices[i]->vertex_id*/
			vertex_id = i;
			if(_DEBUG_) printf("Changing node as current value %d = %d \n", 
										vertex_id, g->vertices[i]->vertex_id);
		}
	}

	return vertex_id;
}

void dijkstra(graph *g, unsigned int initial, unsigned int destination){

	unsigned int i, j, current_vertex, adjacent;

	/*Create a set of the unvisited nodes called the unvisited set*/
	/*consisting of all the nodes.*/
	unsigned int *unvisited = (unsigned int *) malloc(sizeof(unsigned int)*g->vertices_size);

	/*Assign to every node a tentative distance value: set it to zero for */
	/*our initial node and to infinity for all other nodes.*/
	/*Mark all nodes unvisited.*/
	for(i = 0; i < g->vertices_size; i++){
		g->vertices[i]->value = _INF_;
		/*The color is not used in the algorithm because its created a vector*/
		/*of unvisited vertices.*/
/*		g->vertices[i]->color = 'w';*/
/*		unvisited[i] = g->vertices[i]->vertex_id;*/
		unvisited[i] = i;
	}
	g->vertices[initial]->value = 0;

	while(_EMPTY_ == is_empty(unvisited, g->vertices_size)){
		current_vertex = extract_min(g, unvisited);
		unvisited[current_vertex] = _VISITED_;

		/*Check wether the current vertex is accessible from source.*/
		if(_INF_ == g->vertices[current_vertex]->value){
			break;
		}

		for(j = 0; j < g->vertices[current_vertex]->adjacents_size; j++){
			struct edge *e = g->vertices[current_vertex]->adjacents[i];
			(e->right->vertex_id == current_vertex) ? (adjacent = e->left->vertex_id) :
													  (adjacent = e->right->vertex_id);

			/* ------------------------   This can be improved   ---------------------------*/

			if(_INF_ == g->vertices[adjacent]->value){
				g->vertices[adjacent]->value = g->vertices[current_vertex]->value + e->weight;
			}else if(g->vertices[adjacent]->value > (g->vertices[current_vertex]->value + e->weight)){
				g->vertices[adjacent]->value = g->vertices[current_vertex]->value + e->weight;
			}
			
			/*********************************************************************************/
			
		}

		/*Check wether it has reached the destination node.*/
		if(g->vertices[current_vertex]->vertex_id == destination){
			break;
		}

	}

	free(unvisited);
}

int main(void){

	unsigned int n,m,i;
	graph g;
	init_graph(&g);

	scanf("%u%u",&n,&m);
	if(_DEBUG_) printf("%u\t%u\n",n,m);

	create_vertices(&g, m);

	for(i = 0; i < m; i++){
		unsigned int orig, dest, weight;
		scanf("%u%u%u", &orig, &dest, &weight);
		if(_DEBUG_) printf("%u\t%u\t%u\n", orig, dest, weight);
		create_edge(&g, orig-1, dest-1, weight);
	}

	free_all(&g);

	return EXIT_SUCCESS;
}
