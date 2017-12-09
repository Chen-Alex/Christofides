#include <unordered_set>
#include <unordered_map>
#include <vector> 
#include <iostream>
#include <queue>

using namespace std;

// Taken from https://stackoverflow.com/questions/20511347/a-good-hash-function-for-a-vector
/*struct vector_hash {
	std::size_t operator()(std::vector<int> const& vec) const {
		std::size_t seed = vec.size();
		for (auto& i : vec) {
			seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
		}
		return seed;
	}
};*/

//// Standard node
//struct Node {
//	int index;
//	bool is_pseudo = false;
//	double dual = 0;
//
//	// Adjacency list
//	vector<pair<Node*, double>> adj;
//
//	bool operator==(const Node &n) const {
//		return index == n.index;
//	}
//
//	size_t hash() const {
//		return std::hash<int>()(index);
//	}
//
//	Node() {}
//};

struct Edge;
struct Node;
struct Graph;

struct Node {
	int index;
	bool is_pseudo = false;
	double dual = 0;

	// Adjacency list
	vector<Edge*> adj;

	bool operator==(const Node &n) const {
		return index == n.index;
	}

	/*size_t hash() const {
	return std::hash<int>()(index);
	}*/

	Node(int ind, vector<Edge*> &a) {
		index = ind;
		adj = a;
	}

	Node() {}
};

struct Edge {
	Node *node1, *node2;
	double slack;
	double weight;

	// Edge construction must happen before blossom algorithm is run
	Edge(Node *n1, Node *n2, double wt) {
		node1 = n1;
		node2 = n2;
		weight = wt;
		slack = wt;
	}

	// Return the other node corresponding to this edge
	Node *other(Node *n) {
		if (n->index == node1->index) return node2;
		else if (n->index == node2->index) return node1;
		else return 0; // Shouldn't ever happen
	}
};

// Create a hash function for type Node
/*namespace std {
	template <> struct hash<Node> {
		size_t operator()(const Node &n) const {
			return n.hash();
		}
	};
}*/

struct Pseudonode : Node {
	vector<int> index;

	// Store the nodes that this Pseudonode consumed
	unordered_set<Node*> nodes;
	// Store the set of all actual nodes in this Pseudonode 
	unordered_set<Node*> expanded_nodes;

	bool operator==(const Pseudonode &n) const {
		return index == n.index;
	}

	/*size_t hash() const {
		return vector_hash()(index);
	}*/

	// Make the [consumed] set. Recursively iterate through pseudonodes to get their
	// underlying nodes.
	void make_consumed(unordered_set<Node*> &v) {
		for (Node *n : v) {
			if (!n->is_pseudo) expanded_nodes.insert(n);
			else {
				Pseudonode *p = (Pseudonode *)n;
				make_consumed(p->nodes);
			}
		}
	}

	Pseudonode(vector<Node*> &original, Graph &graph);
};

struct Graph {
	unordered_set<Node*> nodes;
	// Store a mapping from nodes to pseudonodes that tells which nodes have been consumed by which pseudonodes in this grap
	unordered_map<Node*, Pseudonode*> consumed;

	Graph(vector<Node*> n) {
		nodes = unordered_set<Node*>(n.begin(), n.end());
	}

	// Add the specified blossom to the graph and remove the original nodes
	void shrink_blossom(Pseudonode *p) {
		nodes.insert(p);
		for (Node *node : p->nodes) {
			nodes.erase(nodes.find(node));
			consumed.insert({ node, p });
		}
	}

	// Expand a blossom back into its proper nodes
	void expand_blossom(Pseudonode *p) {
		nodes.erase(nodes.find(p));
		for (Node *node : p->nodes) {
			nodes.insert(node);
			consumed.erase(node);
		}
	}

	// Given [n] a real node, return [n] if this graph contains [n]; otherwise
	// return the pseudonode that contains [n].
	Node *search(Node *n) {
		while (nodes.find(n) == nodes.end()) n = consumed.find(n)->second;
		return n;
	}

