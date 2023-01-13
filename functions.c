#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "functions.h"

#define MAXLINE 512

// Check if a character is a comma
int checkComma(char ch, int counter) {
    if(ch == ',') {
        counter += 1;
    }
    return counter;
}

// if the current array size is too small, realloc
int increaseDataArray(int currentSize, csvData_t ***dataArray) {
    int i;
    *dataArray = (csvData_t **) realloc(*dataArray, currentSize*2*sizeof(csvData_t *));
    assert(*dataArray);
    for(i = currentSize; i<(currentSize*2); i++) {
            (*dataArray)[i] = (csvData_t *) malloc(sizeof(csvData_t));
            assert((*dataArray)[i]);
		}
    return currentSize*2;
}

// Increase the size of the Vertex array
int increaseVertexArray(int currentSize, vertex_t **verticeArray) {
    currentSize *= 2;
    *verticeArray = (vertex_t *)realloc((*verticeArray), currentSize*sizeof(vertex_t));
    assert(*verticeArray);
    return currentSize;
}

// Increase the size of the Edge/DCEL array
int increaseEdgeArray(int currentSize, edge_t **edgeArray) {
    currentSize *= 2;
    *edgeArray = realloc((*edgeArray), currentSize*sizeof(edge_t));
    assert(*edgeArray);
    return currentSize;
}

// Increase the size of the Face array
int increaseFaceArray(int currentSize, face_t **faceArray) {
    currentSize *= 2;
    *faceArray = realloc((*faceArray), currentSize*sizeof(face_t));
    assert(*faceArray);
    return currentSize;
}

// Free the array of pointers containing csv data
void freeCSVPtrArray(csvData_t ***csvPtrArray, int itemNumber, int arraySize) {
    int i;
    for(i=0; i<itemNumber; i++) {
        free((*csvPtrArray)[i]->watchID);
        free((*csvPtrArray)[i]->postcode);
        free((*csvPtrArray)[i]->watchContactName);
    }
    for(i=0; i<arraySize; i++) {
        free((*csvPtrArray)[i]);
        (*csvPtrArray)[i] = NULL;
    }
    free(*csvPtrArray);
    *csvPtrArray = NULL;
}

// Calculates the gradient between 2 points
double calcGrad(coordinate_t point1, coordinate_t point2) {
    return (point2.yCoord - point1.yCoord)/(point2.xCoord - point1.xCoord);
}

// Calculates the c-intercept between 2 points
double calcInter(coordinate_t point, double grad) {
    return point.yCoord - (grad*point.xCoord);
}

// Calculate the mid point coordinates of 2 given points
coordinate_t calcMidPoint(coordinate_t startPoint, coordinate_t endPoint) {
    coordinate_t midPoint;
    midPoint.xCoord = (startPoint.xCoord + endPoint.xCoord)/2;
    midPoint.yCoord = (startPoint.yCoord + endPoint.yCoord)/2;
    return midPoint;
}
