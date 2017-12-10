#include "edge.h"

using namespace std;

Edge::Edge(int n1, int n2, double w) {
  node1 = n1;
  node2 = n2;
  weight = w;
}

bool Edge::equals(Edge e) {
  return ((e.node1 == node1 && e.node2 == node2) || (e.node1 == node2 && e.node2 == node1)) && weight == e.weight;
}
