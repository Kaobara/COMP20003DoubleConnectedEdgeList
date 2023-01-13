#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"

#define INITAL_FACE_INDEX 0
#define INITAL_VERTEX_INDEX 0
#define START_OF_ARRAY 0
#define OUTSIDE_FACE -1

// This function reads in the coordinates from polygon file and places them into the vertex array
void readVertices(FILE *polygonFile, vertex_t **vertexArray, int *vertexArraySizePtr, int 
    *vertexCounterPtr) {
	while(fscanf(polygonFile, "%lf %lf\n", &(*vertexArray)[*vertexCounterPtr].xCoord, &(*vertexArray)
    [*vertexCounterPtr].yCoord) != EOF) {
        (*vertexCounterPtr)++;
		// If the size of the initial array is too small, increase the array size
        if(*vertexCounterPtr == *vertexArraySizePtr) {
            *vertexArraySizePtr = increaseVertexArray(*vertexArraySizePtr, vertexArray);
        }
	}
}

// Mallocs for the half edge and it's opposite half edge of a new edge
void mallocHalfEdge(dcel_t *dcelPtr, int dcelIndex) {
    (*dcelPtr)[dcelIndex] = (halfEdge_t *) malloc(sizeof(halfEdge_t));
    assert((*dcelPtr)[dcelIndex]);

    // malloc the "opposite" halfEdge_t
    (*dcelPtr)[dcelIndex]->opp = (halfEdge_t *) malloc(sizeof(halfEdge_t));
    assert((*dcelPtr)[dcelIndex]->opp);
}

// Sets the vertex index of a new edge
void newDCELVertices(dcel_t *dcelPtr, int dcelIndex, int vertexStartIndex, int vertexEndIndex) {
    (*dcelPtr)[dcelIndex]->vertexStartIndex = vertexStartIndex;
    (*dcelPtr)[dcelIndex]->vertexEndIndex = vertexEndIndex;
    (*dcelPtr)[dcelIndex]->opp->vertexStartIndex = vertexEndIndex;
    (*dcelPtr)[dcelIndex]->opp->vertexEndIndex = vertexStartIndex;
}

// Special Case: Vertex of final edge of initial polygon will be first vertex
void initalPolyFinalDCELVertex(dcel_t *dcelPtr, int dcelIndex, int vertexIndex) {
    (*dcelPtr)[dcelIndex]->vertexStartIndex = vertexIndex;
    (*dcelPtr)[dcelIndex]->vertexEndIndex = INITAL_VERTEX_INDEX;
    (*dcelPtr)[dcelIndex]->opp->vertexStartIndex = INITAL_VERTEX_INDEX;
    (*dcelPtr)[dcelIndex]->opp->vertexEndIndex = vertexIndex;
}

// Sets the edgeIndex of a new edge
// Since, by my definition, the edge array and dcel is the same, we can use the same indices
void newDCELEdgeIndex(dcel_t *dcelPtr, int edgeIndex) {
    (*dcelPtr)[edgeIndex]->edgeIndex = edgeIndex;
    (*dcelPtr)[edgeIndex]->opp->edgeIndex = edgeIndex;
}

// Sets the faceIndex of a new edge
void newDCELFace(halfEdge_t *halfEdge, face_t **faceArray, int *currentArraySize, int *currentNumFaces) {
    halfEdge->faceIndex = *currentNumFaces;
    (*currentNumFaces)++;
    if(*currentNumFaces == *currentArraySize) {
        *currentArraySize = increaseFaceArray(*currentArraySize, faceArray);
    }
}

// Sets the face index for the edges of the inital polygon (0 for halfEdges inside, and -1 for halfEdges 
// outside)
void firstDCELFace(dcel_t *dcelPtr, int dcelIndex) {
    (*dcelPtr)[dcelIndex]->faceIndex = INITAL_FACE_INDEX;
    (*dcelPtr)[dcelIndex]->opp->faceIndex = OUTSIDE_FACE;
}

// Sets up the opp pointer of the opp half edge for a new edge
void newDCELOppPtrs(dcel_t *dcelPtr, int dcelIndex) {
    (*dcelPtr)[dcelIndex]->opp->opp = (*dcelPtr)[dcelIndex];
}

