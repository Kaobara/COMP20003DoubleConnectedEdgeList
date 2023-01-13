#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "functions.h"
#include "split.h"

#define INITAL_WATCHTOWER_ARR_SIZE 5

void initalizeFaceWatchtower(face_t **faceArray, int faceNumber) {
    int i, j;
    for(i=0; i<faceNumber; i++) {
        // Initalize value of the original watch tower array size per face
        (*faceArray)[i].watchtowerArr = (csvData_t **) malloc
            (INITAL_WATCHTOWER_ARR_SIZE*sizeof(csvData_t *));
        assert((*faceArray)[i].watchtowerArr);

        (*faceArray)[i].watchArrSize = INITAL_WATCHTOWER_ARR_SIZE;

        // Mallocs data for each watchtower pointer in array to stay consistant with increaseDataArray 
        // function
        for(j=0; j<INITAL_WATCHTOWER_ARR_SIZE; j++) {
            (*faceArray)[i].watchtowerArr[j] = (csvData_t *) malloc(sizeof(csvData_t));
        }

        (*faceArray)[i].numWatchtower = 0;
        (*faceArray)[i].population = 0;
    }

}

int traverseFaceCheckPoint(face_t face, coordinate_t point, vertex_t **vertexArray) {
    int checker;
    halfEdge_t *nextChecker = face.halfEdgePtr;
    halfEdge_t **currentHalfEdge = &face.halfEdgePtr->next;
    checker = checkHalfPlane(face.halfEdgePtr, point, vertexArray);
    
    // Traverse through each halfEdge of a face and check if a coordinate is within all of their halfplanes
    while(nextChecker != (*currentHalfEdge)) {
        if(checker == 0) {
            return 0;
        }
        checker = checkHalfPlane((*currentHalfEdge), point, vertexArray);
        currentHalfEdge = &(*currentHalfEdge)->next;
    }
    return 1;
}

void sortWatchtowerIntoFaces(csvData_t ***dataArray, vertex_t **vertexArray, face_t **faceArray, int dataSize, int faceNumber) {
    coordinate_t tempCoordinate;
    int i, j;
    int faceWatchCheck;
    for(i=0; i<dataSize; i++) {
        // printf("watch tower %d\n", i);
        faceWatchCheck=0;
        tempCoordinate.xCoord = (*dataArray)[i]->xCoord;
        tempCoordinate.yCoord = (*dataArray)[i]->yCoord;

        for(j=0; j<faceNumber; j++) {
            // Checks if the watchtowerArr of a face is full
            if((*faceArray)[j].numWatchtower == (*faceArray)[j].watchArrSize) {
                (*faceArray)[j].watchArrSize = increaseDataArray((*faceArray)[j].
                    watchArrSize, &(*faceArray)[j].watchtowerArr);
            }

            // Checks if the watchtower is in a face
            faceWatchCheck = traverseFaceCheckPoint((*faceArray)[j], tempCoordinate, 
                vertexArray);
            
            // puts a watchtower data pointer into watchtowerArr
            if(faceWatchCheck == 1) {
                free((*faceArray)[j].watchtowerArr[(*faceArray)[j].numWatchtower]);
                (*faceArray)[j].population += (*dataArray)[i]->popServed;
                (*faceArray)[j].watchtowerArr[(*faceArray)[j].numWatchtower] = (*dataArray)
                    [i];
                (*faceArray)[j].numWatchtower++;
                break;
            }
            
        }
    }
}
