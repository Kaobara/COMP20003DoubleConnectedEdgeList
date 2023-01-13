#include "structs.h"

// CHecks if a coordinate is in a halfPlane of an halfEdge
int checkHalfPlane(halfEdge_t *halfEdge, coordinate_t point, vertex_t **vertexArray);

// Calculates temporary point to use for halfEdge checking
coordinate_t calcTempPointChecker(halfEdge_t *startHalfEdge, halfEdge_t *endHalfEdge, 
    vertex_t **vertexArray);

// Checks if using correct halfEdge. Else, switch.
void halfEdgeCheckerSwitcher(halfEdge_t *startHalfEdge, halfEdge_t *endHalfEdge, vertex_t **vertexArray);

// Connects prev and next pointers of three edges together
void setPtrStartMidEnd(halfEdge_t *startHalfEdge, halfEdge_t *midHalfEdge, halfEdge_t *endHalfEdge);

// Move the coordinates of the "arrow head" of a half Edge down to the edge's original midpoint
void moveHalfEdgeHeadToMid(halfEdge_t *halfEdge, vertex_t **vertexArray, int *currentNumVertices, 
    int *vertexArraySize);
// Move the coordinates of the "tail" of a half Edge down to the edge's original midpoint
void moveHalfEdgeTailToMid(halfEdge_t *halfEdge, vertex_t **vertexArray, int *currentNumVertices, 
    int *vertexArraySize);

// Makes a new edge used to "split" two edges.
void makeSplitEdge(dcel_t *dcel, int *currentArraySize, int *currentNumEdges, halfEdge_t *startEdge, 
    halfEdge_t *endEdge);

// Splits 2 edges of a polygon
void split(dcel_t *dcel, vertex_t **vertexArray, face_t **faceArray, int *edgeArraySize, 
    int *currentNumEdges, int *vertexArraySize, int *currentNumVertices, int *currentNumFaces, 
    int *faceArraySize, int startEdgeIndex, int endEdgeIndex);
