#include <unordered_set>
#include <unordered_map>
#include <vector> 
#include <iostream>
#include <queue>

using namespace std;

// This doesn't work.

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
struct Pseudonode;

struct Node {
	int index;
	bool is_pseudo = false;
	double dual = 0;
	// 1 indicates a + node, -1 indicates a - node, 0 indicates an outside node
	int type = 0;
	// The parent of this node in the tree
	Node *parent = 0;
	// The edge connecting this node to its parent in the tree 
	Edge *edge_to_parent = 0;
	// The index of the first modification (i.e. epsilon) that this node was a part of
	// -1 means unchanged
	int fst_update = -1;

	// Adjacency list
	vector<pair<Node*, Edge*>> adj;

	bool operator==(const Node &n) const {
		return index == n.index;
	}

	/*size_t hash() const {
	return std::hash<int>()(index);
	}*/

	// Returns the edges in a path starting from this node and following parent pointers until
	// one of the following conditions is met:
	//     - The current node in the path is null
	//     - The current node in the path is the designated [goal] node
	vector<Edge*> get_path_edges(Node *goal) {
		Node *cur = this;
		vector<Edge*> edges;
		while (cur != NULL && !(*cur == *goal)) {
			edges.push_back(cur->edge_to_parent);
			cur = cur->parent;
		}
		return edges;
	}

	// Same as get_path_edges, except returns a list of nodes rather than edges
	vector<Node*> get_path_nodes(Node *goal) {
		Node *cur = this;
		vector<Node*> nodes; 
		while (cur != NULL && !(*cur == *goal)) {
			nodes.push_back(cur);
			cur = cur->parent;
		}
		if (*cur == *goal) nodes.push_back(cur);
		return nodes;
	}

	// Return a vector of nodes in the path to the l.c.a of this node with [n]
	vector<Node*> nodes_to_lca(Node *n) {
		Node *a = this, *b = n;
		vector<Node*> nodes;
		unordered_set<Node*> path_a, path_b;
		if (path_a.find(b) == path_a.end()) {
			path_a.insert(a);
			a = a->parent;
		}
		if (path_b.find(a) == path_b.end()) {
			path_b.insert(b);
			b = b->parent;
		}
		return nodes;
	}

	void update_duals(Graph *graph);

	double true_dual(Graph *graph);

	Node(int ind, vector<pair<Node*, Edge*>> &a) {
		index = ind;
		adj = a;
	}

	Node() {}
};

struct Edge {
	Node *node1, *node2;
	// Stores weight - sum of duals on odd sets > 3
	// subtract duals of individual vertices on the fly
	double partial_slack;
	double weight;
	// Store a pointer to the graph
	Graph *graph;
	// Store a set of psuedonodes that this edge is on the boundary of
	unordered_set<Pseudonode*> pseudos;

	// Edge construction must happen before blossom algorithm is run
	Edge(Node *n1, Node *n2, double wt) {
		node1 = n1;
		node2 = n2;
		weight = wt;
		partial_slack = wt;
	}

	// Given a vector of sums of update values, compute the modified 
	// slack of this edge.
	double slack();

	// Return the other node corresponding to this edge
	/*Node *other(Node *n) {
		if (n->index == node1->index) return node2;
		else if (n->index == node2->index) return node1;
		else return 0; // Shouldn't ever happen
	}*/
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
	// Store the absolute values of the sums of the first k dual changes in dual_changes[k]
	vector<double> dual_changes;
	// A matching in the graph
	unordered_set<Edge*> matching;
	// A set of unmatched nodes
	unordered_set<Node*> unmatched;
	// Maps nodes to their matched partners in the graph
	unordered_map<Node*, pair<Node*, Edge*>> matches;

	Graph(vector<Node*> v) {
		nodes = unordered_set<Node*>(v.begin(), v.end());
		unmatched = nodes;
		// Edges in this graph need a pointer to this graph; set these pointers
		for (Node *n : v) {
			for (pair<Node*, Edge*> a : n->adj) {
				a.second->graph = this;
			}
		}
	}

