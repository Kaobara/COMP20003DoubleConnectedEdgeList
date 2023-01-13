#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"
#include "DCEL.h"

// A function to that returns true or false whether a point lies on a half plane
int checkHalfPlane(halfEdge_t *halfEdge, coordinate_t point, vertex_t **vertexArray) {
    coordinate_t point1 = (*vertexArray)[halfEdge->vertexStartIndex];
    coordinate_t point2 = (*vertexArray)[halfEdge->vertexEndIndex];
    double x1 = point1.xCoord;
    double y1 = point1.yCoord;
    double x2 = point2.xCoord;
    double y2 = point2.yCoord;
    double mGrad;
    double cInter;
    double yPredict;

    // If the line is a vertical line, check if the coordinates is to the left or right it.
    if(x1 == x2) {
        if(y2 > y1) {
            if(point.xCoord > x1) {
                return 1;
            } else {
                return 0;
            }
        } else {
            if(point.xCoord < x1) {
                return 1;
            } else {
                return 0;
            }
        }
    } else {
        // If the line is sloping or horizontal, use the gradient to check if point is in the half plane
        mGrad = calcGrad(point1, point2);
        cInter = calcInter(point2, mGrad);
        yPredict = mGrad*point.xCoord + cInter;
        if(x2 > x1) {
            if((point.yCoord - yPredict) <= 0) {
                return 1;
            } else {
                return 0;
            }
        } else if (x2 < x1) {
            if((point.yCoord - yPredict) >= 0) {
                return 1;
            } else {
                return 0;
            }
        }
    }
    printf("There was an error in checkHalfPlane\n");
    return 1;
}

// This function calculates a temporary point to use for checking which halfEdge to use
coordinate_t calcTempPointChecker(halfEdge_t *startHalfEdge, halfEdge_t *endHalfEdge, vertex_t **vertexArray) {
    coordinate_t startMid = calcMidPoint((*vertexArray)[startHalfEdge->vertexStartIndex], 
        (*vertexArray)[startHalfEdge->vertexEndIndex]);
    coordinate_t endMid = calcMidPoint((*vertexArray)[endHalfEdge->vertexStartIndex], (*vertexArray)[endHalfEdge->vertexEndIndex]);

    return calcMidPoint(startMid, endMid);
}

// This function checks if we're using the correct halfEdges using half planes.
// If we're using the wrong halfEdge, switch the half edge to its opposite half edge.
void halfEdgeCheckerSwitcher(halfEdge_t **startHalfEdge, halfEdge_t **endHalfEdge, vertex_t **vertexArray) {
    int checker;

    coordinate_t avrgVertex = calcTempPointChecker(*startHalfEdge, *endHalfEdge, vertexArray);
    checker = checkHalfPlane(*startHalfEdge, avrgVertex, vertexArray);
    if(checker == 0) {
        *startHalfEdge = (*startHalfEdge)->opp;
    }
    checker = checkHalfPlane(*endHalfEdge, avrgVertex, vertexArray); 
    if(checker == 0) {
        *endHalfEdge = (*endHalfEdge)->opp;
    }
}

// Start of Split, moving the "head" of the start Half Edge to the midpoint
void moveHalfEdgeHeadToMid(halfEdge_t *halfEdge, vertex_t **vertexArray, int *currentNumVertices, int *vertexArraySize) {
    coordinate_t midPoint = calcMidPoint((*vertexArray)[halfEdge->vertexStartIndex], 
        (*vertexArray)[halfEdge->vertexEndIndex]);

    halfEdge->vertexEndIndex = (*currentNumVertices);      //inputNewVertex increases numVertex by 1. 
    halfEdge->opp->vertexStartIndex = (*currentNumVertices);

    // After making the new midpoint, add it into the vertex array
    inputNewVertex(vertexArray, vertexArraySize, currentNumVertices, midPoint);
}

