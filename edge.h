#ifndef _EDGE
#define _EDGE

/** Structure that represents an undirected edge between two nodes.
    Comparison of two edges compares their weights.
    Two edges are the same if they connect the same nodes, in either order. 
  **/
struct Edge {
  int node1, node2;
  double weight;

  Edge(int n1, int n2, double w);

  /* The equals function checks if this edge connects the same nodes as edge [e] and has the same weight.*/
  bool equals(Edge e);
};

#endif 
