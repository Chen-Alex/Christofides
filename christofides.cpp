#include <fstream>
#include <sstream>
#include <set>
#include <math.h>

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

/**
 * Calculates the Euclidian distance between two points
 * */
double eucDist(pair<double,double> a, pair<double,double> b) {
	if (a == b) {
		return 0.0;
	}
	double dist = sqrt ( pow (a.first - b.first, 2) + pow (a.second - b.second, 2));
	return dist;
}
/**
 * Read file metric. 
 * Given a list of 2d points
 * Construct a graph from this
 * */
Graph ReadFileMetric(string filename) {
	vector<pair<double,double>> points;
	vector<Edge> edges;
	set<pair<int,int>> visited;
	string line;

	ifstream infile;
	infile.open(filename);

	if (infile.is_open()) {
		while(getline(infile, line)) {
			int a;
			long double b,c;
			stringstream ss(line);
			ss >> a >> b >> c;
			points.push_back(make_pair(b,c));
		}
		infile.close();
	} else {
		cout << "Unable to read file";
	}


	cout << "------" << endl;
	for (int i = 0; i < points.size(); i++) {
		pair<double,double> a = points[i];
		for (int j = 0; j < points.size(); j++) {
			pair<double,double> b = points[j];
			double dist = eucDist(a,b);
			if ((dist == 0.0) || (visited.count(make_pair(i,j)) || visited.count(make_pair(j,i)))) {
				continue;
			} else {
				visited.insert(make_pair(i,j));
				visited.insert(make_pair(j,i));
				edges.push_back(Edge(i,j,dist));
			}
		}
	}
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



/**
 * Christofides Algorithm
 * Given : Complete Graph whose edge weights follow the triangle inequality 
 * 
 * Procedure:
 *  1. Calculate T = MST
 *  2. Calculate Set of Vertices with Odd Degree in T. This is called O
 *  3. Form the subgraph of G using only the vertices of O
 *  4. Get Minimum Weight perfect matching M from this subgraph
 *  5. Take T union M to form a eularian multigraph R
 *  6. Calculate the euler tour of R
 *  7. Remove the repeated vertices giving the algorithms output
 * */
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

	Graph n = ReadFile("TESTS/GRAPH1.TXT");
	Graph b = ReadFileMetric("TESTS/dj38.tsp");
	b.print();
	
	//vector<int> path2 = euler_cycle(n);
	/*for (int i : path2) {
		cout << i << endl;
	}*/
	//Graph ms = MST(n);
	//ms.print();
	//Graph h = MST(g);
	//h.print();
	//h.remove_edge(0, 0);
	//h.print();
	return 0;
}

