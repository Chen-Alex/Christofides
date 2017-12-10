#include "graph.h"

using namespace std;

Graph::Graph(vector<vector<pair<int,double>>> &a) {
  n_nodes = a.size();
  adj = a;
}

Graph::Graph(vector<Edge> edges) {
  n_nodes = 0;
  for (Edge e : edges) {
    n_nodes = max(n_nodes, e.node1 + 1);
    n_nodes = max(n_nodes, e.node2 + 1);

    while (adj.size() < n_nodes) adj.push_back(vector<pair<int,double>>());
    adj[e.node1].push_back(make_pair(e.node2,e.weight));
    adj[e.node2].push_back(make_pair(e.node1, e.weight));
  }
}

void Graph::print() {
  for (int i = 0; i < adj.size(); i++) {
      cout << i << ": ";
      for (pair<int, double> conn : adj[i]) {
        cout << "(" << conn.first << ", " << conn.second << ") ";
      }
      cout << endl;
  }
}

void Graph::remove_edge(int node, int index) {
  if (node >= adj.size() || index >= adj[node].size()) return;
  int other_node = adj[node][index].first;
  pair<int, double> to_find = make_pair(node, adj[node][index].second);
  adj[other_node].erase(find(adj[other_node].begin(), adj[other_node].end(), to_find));
  adj[node].erase(adj[node].begin() + index);
}





