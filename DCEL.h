#include "structs.h"

void readVertices(FILE *polygonFile, vertex_t **vertexArray, int *vertexArraySizePtr, int *vertexCounterPtr);
void mallocHalfEdge(dcel_t *dcelPtr, int DCELIndex);
void newDCELVertices(dcel_t *dcelPtr, int DCELIndex, int vertexStartIndex, int vertexEndIndex);
void initalPolyFinalDCELVertex(dcel_t *dcelPtr, int vertexIndex);
void newDCELFace(halfEdge_t *halfEdge, face_t **faceArray, int *currentArraySize, int *currentNumFaces);
void firstDCELFace(dcel_t *dcelPtr, int dcelIndex);
void newDCELOppPtrs(dcel_t *dcelPtr, int DCELIndex);
void newDCELEdgeIndex(dcel_t *dcelPtr, int DCELIndex, int edgeIndex);
void newDCELNextPrev(halfEdge_t *halfEdge, halfEdge_t *next, halfEdge_t *prev);
void firstDCELNextPrev(dcel_t *dcelPtr, int dcelIndex, int nextIndex, int prevIndex);
void inputNewVertex(vertex_t **vertexArray, int *currentSize, int *currentNumVertices, coordinate_t newCoordinate);
void makeNewEdge(edge_t **edgeArray, int *currentArraySize, int *currentNumEdges, edge_t newEdge);
void inputNewEdge(dcel_t *dcel, int *currentArraySize, int *currentNumEdges, int vertexStartIndex, int vertexEndIndex);
dcel_t makeDCEL(edge_t **edgeArray, int initialSize, face_t **faceArray);
void freeHalfEdges(edge_t **edgeArray, int arraySize);
void freeDCEL(dcel_t *dcel, vertex_t **vertexArray, edge_t **edgeArray, int edgeArraySize);
