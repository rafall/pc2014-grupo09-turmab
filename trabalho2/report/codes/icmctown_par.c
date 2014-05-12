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

#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

/*Show debug messages*/
#define _DEBUG_ 0

/*Variable used to control the send/receive of messages and program flow*/
#define MASTER 0
#define FROM_MASTER 1
#define FROM_WORKER 2

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

	/*Free all the vertices*/
	for(i = 0; i < g->vertices_size; i++){
		free_vertex(g->vertices[i]);
	}
	free(g->vertices);

	/*Free all the edges*/
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


/*Extract the minimal value of the values vector only if the vertex has not been visited yet.*/
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

/*Calculate the distance from vertex source to all node in the g graph*/
unsigned int *dijkstra(graph *g, unsigned int source){

	if(_DEBUG_) printf("Executing Dijkstra\n");

	unsigned int i, j, current_vertex, adjacent;

	/*Create a set of the unvisited nodes called the unvisited set,*/
	/*consisting of all the nodes.*/
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

		/*Check wether the current vertex is accessible from source.*/
		if(_INF_ == values[current_vertex]){
			if(_DEBUG_) printf("Breaking, vertex not accessible from source.\n");
			break;
		}

		/*For all the edges of the current_vertex check the less weighted*/
		for(j = 0; j < g->vertices[current_vertex]->adjacents_size; j++){
			struct edge *e = g->vertices[current_vertex]->adjacents[j];
			/*Check the side of the vertex (right or left)*/
			(e->right->vertex_id == current_vertex) ? (adjacent = e->left->vertex_id) :
													  (adjacent = e->right->vertex_id);

			/*Change the value of the weight for the adjacent vertex*/
			if(_INF_ == values[adjacent]){
				values[adjacent] = values[current_vertex] + e->weight;
			}else if(values[adjacent] > (values[current_vertex] + e->weight)){
				values[adjacent] = values[current_vertex] + e->weight;
			}

		}

	}

	free(unvisited);
	if(_DEBUG_) printf("Dijkstra execution finished\n");
	return values;
}

/*Search the max value inside the vector v*/
unsigned int search_max(unsigned int *v, unsigned int size){

	unsigned int i, max = 0;
	for(i = 0; i < size; i++){
		if(v[i] > max){
			max = v[i];
		}
	}
	return max;
}

/*Search the min value inside the vector v*/
unsigned int search_min(unsigned int *v, unsigned int size){

	unsigned int i, min = _INF_;
	for(i = 0; i < size; i++){
		if(min > v[i]){
			min = v[i];
		}
	}
	return min;
}