// Sets up the next and prev for halfEdges
void newDCELNextPrev(halfEdge_t *halfEdge, halfEdge_t *next, halfEdge_t *prev) {
    halfEdge->next = next;
    halfEdge->prev = prev;
    }

// Sets up the inital next and prev for halfEdges for the inital array
void firstDCELNextPrev(dcel_t *dcelPtr, int dcelIndex, int nextIndex, int prevIndex) {
    newDCELNextPrev((*dcelPtr)[dcelIndex], (*dcelPtr)[nextIndex], (*dcelPtr)[prevIndex]);
    newDCELNextPrev((*dcelPtr)[dcelIndex]->opp, (*dcelPtr)[prevIndex]->opp,(*dcelPtr)[nextIndex]->opp);
}

// Takes in a coordinate, and places it into the vertex array
void inputNewVertex(vertex_t **vertexArray, int *currentArraySize, int *currentNumVertices, coordinate_t newCoordinate) {
    (*vertexArray)[*currentNumVertices] = newCoordinate;
    (*currentNumVertices)++;
    if(*currentNumVertices >= *currentArraySize) {
        *currentArraySize = increaseVertexArray(*currentArraySize, vertexArray);
    }
}

// Function that mallocs a new Edge and puts it into the edge array
void makeNewEdge(edge_t **edgeArray, int *currentArraySize, int *currentNumEdges) {
    mallocHalfEdge(edgeArray, *currentNumEdges);
    (*currentNumEdges)++;
    if((*currentNumEdges) == (*currentArraySize)) {
        *currentArraySize = increaseEdgeArray(*currentArraySize, edgeArray);
    }
}

// This function makes a completely new edge, including edge index, mallocing, and its vertices
void inputNewEdge(dcel_t *dcel, int *currentArraySize, int *currentNumEdges, int vertexStartIndex, int vertexEndIndex) {
    makeNewEdge(dcel, currentArraySize, currentNumEdges);
    newDCELEdgeIndex(dcel, (*currentNumEdges)-1);
    newDCELVertices(dcel, (*currentNumEdges)-1, vertexStartIndex, vertexEndIndex);
}

// The function frees an edge
void freeHalfEdges(edge_t **edgeArray, int arraySize) {
    int i;
    for(i=0; i<arraySize; i++) {
        free((*edgeArray)[i]->opp);
        (*edgeArray)[i]->opp = NULL;
        free((*edgeArray)[i]);
        (*edgeArray)[i] = NULL;
    }
    free(*edgeArray);
    *edgeArray = NULL;
}

// This function full frees a DCEL
void freeDCEL(dcel_t *dcel, vertex_t **vertexArray, edge_t **edgeArray, int edgeArraySize) {
    free(*vertexArray);
    *vertexArray = NULL;
    freeHalfEdges(edgeArray, edgeArraySize);
    dcel = NULL;
}

// This takes the initial edges and vertices, and creates the initial polygon and dcel
dcel_t makeDCEL(edge_t **edgeArray, int initialSize, face_t **faceArray) {
    int i;
    dcel_t dcel = *edgeArray;

    for(i=0; i<initialSize; i++) {
        // Malloc all halfEdges of the DCEL
        mallocHalfEdge(&dcel, i);

        if(i == initialSize-1) {        // Special case: The final vertex is the first vertex
            initalPolyFinalDCELVertex(&dcel, i, i);       
        } else {
            newDCELVertices(&dcel, i, i, i+1);
        }

        newDCELEdgeIndex(&dcel, i);
        firstDCELFace(&dcel, i);
        newDCELOppPtrs(&dcel, i);

    }

    // Set prev and next pointers for all half-edges
    for(i=0; i<initialSize; i++) {
        if(i == START_OF_ARRAY) {
            firstDCELNextPrev(&dcel, i, i+1, initialSize-1);
        } else if(i == initialSize-1) {
            firstDCELNextPrev(&dcel, i, START_OF_ARRAY, i-1);
        } else {
            firstDCELNextPrev(&dcel, i, i+1, i-1);
        }
    }

    // Make sure that the first face will point to a halfedge
    faceArray[INITAL_FACE_INDEX]->halfEdgePtr = dcel[0];

    return dcel;
}
