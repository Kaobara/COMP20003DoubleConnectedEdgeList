#ifndef STRUCTDEFINED
#define STRUCTDEFINED

/* These are structs that can be used for the program */
// CSV Data struct
typedef struct{
    char *watchID;
    char *postcode;
    int popServed;
    char *watchContactName;
    double xCoord;
    double yCoord;
}csvData_t;

// DCLE structs
typedef struct halfEdge halfEdge_t;
typedef struct coordinate coordinate_t;
typedef halfEdge_t *edge_t;
typedef edge_t *dcel_t;
typedef coordinate_t vertex_t;

/* Coordinate used for temporary points and vertices. On a 2D plane, only contains
an x and y coordinate */
struct coordinate {
    double xCoord;
    double yCoord;
};

/* The main part of the DCEL, the halfEdge, which contains the vertices of the edge, the 
face and edge it is a part of, and pointers that points to the previous, next pointers and
the twin halfEdge that makes up the same edge */
struct halfEdge {
    int vertexEndIndex;
    int vertexStartIndex;
    halfEdge_t *next;
    halfEdge_t *prev;
    halfEdge_t *opp;
    int faceIndex;
    int edgeIndex;
};

/* A face struct that is made of halfEdges. As all halfEdges that belongs to a face would have
an index that leads back to its corresponding face, each face only needs a single pointer 
one of the half Edges. For the purposes of this specific program, it also contains 
space for data for watchtowers. */
typedef struct {
    halfEdge_t *halfEdgePtr;
    csvData_t **watchtowerArr;
    int numWatchtower;
    int watchArrSize;
    int population;
}face_t;

#endif