	// Print out a representation of this graph
	void print() {
		for (auto it = nodes.begin(); it != nodes.end(); it++) {
			Node *n = *it;
			if (n->is_pseudo) {
				Pseudonode *p = (Pseudonode *)n;
				cout << "Pseudonode ( ";
				for (int i : p->index) {
					cout << i << " ";
				}
				cout << ")";
			}
			else cout << "Node " << n->index;
			cout << " (dual = " << n->dual << "): ";
			for (Edge *e : n->adj) {
				cout << "( ";
				cout << "<" << e->node1->index << "," << e->node2->index << ">, ";
				cout << "weight = " << e->weight << ", ";
				cout << "slack = " << e->slack << " ) ";
			}
			cout << endl;
		}
	}
};

Pseudonode::Pseudonode(vector<Node*> &original, Graph &graph) {
	is_pseudo = true;
	nodes = unordered_set<Node*>(original.begin(), original.end());
	make_consumed(nodes);
	index = vector<int>();
	for (Node *n : expanded_nodes) {
		index.push_back(n->index);
	}
	sort(index.begin(), index.end());
	// The adjacency list of this pseudonode will contain all (actual) nodes that
	// are not inside of this, but are adjacent to some node inside this
	for (Node *n : nodes) {
		for (Edge *e : n->adj) {
			// This requires that graph is up-to-date (other than this blossom).
			// Can use [expanded_nodes] instead, but it's likely slower.
			// Looking to remove [expanded_nodes] in the final version...
			auto f1 = nodes.find(graph.search(e->node1));
			auto f2 = nodes.find(graph.search(e->node2));
			if (f1 == nodes.end() ^ f2 == nodes.end()) {
				adj.push_back(e);
			}
		}
	}
}

/** Computes a minimum weight perfect matching in [graph]. Assumes that [graph] has a perfect matching and no pseudonodes. **/
vector<Edge*> min_wt_perfect_matching(Graph &graph) {
	// List of unmatched (actual) nodes
	list<Node*> unmatched;
	for (Node *n : graph.nodes) {
		unmatched.push_back(n);
	}
	// The matching to return
	vector<Edge*> matching;
}