// Start of Split, moving the "tail" of the end Half Edge to the midpoint
void moveHalfEdgeTailToMid(halfEdge_t *halfEdge, vertex_t **vertexArray, int *currentNumVertices, int *vertexArraySize) {
    coordinate_t midPoint = calcMidPoint((*vertexArray)[halfEdge->vertexStartIndex], 
        (*vertexArray)[halfEdge->vertexEndIndex]);

    // Set the indices of the half edges
    halfEdge->vertexStartIndex = (*currentNumVertices);      
    halfEdge->opp->vertexEndIndex = (*currentNumVertices);

    // After making the new midpoint, add it into the vertex array
    inputNewVertex(vertexArray, vertexArraySize, currentNumVertices, midPoint);
}

// Creates the new splitting edge
void makeSplitEdge(dcel_t *dcel, int *currentArraySize, int *currentNumEdges, halfEdge_t *startEdge, halfEdge_t *endEdge) {
    inputNewEdge(dcel, currentArraySize, currentNumEdges, startEdge->vertexEndIndex, 
        endEdge->vertexStartIndex);
}

// From the halfEdge that the face is pointing to, traverse through every halfEdge belonging to the face, and set the face index for the half edges
void traverseSetFace(halfEdge_t *rootHalfEdge, int faceIndex) {
    halfEdge_t *nextChecker = rootHalfEdge;
    halfEdge_t **currentHalfEdge = &rootHalfEdge->next;

    rootHalfEdge->faceIndex = faceIndex;
    while(nextChecker != (*currentHalfEdge)) {
        (*currentHalfEdge)->faceIndex = faceIndex;
        currentHalfEdge = &(*currentHalfEdge)->next;
        }
}

// Connects 3 halfEdges together, a "start", a "middle", and an "end" halfEdge.
void setPtrStartMidEnd(halfEdge_t *startHalfEdge, halfEdge_t *midHalfEdge, halfEdge_t *endHalfEdge) {
    startHalfEdge->next = midHalfEdge;
    endHalfEdge->prev = midHalfEdge;
    midHalfEdge->prev = startHalfEdge;
    midHalfEdge->next = endHalfEdge;
}

