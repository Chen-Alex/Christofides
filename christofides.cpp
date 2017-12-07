#include <stdio.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <stack>

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
	/* The equals function checks if this edge connects the same nodes as edge [e] and has the same weight.*/
	bool equals(Edge e) {
		return ((e.node1 == node1 && e.node2 == node2) || (e.node1 == node2 && e.node2 == node1)) && weight == e.weight;
	}
};

/** Compare edges by weight, descending **/
struct CompareEdge {
	bool operator() (const Edge &e1, const Edge &e2) {
		return e1.weight > e2.weight;
	}
};

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
	// Print the adjacency list...
	void print() {
		for (int i = 0; i < adj.size(); i++) {
			cout << i << ": ";
			for (pair<int, double> conn : adj[i]) {
				cout << "(" << conn.first << ", " << conn.second << ") ";
			}
			cout << endl;
		}
	}
	// Remove an edge from the graph. In particular, removes
	// the [index]th edge from adj[node], and the reverse direction as well.
	void remove_edge(int node, int index) {
		if (node >= adj.size() || index >= adj[node].size()) return;
		int other_node = adj[node][index].first;
		pair<int, double> to_find = make_pair(node, adj[node][index].second);
		adj[other_node].erase(find(adj[other_node].begin(), adj[other_node].end(), to_find));
		adj[node].erase(adj[node].begin() + index);
	}
};

/** Computes the minimum spanning tree of [graph]. Takes and returns adjacency-list representations of a graph.
The graph must be connected and contain at least one node! **/
Graph MST(Graph &graph) {
	unordered_set<int> visited;
	priority_queue<Edge, vector<Edge>, CompareEdge> edge_queue;
	visited.insert(0);
	for (pair<int, double> p : graph.adj[0]) {
		Edge e(0, p.first, p.second);
		edge_queue.push(e);
	}

	vector<Edge> edges;

	while (visited.size() < graph.n_nodes) {
		Edge e = edge_queue.top();
		while (visited.find(e.node1) != visited.end() && visited.find(e.node2) != visited.end()) {
			edge_queue.pop();
			e = edge_queue.top();
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

/** Computes an Eulerian cycle in [graph], which may be a multigraph. Outputs a list of the vertices traversed in order.
    Every vertex must have even degree. [graph] must be connected. Will modify [graph]. **/
vector<int> euler_cycle(Graph &graph) {
	stack<int> fwd, bkwd;
	int start = 0;
	fwd.push(start);
	while (true) {
		fwd.push(graph.adj[start][0].first);
		graph.remove_edge(start, 0);
		while (graph.adj[fwd.top()].size() > 0) {
			int k = fwd.top();
			fwd.push(graph.adj[k][0].first);
			graph.remove_edge(k, 0);
		}
		while (graph.adj[fwd.top()].size() == 0) {
			bkwd.push(fwd.top());
			fwd.pop();
			if (fwd.empty()) goto end;
		}
		start = fwd.top();
	}
end:
	vector<int> path;
	while (!fwd.empty()) {
		path.push_back(fwd.top());
		fwd.pop();
	}
	reverse(path.begin(), path.end());
	while (!bkwd.empty()) {
		path.push_back(bkwd.top());
		bkwd.pop();
	}
	return path;
}

/** Computes a minimum weight perfect matching in [graph]. Assumes that [graph] has a perfect matching. **/
vector<Edge> min_wt_perfect_matching(Graph &graph) {

}


int main()
{
	vector<Edge> edges;
	/*edges.push_back(Edge(0, 1, 1));
	edges.push_back(Edge(1, 2, 1));
	edges.push_back(Edge(2, 3, 1));
	edges.push_back(Edge(3, 0, 1));
	edges.push_back(Edge(3, 4, 1));
	edges.push_back(Edge(4, 5, 1));
	edges.push_back(Edge(5, 6, 1));
	edges.push_back(Edge(6, 3, 1));
	edges.push_back(Edge(2, 7, 1));
	edges.push_back(Edge(7, 8, 1));
	edges.push_back(Edge(8, 9, 1));
	edges.push_back(Edge(9, 2, 1));
	edges.push_back(Edge(1, 10, 1));
	edges.push_back(Edge(10, 11, 1));
	edges.push_back(Edge(11, 12, 1));
	edges.push_back(Edge(12, 1, 1));
	edges.push_back(Edge(0, 13, 1));
	edges.push_back(Edge(13, 14, 1));
	edges.push_back(Edge(14, 15, 1));
	edges.push_back(Edge(15, 0, 1));*/

	//edges.push_back(Edge(0, 2, 2));
	//edges.push_back(Edge(1, 3, 4));
	Graph g(edges);
	vector<int> path = euler_cycle(g);
	for (int i : path) {
		cout << i << endl;
	}
	//Graph h = MST(g);
	//h.print();
	//h.remove_edge(0, 0);
	//h.print();
	return 0;
}