	// Add the specified blossom to the graph and remove the original nodes
	void shrink_blossom(Pseudonode *p) {
		nodes.insert(p);
		for (Node *node : p->nodes) {
			nodes.erase(nodes.find(node));
			consumed.insert({ node, p });
			// When we shrink a blossom we need to fix its nodes' dual values
			node->update_duals(this);
			// Also it is no longer part of the tree
			node->type = 0;
		}
		// Add this pseudonode to the [pseudo]s of relevant edges
		for (pair<Node*, Edge*> ne : p->adj) {
			ne.second->pseudos.insert(p);
		}
	}

	// Expand a blossom back into its proper nodes
	void expand_blossom(Pseudonode *p) {
		nodes.erase(nodes.find(p));
		for (Node *node : p->nodes) {
			nodes.insert(node);
			consumed.erase(node);
		}
		// Remove this pseudonode from the [pseudo]s of relevant edges
		for (pair<Node*, Edge*> ne : p->adj) {
			ne.second->pseudos.erase(p);
		}
	}

	// Given [n] a real node, return [n] if this graph contains [n]; otherwise
	// return the pseudonode that contains [n].
	Node *search(Node *n) {
		while (nodes.find(n) == nodes.end()) n = consumed.find(n)->second;
		return n;
	}

	// Reset a node by making its type 0, nullifying its parent and edge_to_parent, and setting
	// its fst_update to 0. If [node] is a pseudonode also recursively resets the nodes contained
	// inside [node].
	void reset_node(Node *node) {
		node->type = 0;
		node->parent = NULL;
		node->edge_to_parent = NULL;
		node->fst_update = -1;
		if (node->is_pseudo) {
			Pseudonode *p = (Pseudonode *)node;
			for (Node *n : p->nodes) {
				reset_node(n);
			}
		}
	}

