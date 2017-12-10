#include <fstream>
#include <sstream>

using namespace std;

#include "edge.h"
#include "graph.h"
#include "compareedge.h"

/**
 * Reads in a file in specified format and returns the resulting graph
 * **/
Graph ReadFile(string filename) {
	vector<Edge> edges;
	string line;

	ifstream infile;
	infile.open(filename);

	if (infile.is_open()) {
		while(getline(infile, line)) {
			int a,b,c;
			stringstream ss(line);
			ss >> a >> b >> c;
			edges.push_back(Edge(a,b,c));
		}
		infile.close();
	} else {
		cout << "Unable to read file";
	}
	cout << "--------" << endl;
	return Graph(edges);
}

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
	Graph n = ReadFile("GRAPH1.TXT");
	vector<int> path2 = euler_cycle(n);
	for (int i : path2) {
		cout << i << endl;
	}
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

