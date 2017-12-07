#include <stdio.h>
#include <vector>
#include <queue>
#include <unordered_set>

using namespace std;

/** Structure that represents an undirected edge between two nodes. 
	Comparison of two edges compares their weights. 
    Two edges are the same if they connect the same nodes, in either order. **/
struct Edge {
	int node1, node2;
	double weight;
	Edge(int n1, int n2, double w) {
		node1 = n1;
		node2 = n2;
		weight = w;
	}
	/* The equals function checks if this edge connects the same nodes as edge [e].*/
	bool equals(Edge e) {
		return (e.node1 == node1 && e.node2 == node2) || (e.node1 == node2 && e.node2 == node1);
	}
};

/* The binary comparison operators compare the weight of edges. */
inline bool operator==(const Edge &e1, const Edge &e2) {
	return e1.weight == e2.weight;
}
inline bool operator!=(const Edge &e1, const Edge &e2) {
	return !operator==(e1, e2);
}
inline bool operator<(const Edge &e1, const Edge &e2) {
	return e1.weight < e2.weight;
}
inline bool operator>(const Edge &e1, const Edge &e2) {
	return operator<(e2, e1);
}
inline bool operator <=(const Edge &e1, const Edge &e2) {
	return !operator>(e1, e2);
}
inline bool operator >=(const Edge &e1, const Edge &e2) {
	return !operator<(e1, e2);
}

/** Represents a (undirected) graph. **/
struct Graph {
	// Number of nodes. The nodes are labeled 0, 1, 2, ... , up to n_nodes - 1.
	int n_nodes; 
	// Adjacency list. [adj[i]] gives a list of (node, weight) pairs where i, node are connected.
	vector<vector<pair<int, double>>> adj;
	// Construct a graph by giving the adjacency list
	Graph(vector<vector<pair<int, double>>> &a) {
		n_nodes = a.size();
		adj = a;
	}
	// Construct a graph by giving a list of edges
	Graph(vector<Edge> edges) {
		n_nodes = 0;
		for (Edge e : edges) {
			n_nodes = max(n_nodes, e.node1 + 1);
			n_nodes = max(n_nodes, e.node2 + 1);
			while (adj.size() < n_nodes) adj.push_back(vector<pair<int, double>>());
			adj[e.node1].push_back(make_pair(e.node2, e.weight)); 
			adj[e.node2].push_back(make_pair(e.node1, e.weight));
		}
	}
};

/** Computes the minimum spanning tree of [graph]. Takes and returns adjacency-list representations of a graph.
    The graph must be connected and contain at least one node! **/
Graph MST(Graph graph) {
	unordered_set<int> visited;
	priority_queue<Edge, vector<Edge>, greater<Edge>> edge_queue;
	visited.insert(0);
	for (pair<int, double> p : graph.adj[0]) {
		Edge e(0, p.first, p.second);
		edge_queue.push(e);
	}

	vector<Edge> edges;

	while (visited.size() < graph.n_nodes) {
		Edge e = edge_queue.front();
		while (visited.find(e.node1) != visited.end() && visited.find(e.node2) != visited.end()) {
			edge_queue.pop();
			e = edge_queue.front();
		}
		edge_queue.pop();
		edges.push_back(e);
		int expand_node = visited.find(e.node1) == visited.end() ? e.node1 : e.node2;
		for (pair<int, double> p : graph.adj[expand_node]) {
			Edge f(expand_node, p.first, p.second);
			if (visited.find(p.first) == visited.end()) {
				edge_queue.push(f);
			}
		}
		visited.insert(e.node1);
		visited.insert(e.node2);
	}

	return Graph(edges);
}