int main(int argc, char *argv[]){

	int task, ntask, rc = 0, er, destination, nworker, i;
	unsigned int n, m, start_vertex, nvertex;
	MPI_Status status;

	/*Init the MPI parameters*/
	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&task);
	MPI_Comm_size(MPI_COMM_WORLD,&ntask);

	/*Check to see wether the program is executing in more than one machine*/
	if(ntask < 2 ){
		printf("To run an MPI program 2 or more tasks are needed.\n");
		MPI_Abort(MPI_COMM_WORLD, rc);
		exit(EXIT_FAILURE);
	}

	nworker = ntask-1;

	/*Start the master activity*/
	if(MASTER == task){

		/*Start reading the data from the stdin*/
		scanf("%u%u",&n,&m);
		if(_DEBUG_) printf("%u\t%u\n",n,m);

		unsigned int *orig, *dest, *weight, j;
		orig = (unsigned int *) malloc(sizeof(unsigned int)*m);
		dest = (unsigned int *) malloc(sizeof(unsigned int)*m);
		weight = (unsigned int *) malloc(sizeof(unsigned int)*m);

		for(i = 0; i < (int) m; i++){
			scanf("%u%u%u", &orig[i], &dest[i], &weight[i]);
			if(_DEBUG_) printf("%u\t%u\t%u\n", orig[i], dest[i], weight[i]);
		}

		start_vertex = 0;
		if(_DEBUG_) printf("Starting sending the data to the nodes\n");
		/*Send the data needed in the slave nodes to compute the dijkstra algorithm*/
		for(destination = 1; destination <= nworker; destination++){
			nvertex = (destination <= (int) n%nworker) ? (n/nworker)+1 : n/nworker;

			er = MPI_Send(&start_vertex, 1, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the start vertex for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(&nvertex, 1, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the number of vertex for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(&n, 1, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the number of vertex in the graph for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(&m, 1, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the the size of the graph for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(orig, m, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the orig vector for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(dest, m, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the dest vector for the %u worker.\n", destination);
				break;
			}

			er = MPI_Send(weight, m, MPI_UNSIGNED, destination, FROM_MASTER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the weight vector for the %u worker.\n", destination);
				break;
			}

			start_vertex += nvertex;
		}

		free(orig);
		free(dest);
		free(weight);
		if(_DEBUG_) printf("All Data sent to al the workers.\n");

		unsigned int *results = (unsigned int*) malloc(sizeof(unsigned int)*n);

		if(_DEBUG_) printf("Starting receiving the data from nodes.\n");
		/*Receive the data*/
		for(i = 1; i <= nworker; i++){

			er = MPI_Recv(&start_vertex, 1, MPI_UNSIGNED, i, FROM_WORKER, MPI_COMM_WORLD, &status);
			if(MPI_SUCCESS != er){
				printf("Failure receiving the start vertex for the %u worker.\n", nworker);
			}

			er = MPI_Recv(&nvertex, 1, MPI_UNSIGNED, i, FROM_WORKER, MPI_COMM_WORLD, &status);
			if(MPI_SUCCESS != er){
				printf("Failure receiving the number of vertex for the %u worker.\n", nworker);
			}

			for(j = 0; j < nvertex; j++){

				er = MPI_Recv(&results[j+start_vertex], 1, MPI_UNSIGNED, i, FROM_WORKER, MPI_COMM_WORLD, &status);
				if(MPI_SUCCESS != er){
					printf("Failure receiving the results vector for the %u worker.\n", destination);
				}

				if(_DEBUG_) printf("%u\n",results[j+start_vertex]);
			}

			if(_DEBUG_) printf("All data received from worker %u\n",i);
		}

		printf("%u\n", search_min(results, n));

		if(_DEBUG_) printf("All data received from workers\n");

		free(results);
	}
	/****************************End of the master***************************************/

	/*Start all nodes that are going to process the graph.*/
	if(task > MASTER){

		if(_DEBUG_) printf("Starting to receive data in worker %d.\n", task);

		er = MPI_Recv(&start_vertex, 1, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the start vertex in worker %d\n", task);
		}

		er = MPI_Recv(&nvertex, 1, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the number of vertex to be calculated in worker %d\n", task);
		}

		er = MPI_Recv(&n, 1, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the number of vertex in the graph in worker %d\n", task);
		}

		er = MPI_Recv(&m, 1, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the size of the graph in worker %d\n", task);
		}

		unsigned int *orig, *dest, *weight;
		orig = (unsigned int *) malloc(sizeof(unsigned int)*m);
		dest = (unsigned int *) malloc(sizeof(unsigned int)*m);
		weight = (unsigned int *) malloc(sizeof(unsigned int)*m);

		er = MPI_Recv(orig, m, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the orig vector in worker %d.\n", task);
		}

		er = MPI_Recv(dest, m, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the dest vector in worker %d.\n", task);
		}

		er = MPI_Recv(weight, m, MPI_UNSIGNED, MASTER, FROM_MASTER, MPI_COMM_WORLD, &status);
		if(MPI_SUCCESS != er){
				printf("Failure receiving the weight vector in worker %d.\n", task);
		}

		if(_DEBUG_) printf("%u\t%u\n", n,m);
		if(_DEBUG_){
			for(i=0;i<(int)m;i++)
				printf("%u\t%u\t%u\n",orig[i],dest[i],weight[i]);
		}

		if(_DEBUG_) printf("Starting to process all data received from master in worker %d.\n", task);

		graph g;
		init_graph(&g);

		create_vertices(&g, n);

		for(i = 0; i < (int) m; i++){
			create_edge(&g, orig[i]-1, dest[i]-1, weight[i]);
		}

		if(_DEBUG_) printf("Graph created in worker %d", task);
		/*Free the used memory*/
		free(orig);
		free(dest);
		free(weight);

		/*Compute the dijkstra algorithm for all the vertex in the graph*/
		unsigned int **results = (unsigned int **)malloc(sizeof(unsigned int *)*nvertex), j;

		for(i = 0; i < (int) nvertex; i++){
			results[i] = dijkstra(&g,i+start_vertex);
			if(_DEBUG_){
				for(j = 0; j < n; j++){
					printf("%u\t", results[i][j]);
				}
				printf("\n");
			}
		}

		er = MPI_Send(&start_vertex, 1, MPI_UNSIGNED, MASTER, FROM_WORKER, MPI_COMM_WORLD);
		if(MPI_SUCCESS != er){
			printf("Failure sending the start vertex for the master from worker %d.\n", task);
		}

		er = MPI_Send(&nvertex, 1, MPI_UNSIGNED, MASTER, FROM_WORKER, MPI_COMM_WORLD);
		if(MPI_SUCCESS != er){
			printf("Failure sending the number of vertex for the master from worker %d.\n", task);
		}

		for(i = 0; i < (int) nvertex; i++){

			unsigned int v = search_max(results[i], n);
			er = MPI_Send(&v, 1, MPI_UNSIGNED, MASTER, FROM_WORKER, MPI_COMM_WORLD);
			if(MPI_SUCCESS != er){
				printf("Failure sending the result vector for the master from worker %d.\n", task);
			}

		}

		if(_DEBUG_) printf("All data sent to master from worker %d.\n", task);

		for(i = 0; i < (int) nvertex; i++){
			free(results[i]);
		}

		free(results);
		free_all(&g);
	}
	/****************************End of nodes***************************************/

	MPI_Finalize();
	return EXIT_SUCCESS;
}