void split(dcel_t *dcel, vertex_t **vertexArray, face_t **faceArray, int *edgeArraySize, int *currentNumEdges, int *vertexArraySize, int *currentNumVertices, int *faceArraySize, int *currentNumFaces, int startEdgeIndex, int endEdgeIndex) {
    halfEdge_t *startHalfEdge = (*dcel)[startEdgeIndex];
    halfEdge_t *endHalfEdge = (*dcel)[endEdgeIndex];
    int originHeadVertexIndex;
    int originTailVertexIndex;
    halfEdge_t *splitHalfEdge;
    halfEdge_t *splitHalfEdgeOther;
    halfEdge_t *newHalfEdgeOne;
    halfEdge_t *newHalfEdgeTwo;
    halfEdge_t *tempStartNext = 0;
    halfEdge_t *tempEndPrev = 0;

    // Checks is we're using the correct halfEdges, or if we should switch to opposite
    // if incorrect, it will automatically switch the halfEdge pointers to its counterpart
    halfEdgeCheckerSwitcher(&startHalfEdge, &endHalfEdge, vertexArray);

    // Store the original vertices before changing the edges
    originHeadVertexIndex = startHalfEdge->vertexEndIndex;
    originTailVertexIndex = endHalfEdge->vertexStartIndex;

    // Move the head of start edge and tail of end edge to its midpoint
    moveHalfEdgeHeadToMid(startHalfEdge, vertexArray, currentNumVertices, vertexArraySize);
    moveHalfEdgeTailToMid(endHalfEdge, vertexArray, currentNumVertices, vertexArraySize);

    /* Make the new edges */
    // Add in the splitEdge. Note, face, prev, and next has not yet been changed
    makeSplitEdge(dcel, edgeArraySize, currentNumEdges, startHalfEdge, endHalfEdge);
    splitHalfEdge = (*dcel)[(*currentNumEdges)-1];
    splitHalfEdgeOther = splitHalfEdge->opp;
    splitHalfEdgeOther->opp = splitHalfEdge;

    inputNewEdge(dcel, edgeArraySize, currentNumEdges, startHalfEdge->vertexEndIndex, originHeadVertexIndex);
    newHalfEdgeOne = (*dcel)[(*currentNumEdges)-1];

    inputNewEdge(dcel, edgeArraySize, currentNumEdges, originTailVertexIndex, endHalfEdge->vertexStartIndex);
    newHalfEdgeTwo = (*dcel)[(*currentNumEdges)-1];

    /* Start linking next and prevs */
    // Special case, if the two edges are adjacent and startEdge's next points to endEdge
    if(startHalfEdge->next != endHalfEdge) {
        tempStartNext = startHalfEdge->next;
        tempEndPrev = endHalfEdge->prev;
    }
    setPtrStartMidEnd(startHalfEdge, splitHalfEdge, endHalfEdge);
    setPtrStartMidEnd(newHalfEdgeTwo, splitHalfEdgeOther, newHalfEdgeOne);

    // Normal case, affects the pointers of other edges
    if(tempStartNext) {
        newHalfEdgeOne->next = tempStartNext;
        newHalfEdgeTwo->prev = tempEndPrev;

        // change the prev's next to the new edges
        newHalfEdgeOne->next->prev = newHalfEdgeOne;
        newHalfEdgeTwo->prev->next = newHalfEdgeTwo;
    } else {        // special case, the new face is a triangle.
        newHalfEdgeOne->next = newHalfEdgeTwo;
        newHalfEdgeTwo->prev = newHalfEdgeOne;
    }

    // Start updating the opposite/other halfEdges
    // Special case where the other halfEdges are adjacent pointing to each other
    if(endHalfEdge->opp->next == startHalfEdge->opp) {
        newHalfEdgeOne->opp->prev = newHalfEdgeTwo->opp;
        newHalfEdgeTwo->opp->next = newHalfEdgeOne->opp;
    } else {
         newHalfEdgeOne->opp->prev = startHalfEdge->opp->prev;
         newHalfEdgeOne->opp->prev->next = newHalfEdgeOne->opp;
         newHalfEdgeTwo->opp->next = endHalfEdge->opp->next;
         newHalfEdgeTwo->opp->next->prev = newHalfEdgeTwo->opp;
    }
    newHalfEdgeOne->opp->next = startHalfEdge->opp;
    newHalfEdgeOne->opp->next->prev = newHalfEdgeOne->opp;
    newHalfEdgeTwo->opp->prev = endHalfEdge->opp;
    newHalfEdgeTwo->opp->prev->next = newHalfEdgeTwo->opp;

    newHalfEdgeOne->opp->opp = newHalfEdgeOne;
    newHalfEdgeTwo->opp->opp = newHalfEdgeTwo;

    /* Start making and setting up the faces */
    // Split half edge will always follow the face the edges being split
    splitHalfEdge->faceIndex = startHalfEdge->faceIndex;
    newHalfEdgeOne->faceIndex = startHalfEdge->faceIndex;
    newHalfEdgeTwo->faceIndex = startHalfEdge->faceIndex;
    (*faceArray)[splitHalfEdge->faceIndex].halfEdgePtr = splitHalfEdge;

    // Other split half edge will always be a new face
    newDCELFace(splitHalfEdgeOther, faceArray, faceArraySize, currentNumFaces);
    (*faceArray)[(*currentNumFaces)-1].halfEdgePtr = splitHalfEdgeOther;

    // Set the face index of all edges in new face as the new face
    traverseSetFace(splitHalfEdgeOther, splitHalfEdgeOther->faceIndex);
    traverseSetFace(newHalfEdgeOne->opp->next, newHalfEdgeOne->opp->next->faceIndex);
    traverseSetFace(newHalfEdgeTwo->opp->next, newHalfEdgeTwo->opp->next->faceIndex);
}
