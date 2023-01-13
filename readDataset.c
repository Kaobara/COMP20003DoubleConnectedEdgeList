#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "functions.h"

#define DATA_STR_SIZE 512

// The following reads in the data from the file that has the CSV data, and inputs them into the array of pointers to data structs
void readData(FILE *csvDataFile, csvData_t ***csvPtrArray, int *csvArraySizePtr, int *csvDataLines) {
    int i, commaCounter = 0, currentLine = 0, stringCounter = 0;
    char currentChar, tempStr[DATA_STR_SIZE];
    int currentArraySize = *csvArraySizePtr;	

	while((currentChar = fgetc(csvDataFile)) != EOF){
        // If current array size is too small, incrase it
		if(currentLine == currentArraySize) {
			currentArraySize = increaseDataArray(currentArraySize, csvPtrArray);
		}

        // Read in watchID from first column per row
		while(commaCounter == 0) {
            // Store characters into temp string up until comma has been reached to signify end of string
			tempStr[stringCounter] = currentChar;
            stringCounter ++;
			currentChar = fgetc(csvDataFile);   //Reads next char for watchID from CSV
			commaCounter = checkComma(currentChar, commaCounter);
			if(commaCounter != 0) {
                // Copy the string from tempStr into the data struct
				tempStr[stringCounter] = '\0';
				stringCounter=0;
				(*csvPtrArray)[currentLine]->watchID = malloc ((strlen(tempStr)+1)*sizeof(char));
                assert((*csvPtrArray)[currentLine]->watchID);
				strcpy((*csvPtrArray)[currentLine]->watchID, tempStr);
				break;
			}
		}

        // Read postcode string and store into data struct
		while(commaCounter == 1) {
			currentChar = fgetc(csvDataFile);   //Reads next char for watchID from CSV
			commaCounter = checkComma(currentChar, commaCounter);
			if(commaCounter != 1) {
				tempStr[stringCounter] = '\0';
				stringCounter=0;
				(*csvPtrArray)[currentLine]->postcode = malloc ((strlen(tempStr)+1)*sizeof(char));
                assert((*csvPtrArray)[currentLine]->postcode);
				strcpy((*csvPtrArray)[currentLine]->postcode, tempStr);
				break;
			}
			tempStr[stringCounter] = currentChar;
            stringCounter ++;
		}
		
        // Reads the popServed from the CSV file and places them into corresponding struct
        if(commaCounter == 2) {
            fscanf(csvDataFile, "%d", &(*csvPtrArray)[currentLine]->popServed);
            currentChar = fgetc(csvDataFile);   //Reads watchID from CSV
            commaCounter = checkComma(currentChar, commaCounter);
        }

        // Read contact name and store into data struct
        while(commaCounter == 3) {
			currentChar = fgetc(csvDataFile);   
			commaCounter = checkComma(currentChar, commaCounter);
			if(commaCounter != 3) {
				tempStr[stringCounter] = '\0';
				stringCounter=0;
				(*csvPtrArray)[currentLine]->watchContactName = malloc ((strlen(tempStr)+1)*sizeof(char));
                assert((*csvPtrArray)[currentLine]->watchContactName);
				strcpy((*csvPtrArray)[currentLine]->watchContactName, tempStr);
				break;
			}
			tempStr[stringCounter] = currentChar;
            stringCounter ++;
		}

        // Reads in the x and y coordinates and automatically store them into data struct
        if(commaCounter == 4) {
            fscanf(csvDataFile, "%lf,%lf", &(*csvPtrArray)[currentLine]->xCoord, &(*csvPtrArray)[currentLine]->yCoord);
            currentChar = fgetc(csvDataFile);
        }

        // Checks if reach end of line, and moves to next line
        if(currentChar == '\n'){
			commaCounter = 0;
			currentLine += 1;
            continue;
        }
	}

	*csvArraySizePtr = currentArraySize;
	*csvDataLines = currentLine;
}
