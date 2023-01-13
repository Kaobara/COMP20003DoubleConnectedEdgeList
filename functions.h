#include "structs.h"

/* Used for Reading in the watchtower data to figure out which column of the CSV is being read.
If the character is a comma, it returns 1 (true). Otherwise, it will return 0 */
int checkComma(char ch, int counter);

/* Realloc functions for different arrays used in this program, including array of watchtower data, array of vertices, array of edges, and an array of faces. Also include arrays used to free 
the watchtower data array as you have to free every pointer within the array and the array 
itself. */
int increaseDataArray(int currentSize, csvData_t ***dataArray);
int increaseVertexArray(int currenSize, vertex_t **verticeArray);
int increaseEdgeArray(int currentSize, edge_t **edgeArray);
int increaseFaceArray(int currentSize, face_t **faceArray);
void freeCSVPtrArray(csvData_t ***csvPtrArray, int itemNumber, int arraySize);

// Calculate the gradient of a line with contains 2 points.
double calcGrad(coordinate_t point1, coordinate_t point2);

// Calculate the C-intercept of a line using a point in the line and the gradient
double calcInter(coordinate_t point, double grad);

// Takes in 2 coordinates, and calculates the coordinate of the point in the middle of the two
coordinate_t calcMidPoint(coordinate_t startPoint, coordinate_t endPoint);
