#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "functions.h"
#include "readDataset.h"
#include "DCEL.h"
#include "split.h"
#include "watchtower.h"

#define DATA_STR_SIZE 512
#define MAXLINE 512 
#define MAXFILE 3
#define INITIAL_PTR_ARRAY_SIZE 2
#define CSV_DATA_FILE 0
#define INITIAL_POLY_FILE 1
#define OUTPUT_FILE 2
#define INITIAL_NUM_VERTICES 5
#define INITAL_NUM_FACES 2

int main(int argc, char **argv) {
	/* Setup and construction can go here! */

	/* Setting up - Opening the files */
    FILE *fp[MAXFILE];
    int i, j;
    // Open the dataset and initial vertice files as read
    for(i=1; i<argc-1; i++) {
        fp[i-1] = fopen(argv[i], "r");
        if((fp[i-1]) == NULL) {
            exit(EXIT_FAILURE);
        }
    }

    // Open the last file (output) as write
    fp[OUTPUT_FILE] = fopen(argv[i], "w");
    if (fp[OUTPUT_FILE] == NULL) {
        fprintf(stderr, "Cannot open this file: %s\n", argv[argc]);
        exit(EXIT_FAILURE);
        }

	/* Part 1 - Create a Dynamic Array of pointers to structs with data from CSV*/
	// Dynamic array of pointers, each pointer to a csvData_t
    csvData_t **csvPtrArray = (csvData_t **) malloc(sizeof(csvData_t*)*INITIAL_PTR_ARRAY_SIZE);
    assert(csvPtrArray);

	// Allocate INITIAL_PTR_ARRAY_SIZE csv structs and have array point to them
    for(i=0; i<INITIAL_PTR_ARRAY_SIZE; i++) {
        csvPtrArray[i] = (csvData_t *) malloc(sizeof(csvData_t));
        assert(csvPtrArray[i]);
    }

	// Read the header of the first file
    char tempCsvHeader[MAXLINE];
    fgets(tempCsvHeader, MAXLINE, fp[CSV_DATA_FILE]);

    int csvArraySize = INITIAL_PTR_ARRAY_SIZE;
    int *csvArraySizePtr = &csvArraySize;
    int csvDataLines = 0;
	// Take CSV file, pointer array, and the current size and fill up the data structs with data from 
    // the file
	readData(fp[CSV_DATA_FILE], &csvPtrArray, csvArraySizePtr, &csvDataLines);

    // Close reading CSV file
    fclose(fp[CSV_DATA_FILE]);
    fp[CSV_DATA_FILE] = NULL;

	/*Part 2 - Create the Initial Polygon*/
    // Initialize vertex variables
    int vertexArraySize = INITIAL_NUM_VERTICES;
    int *vertexArraySizePtr = &vertexArraySize; // keeps track of the size of the array itself
    int vertexCount = 0;        // counts the actual number of vertices and edges read in.
    int *vertexCountPtr = &vertexCount;

    // Initialize edge variables
    int edgeArraySize;
    int *edgeArraySizePtr = &edgeArraySize;
    int edgeCount;
    int *edgeCountPtr = &edgeCount;

    // Initalize face variables
    int faceArraySize = INITIAL_NUM_VERTICES;
    int *faceArraySizePtr = &faceArraySize;
    int faceCount = 1;
    int *faceCountPtr = &faceCount;

    // Create vertex array
    vertex_t *vertexArray = (vertex_t *) malloc(INITIAL_NUM_VERTICES*sizeof(vertex_t));
    assert(vertexArray);

    // Read vertex coordinates from file into array
    readVertices(fp[INITIAL_POLY_FILE], &vertexArray, vertexArraySizePtr, vertexCountPtr);

    // Close reading inital polygon file
    fclose(fp[INITIAL_POLY_FILE]);
    fp[INITIAL_POLY_FILE] = NULL;

    // Set up edge variables
    edgeArraySize = vertexArraySize;
    edgeCount = vertexCount;        // Initially, the num vertice = num edges

    // Create edge array
    edge_t *edgeArray = (edge_t *) malloc(edgeArraySize*sizeof(edge_t));
    assert(edgeArray);

    // Create face array
    face_t *faceArray = (face_t *) malloc(faceArraySize*sizeof(face_t));
    assert(faceArray);

    // Create DCEL
    dcel_t dcel = makeDCEL(&edgeArray, edgeCount, &faceArray);

    /* start splitting. From terminal or from a split input file, input the two edges
    that you want to be split in the format of "edgeIndex1 edgeIndex2\n" */
    int splitStartIndex, splitEndIndex;
    while(scanf("%d %d\n", &splitStartIndex, &splitEndIndex)!=EOF){
        split(&dcel, &vertexArray, &faceArray, edgeArraySizePtr, edgeCountPtr, vertexArraySizePtr, 
            vertexCountPtr, faceArraySizePtr, faceCountPtr, splitStartIndex, splitEndIndex);
    }
    edgeArray = dcel;

    // // /* Part 4 Finding Watchtowers in Faces */
    // Malloc watchtower data array for all faces
    initalizeFaceWatchtower(&faceArray, faceCount);
    sortWatchtowerIntoFaces(&csvPtrArray, &vertexArray, &faceArray,
    csvDataLines, faceCount);

    // Print out the faces and watchtowers that belong into it into output file
    for(i=0; i<faceCount; i++) {
        fprintf(fp[OUTPUT_FILE], "%d\n", i);
        for(j=0; j<faceArray[i].numWatchtower; j++) {
            fprintf(fp[OUTPUT_FILE], "Watchtower ID: %s, Postcode: %s, Population Served: %d, Watchtower Point of Contact Name: %s, x: %lf, y: %lf\n", 
            (faceArray)[i].watchtowerArr[j]->watchID, faceArray[i].watchtowerArr[j]->postcode, 
            faceArray[i].watchtowerArr[j]->popServed, faceArray[i].watchtowerArr[j]->watchContactName, 
            faceArray[i].watchtowerArr[j]->xCoord, faceArray[i].watchtowerArr[j]->yCoord);
        }
    }
    for(i=0; i<faceCount; i++) {
        fprintf(fp[OUTPUT_FILE], "Face %d population served: %d\n", i, faceArray[i].population);
    }

    /* Clean up! */
    // Close reading output file
    fclose(fp[OUTPUT_FILE]);
    fp[OUTPUT_FILE] = NULL;

    // Free everything related to the DCEL sans the faces.
    freeDCEL(&dcel, &vertexArray, &edgeArray, edgeCount);

    // Free csvPtrArray
    freeCSVPtrArray(&csvPtrArray, csvDataLines, csvArraySize);
    free(csvPtrArray);
    csvPtrArray = NULL;

    // Free the remaining watchtower arrays within the faces and the face array itself.
    for(i=0; i<faceCount; i++) {
        for(j=faceArray[i].numWatchtower; j<faceArray[i].watchArrSize; j++) {
            free(faceArray[i].watchtowerArr[j]);
        }
        free(faceArray[i].watchtowerArr);
    }
    free(faceArray);
    faceArray = NULL;
}
