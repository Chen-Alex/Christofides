#ifndef _GRAPH
#define _GRAPH

#include <vector>
#include <stdio.h>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <random>
#include <stack>
#include <string>
#include <fstream>
#include <iostream>

#include "edge.h"
#include "compareedge.h"

using namespace std;
/**
   Representing an (undirected graph)
 **/
struct Graph {
  // Number of nodes. Nodes are labeled 0,1,2,... up to n -1 
  int n_nodes;

  // Adjacency list [adj[i]] gives a list of (node,weight) pairs where i,node are connected.
  vector<vector<pair<int,double>>> adj;

  // Construct graph given adjacency list
  Graph(vector<vector<pair<int,double>>> &a);

  // Construct graph given list of edges
  Graph(vector<Edge> edges);

  // Print adjacency list
  void print();

  // Remove an edge from the graph. In particular,
  // removes the [index]th edge from adj[node], and the reverse direction as well
  void remove_edge(int node, int index);
};

#endif
