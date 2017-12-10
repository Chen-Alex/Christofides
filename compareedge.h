#ifndef _COMPARE_EDGE
#define _COMPARE_EDGE

#include "edge.h"

/** Compare Edges by weight, descending **/
struct CompareEdge {
  bool operator() (const Edge &e1, const Edge &e2);
};

#endif