	// Reset every node by making their type 0, nullifying their parent and edge_to_parent, and setting
	// their fst_update to 0. Recursively travels through pseudonodes.
	void reset_nodes() {
		for (Node *n : nodes) {
			reset_node(n);
		}
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
			for (pair<Node*, Edge*> p : n->adj) {
				Edge *e = p.second;
				cout << "( ";
				cout << "<" << e->node1->index << "," << e->node2->index << ">, ";
				cout << "weight = " << e->weight << ", ";
				cout << "slack = " << e->slack() << " ) ";
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
		for (pair<Node*, Edge*> ne : n->adj) {
			if (nodes.find(graph.search(ne.first)) == nodes.end()) {
				adj.push_back(ne);
			}
			// This requires that graph is up-to-date (other than this blossom).
			// Can use [expanded_nodes] instead, but it's likely slower.
			// Looking to remove [expanded_nodes] in the final version...
			//auto f1 = nodes.find(graph.search(e->node1));
			//auto f2 = nodes.find(graph.search(e->node2));
			//if (f1 == nodes.end() ^ f2 == nodes.end()) {
			//	adj.push_back(e);
			//}
		}
	}
}

double Edge::slack() {
	vector<double> *sum_updates = &(graph->dual_changes);
	double d1 = node1->dual, d2 = node2->dual;
	int fst1 = node1->fst_update, fst2 = node2->fst_update;
	double eps1, eps2;
	double total = (*sum_updates)[sum_updates->size() - 1];
	eps1 = fst1 >= 0 ? total - (*sum_updates)[fst1] : 0;
	eps2 = fst2 >= 0 ? total - (*sum_updates)[fst2] : 0;
	d1 += node1->type * eps1;
	d2 += node2->type * eps2;
	double d = 0;
	for (Pseudonode *p : pseudos) {
		int fst = p->fst_update;
		double eps = fst >= 0 ? total - (*sum_updates)[fst] : 0;
		d += p->type * eps;
	}
	return partial_slack - d1 - d2 - d;
}

/** A class to compare edges based on their skacks in a p.q. **/
struct CompareEdge {
	bool operator() (pair<double, Edge*> e1, pair<double, Edge*> e2)  {
		return e1.first > e2.first;
	}
};

/** A class to compare pseudonodes based on their duals **/
struct CompareBlossom {
	bool operator() (pair<double, Pseudonode*> p1, pair<double, Pseudonode*> p2) {
		return p1.first > p2.first;
	}
};

// Update the dual of this node with the information in [graph]
void Node::update_duals(Graph *graph) {
	if (fst_update >= 0) {
		dual += type * (graph->dual_changes.back() - graph->dual_changes[fst_update]);
	}
	fst_update = -1;
}

// Get the true dual based on the graph
double Node::true_dual(Graph *graph) {
	return fst_update == -1 ? dual : dual + type * (graph->dual_changes.back() - graph->dual_changes[fst_update]);
}

/** Find a (shortest) augmenting path in [graph] and update the matching with it**/
void augmenting_path(Graph &graph) {
	Node *r = *(graph.unmatched.begin());
	graph.reset_nodes();
	graph.dual_changes.clear();
	// Edges that join + nodes in T to nodes outside of T
	// We need to insert a modified slack in order to allow for ``fair''
	// comparison between edges that have already been updated but not in the queue
	priority_queue<pair<double, Edge*>, vector<pair<double, Edge*>>, CompareEdge> q_evenout;
	priority_queue<pair<double, Edge*>, vector<pair<double, Edge*>>, CompareEdge> q_eveneven;
	// The duals of the relevant blossoms, modified to match the current iteration
	priority_queue<pair<double, Pseudonode*>, vector<pair<double, Pseudonode*>>, CompareBlossom> q_pseudo;
	// BFS queue for expansion of T
	queue<Node*> bfsq;
	bfsq.push(r);
	r->type = 1;
	// The update iteration
	int iteration = 0;
	graph.dual_changes.push_back(0);
loop:
	while (!bfsq.empty()) {
		Node *cur = bfsq.front();
		bfsq.pop();
		if (graph.nodes.find(cur) == graph.nodes.end()) continue;
		// If the current node is a - pseudonode, insert it into [q_pseudo]
		if (cur->is_pseudo && cur->type == -1) {
			q_pseudo.push(make_pair(cur->true_dual(&graph) + graph.dual_changes.back(), (Pseudonode *)cur));
		}
		if (cur->fst_update == -1) cur->fst_update = iteration;
		// If this node is a - node, check for a matching
		if (cur->type == -1) {
			auto find = graph.matches.find(cur);
			if (find != graph.matches.end()) {
				Node *partner = find->second.first;
				partner->parent = cur;
				partner->type = 1;
				partner->edge_to_parent = find->second.second;
				bfsq.push(partner);
			}
			// Else, we have found an augmenting path
			else {
				vector<Edge*> path = cur->get_path_edges(r);
				for (Edge *e : path) {
					graph.unmatched.erase(e->node1); 
					graph.unmatched.erase(e->node2);
					auto find = graph.matching.find(e);
					if (graph.matching.find(e) != graph.matching.end()) {
						Edge *f = *find;
						graph.matches.erase(f->node1);
						graph.matches.erase(f->node2);
						graph.matching.erase(f);
					}
					else {
						graph.matching.insert(e);
						graph.matches.insert({ e->node1, make_pair(e->node2, e) });
						graph.matches.insert({ e->node2, make_pair(e->node1, e) });
					}
				}
				for (Node *n : graph.nodes) {
					n->update_duals(&graph);
				}
				return;
			}
			continue;
		}
		// Now assumming cur is a + node
		for (pair<Node*, Edge*> nbr : cur->adj) {
			Node *n = graph.search(nbr.first);
			Edge *e = nbr.second;
			double sl = e->slack();
			// If the edge is not tight, we may have to insert into a priority queue
			if (sl > 0) {
				// Insert into the (+, +) queue
				if (n->type == 1) {
					q_eveneven.push(make_pair(sl + 2 * graph.dual_changes.back(), e));
				}
				// Insert into the (+, 0) queue
				else if (n->type == 0) {
					q_evenout.push(make_pair(sl + graph.dual_changes.back(), e));
				}
			}
			// If the edge is tight, we may need to shrink a blossom
			else if (n->type == 1) {
				vector<Node*> blossom = cur->nodes_to_lca(n);
				Pseudonode pseudo(blossom, graph);
				graph.shrink_blossom(&pseudo);
				bfsq.push(&pseudo);
			}
			// Otherwise, if the edge is tight and not yet in the tree, assimilate it
			else if (n->type == 0) {
				n->parent = cur;
				n->edge_to_parent = e;
				n->type = -(cur->type);
				bfsq.push(n);
			}
		}
	}
	// Now we are stuck, figure out why
	Edge *eo = NULL, *ee = NULL;
	Pseudonode *p = NULL;
	double seo = DBL_MAX, see = DBL_MAX, dual = DBL_MAX;
	if (!q_evenout.empty()) {
		eo = q_evenout.top().second;
		seo = eo->slack();
	}
	if (!q_eveneven.empty()) {
		ee = q_eveneven.top().second;
		see = ee->slack();
	}
	if (!q_pseudo.empty()) {
		p = q_pseudo.top().second;
		dual = p->true_dual(&graph);
	}
	// Slack could be zero
	while (seo == 0) {
		q_evenout.pop();
		eo = q_evenout.top().second;
		seo = eo->slack();
	}
	while (see == 0) {
		q_eveneven.pop();
		ee = q_eveneven.top().second;
		see = ee->slack();
	}
	// If [seo] is the minimum slack, we have a new tight edge
	if (seo <= see && seo <= dual) {
		graph.dual_changes.push_back(graph.dual_changes[iteration] + seo);
		iteration++;
		q_evenout.pop();
		Node *ne, *no;
		if (eo->node1->type == 0) {
			no = eo->node1; 
			ne = eo->node2;
		}
		else {
			no = eo->node2;
			ne = eo->node1;
		}
		no->type = -1;
		no->parent = ne;
		no->edge_to_parent = eo;
		bfsq.push(no);
		goto loop;
	}
	// If [see] is the minimum slack, it is a new tight edge
	else if (see <= seo && see <= dual) {
		graph.dual_changes.push_back(graph.dual_changes[iteration] + see);
		iteration++;
		q_eveneven.pop();
		vector<Node*> blossom = ee->node1->nodes_to_lca(ee->node2);
		Pseudonode pseudo(blossom, graph);
		graph.shrink_blossom(&pseudo);
		bfsq.push(&pseudo);
		goto loop;
	}
	// Otherwise, we need to expand a blossom
	else {
		graph.dual_changes.push_back(graph.dual_changes[iteration] + dual);
		iteration++;
		q_pseudo.pop();
		graph.expand_blossom(p);
		for (Node *n : p->nodes) {
			bfsq.push(n);
		}
		goto loop;
	}
}

/** Computes a minimum weight perfect matching in [graph]. Assumes that [graph] has a perfect matching and no pseudonodes. **/
vector<Edge*> min_wt_perfect_matching(Graph &graph) {
	while (!graph.unmatched.empty()) {
		augmenting_path(graph);
	}
	return vector<Edge*>(graph.matching.begin(), graph.matching.end());
}

//int main() {
//	vector<pair<Node*, Edge*>> adj1, adj2, adj3, adj4;
//	Node *n1 = new Node(), *n2 = new Node(), *n3 = new Node(), *n4 = new Node();
//	Edge e1(n1, n2, 1), e2(n2, n3, 2), e3(n3, n4, 3), e4(n4, n1, 5);
//	adj1.push_back(make_pair(n2, &e1));
//	adj1.push_back(make_pair(n4, &e4));
//	adj2.push_back(make_pair(n1, &e1));
//	adj2.push_back(make_pair(n3, &e2));
//	adj3.push_back(make_pair(n2, &e2));
//	adj3.push_back(make_pair(n4, &e3));
//	adj4.push_back(make_pair(n1, &e4));
//	adj4.push_back(make_pair(n3, &e3));
//	n1->adj = adj1;
//	n2->adj = adj2;
//	n3->adj = adj3;
//	n4->adj = adj4;
//	n1->index = 0;
//	n2->index = 1;
//	n3->index = 2;
//	n4->index = 3;
//	vector<Node*> nodes;
//	nodes.push_back(n1);
//	nodes.push_back(n2);
//	nodes.push_back(n3);
//	nodes.push_back(n4);
//	Graph g(nodes);
//	g.dual_changes.push_back(0);
//	min_wt_perfect_matching(g);
//	/*vector<Node*> p1, p2;
//	p1.push_back(n1);
//	p1.push_back(n2);
//	p2.push_back(n3);
//	Pseudonode p1n(p1, g);
//	g.shrink_blossom(&p1n);
//	p2.push_back(&p1n);
//	Pseudonode p2n(p2, g);
//	g.shrink_blossom(&p2n);
//	g.print();
//	cout << endl;
//	g.expand_blossom(&p2n);
//	g.print();
//	cout << endl;
//	g.expand_blossom(&p1n);
//	g.print();
//	cout << endl;
//	free(n1);
//	free(n2);
//	free(n3);*/
//	return 0;
//}