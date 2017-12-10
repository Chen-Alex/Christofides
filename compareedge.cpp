#include "compareedge.h"

bool CompareEdge::operator() (const Edge &e1, const Edge &e2) {
    return e1.weight > e2.weight;
}
 