//// Pseudonode for odd-set vertices of size >= 3
//struct Pseudonode : Node {
//	// [index] must be sorted!
//	vector<int> index;
//
//	// Store the nodes that this Pseudonode consumed
//	unordered_set<Node*> nodes;
//
//	bool operator==(const Pseudonode &n) const {
//		return index == n.index;
//	}
//
//	size_t hash() const {
//		return vector_hash()(index);
//	}
//
//	Pseudonode(vector<Node*> &original) {
//		is_pseudo = true;
//		nodes = unordered_set<Node*>(original.begin(), original.end());
//		index = vector<int>();
//		for (Node *n : original) {
//			index.push_back(n->index);
//		}
//		sort(index.begin(), index.end());
//		// The adjacency list of this pseudonode will contain all (actual) nodes that
//		// are not inside of this, but are adjacent to some node inside this
//		unordered_set<Node> added;
//		for (Node &n : original) {
//			for (pair<Node, double> nw : n.adj) {
//				Node cand = nw.first;
//				if (added.find(cand) == added.end() && nodes.find(cand) == nodes.end()) {
//					adj.push_back(make_pair(cand, nw.second));
//					added.insert(cand);
//				}
//			}
//		}
//	}
//};
//
//struct Graph {
//	unordered_set<Node> nodes;
//	// Store the dual value of each (actual) node
//	vector<double> duals;
//	// Store the slack of each edge in a 2D vector where slack[a][b] = slack[b][a] is the slack on edge (a, b)
//	vector<vector<double>> slacks;
//	// Store a mapping from nodes to pseudonodes that tells which nodes have been consumed by which pseudonodes in this graph
//	unordered_map<Node, Pseudonode> consumed;
//	// Store the dual value of the odd-set vertices >= 3 as a map
//	unordered_map<Pseudonode, double, hash<Node>> o_duals;
//
//	// Initialize the slack values for the edges
//	void initialize_slacks() {
//		for (auto it = nodes.begin(); it != nodes.end(); it++) {
//			for (pair<Node, double> nw : it->adj) {
//				slacks[it->index][nw.first.index] = nw.second;
//			}
//		}
//	}
//
//	Graph(vector<Node> &n) {
//		nodes = unordered_set<Node>(n.begin(), n.end());
//		duals = vector<double>(nodes.size(), 0);
//		slacks = vector<vector<double>>(nodes.size(), vector<double>(nodes.size(), 0));
//		initialize_slacks();
//	}
//
//	// Add the specified blossom to the graph and remove the original nodes
//	void shrink_blossom(Pseudonode &p) {
//		nodes.insert(p);
//		for (Node node : p.nodes) {
//			nodes.erase(nodes.find(node));
//			consumed.insert({ node, p });
//		}
//	}
//
//	// Expand a blossom back into its proper nodes
//	void expand_blossom(Pseudonode &p) {
//		nodes.erase(nodes.find(p));
//		for (Node node : p.nodes) {
//			nodes.insert(node); 
//			consumed.erase(node);
//		}
//	}
//
//	// Given [n] a real node, return [n] if this graph contains [n]; otherwise
//	// return the pseudonode that contains [n].
//	Node search(Node n) {
//		while (nodes.find(n) != nodes.end()) n = consumed.find(n)->second;
//		return n;
//	}
//};
//
///** Computes a minimum weight perfect matching in [graph]. Assumes that [graph] has a perfect matching and no pseudonodes. **/
//vector<pair<int, int>> min_wt_perfect_matching(Graph &graph) {
//	// List of unmatched (actual) nodes
//	list<Node> unmatched;
//	for (Node n : graph.nodes) {
//		unmatched.push_back(n);
//	}
//	// The matching to return
//	vector<pair<int, int>> matching;
//
//	// Minimum slack on nodes joining + to outside
//	double ep_bound_a = INFINITY;
//	// Minimum slack on nodes joining + to +
//	double ep_bound_b = INFINITY;
//	// Minimum dual value on - pseudonodes
//	double ep_bound_c = INFINITY;
//
//	while (!unmatched.empty()) {
//		Node root = unmatched.front();
//		ep_bound_a = INFINITY;
//		ep_bound_b = INFINITY;
//		// Create a queue of nodes inside the tree
//		// The second parameter is true iff the node is a + node
//		queue<pair<Node, bool>> tree;
//		tree.push(make_pair(root, true));
//	}
//
//	return matching;
//}

int main() {
	vector<Edge*> adj1, adj2, adj3;
	Node *n1 = new Node(), *n2 = new Node(), *n3 = new Node();
	Edge e1(n1, n2, 1), e2(n2, n3, 2), e3(n3, n1, 3);
	adj1.push_back(&e1);
	adj1.push_back(&e3);
	adj2.push_back(&e1);
	adj2.push_back(&e2);
	adj3.push_back(&e2);
	adj3.push_back(&e3);
	n1->adj = adj1;
	n2->adj = adj2;
	n3->adj = adj3;
	n1->index = 0;
	n2->index = 1;
	n3->index = 2;
	vector<Node*> nodes;
	nodes.push_back(n1);
	nodes.push_back(n2);
	nodes.push_back(n3);
	Graph g(nodes);
	vector<Node*> p1, p2;
	p1.push_back(n1);
	p1.push_back(n2);
	p2.push_back(n3);
	Pseudonode p1n(p1, g);
	g.shrink_blossom(&p1n);
	p2.push_back(&p1n);
	Pseudonode p2n(p2, g);
	g.shrink_blossom(&p2n);
	g.print();
	cout << endl;
	g.expand_blossom(&p2n);
	g.print();
	cout << endl;
	g.expand_blossom(&p1n);
	g.print();
	cout << endl;
	free(n1);
	free(n2);
	free(n3);
	return 0;
}