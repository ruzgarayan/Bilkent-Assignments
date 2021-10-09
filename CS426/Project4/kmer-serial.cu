#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 
#include <time.h>
#include <math.h>
#include "util.h"
#include "hashtable.c"

//#define MAX_REF_LENGTH 1000010
//#define MAX_READ_LENGTH 210

//This includes enough information to hold the best solutions so far
//to be later outputted in the form "<k-mer>, <index on reference>, <extension score>"
typedef struct {
	int indexOnRead;
	int indexOnReference;
	int extensionScore;
} Solution;

/*
 * ##################################################################################################
 * To store the hit locations for a single kmer, 
 * I have minimally implemented a resizing list.
 * (When k is low like 2-3, there will be a lot of hits, so I didn't want to use a fixed sized array)
*/

#define ARRAY_DEFAULT_SIZE 10
typedef struct {
	int *nums;
	int size;
	int maxSize;
} list;

list* createList() {
	list *l = (list *) malloc(sizeof(list));
	l->size = 0;
	l->maxSize = ARRAY_DEFAULT_SIZE;
	l->nums = (int *) malloc(l->maxSize * sizeof(int));

	return l;
}

void listAdd(list* l, int num) {
	if (l->size == l->maxSize) {
		l->maxSize *= 2;
		l->nums = (int *) realloc(l->nums, l->maxSize * sizeof(int));
	}

	l->nums[l->size++] = num; 
}
//##################################################################################################

//Hash Table to not search for already searched kmers
hashtable_t *ht;

//Bruteforce searching and extending, not used in the code right now.
Solution bruteForceKmerSearch(char *referenceString, int referenceLength, char *readString,
 int readLength, int k, int indexOnRead, list *hitLocs) {
	char *kmer = readString + indexOnRead;
	int isHit;

	Solution bestKmerHit;
	bestKmerHit.extensionScore = -1;

	//Iterate over all the possible starting indices on the reference string to check a hit
	for (int i = 0; i <= referenceLength - k; i++) {
		isHit = 1;
		for (int j = 0; j < k; j++) {
			if (referenceString[i + j] != kmer[j]) {
				isHit = 0;
				break; 
			}
		}

		if (isHit) {
			int extendedReferenceStart = i - indexOnRead;
			int extendedReferenceEnd = extendedReferenceStart + readLength;
			if (extendedReferenceEnd > referenceLength) extendedReferenceEnd = referenceLength;

			int extensionScore = 0;

			//Extend the original read string and count the equal characters for the extension score.
			for (int j = 0; j < readLength && extendedReferenceStart + j < extendedReferenceEnd; j++) {
				if (extendedReferenceStart + j >= 0 && referenceString[extendedReferenceStart + j] == readString[j]) {
					extensionScore++;
				}
			}

			//Add to the hit locations
			listAdd(hitLocs, i);

			//Check if this new hit is better than the current best. In that case, change the current best
			if (extensionScore > bestKmerHit.extensionScore 
				|| (extensionScore == bestKmerHit.extensionScore && i < bestKmerHit.indexOnReference) ) {
				bestKmerHit.extensionScore = extensionScore;
				bestKmerHit.indexOnReference = i;
				bestKmerHit.indexOnRead = indexOnRead;
			}
			//printf("Hit at reference index %d, extension score is %d\n", i, extensionScore);
		}
	}

	return bestKmerHit;
}

//KMP searching and extending
Solution KMPKmerSearch(char *referenceString, int referenceLength, char *readString,
 int readLength, int k, int indexOnRead, list *hitLocs) {
	char *kmer = readString + indexOnRead;

	int kmpTable[MAX_READ_LENGTH + 1];
	kmpTable[0] = -1;

	Solution bestKmerHit;
	bestKmerHit.extensionScore = -1;

	//Create kmp state transition table
	int cnd = 0, pos = 1;
	while (pos < k) {
		if (kmer[pos] == kmer[cnd])
			kmpTable[pos] = kmpTable[cnd];
		else {
			kmpTable[pos] = cnd;
			while (cnd >= 0 && kmer[pos] != kmer[cnd]) {
				cnd = kmpTable[cnd];
			}
		}
		pos++;
		cnd++;
	}
	kmpTable[pos] = cnd;

	int kmerPos = 0, refPos = 0;
	while (refPos < referenceLength) {
		if (referenceString[refPos] == kmer[kmerPos]) {
			kmerPos++;
			refPos++;

			//Hit is found
			if (kmerPos == k) {
				int hitLocation = refPos - kmerPos;
				int extendedReferenceStart = hitLocation - indexOnRead;
				int extendedReferenceEnd = extendedReferenceStart + readLength;
				if (extendedReferenceEnd > referenceLength) extendedReferenceEnd = referenceLength;

				int extensionScore = 0;

				//Extend the original read string and count the equal characters for the extension score.
				for (int i = 0; i < readLength && extendedReferenceStart + i < extendedReferenceEnd; i++) {
					if (extendedReferenceStart + i >= 0 && referenceString[extendedReferenceStart + i] == readString[i]) {
						extensionScore++;
					}
				}

				//Add to the hit locations
				listAdd(hitLocs, hitLocation);

				//Check if this new hit is better than the current best. In that case, change the current best
				if (extensionScore > bestKmerHit.extensionScore 
				|| (extensionScore == bestKmerHit.extensionScore && hitLocation < bestKmerHit.indexOnReference) ) {
					bestKmerHit.extensionScore = extensionScore;
					bestKmerHit.indexOnReference = hitLocation;
					bestKmerHit.indexOnRead = indexOnRead;
				}
				//printf("Hit at reference index %d, extension score is %d\n", hitLocation, extensionScore);

				kmerPos = kmpTable[kmerPos];
			}
		} else {
			kmerPos = kmpTable[kmerPos];
			if (kmerPos < 0) {
				kmerPos++;
				refPos++;
			}
		}
	}

	return bestKmerHit;
}

//Use the already found and hashed hit locations (skip the searching operation),
//Just extend the hits and find the extension scores  
Solution justExtend(char *referenceString, int referenceLength, char *readString,
 int readLength, int k, int indexOnRead, list *hitLocs) {
	Solution bestKmerHit;
	bestKmerHit.extensionScore = -1;

	int numHits = hitLocs->size;
	int *hits = hitLocs->nums;
	//Go over all the hit locations
	for (int i = 0; i < numHits; i++) {
		int hitLocation = hits[i];
		int extendedReferenceStart = hitLocation - indexOnRead;
		int extendedReferenceEnd = extendedReferenceStart + readLength;
		if (extendedReferenceEnd > referenceLength) extendedReferenceEnd = referenceLength;

		int extensionScore = 0;

		for (int i = 0; i < readLength && extendedReferenceStart + i < extendedReferenceEnd; i++) {
			if (extendedReferenceStart + i >= 0 && referenceString[extendedReferenceStart + i] == readString[i]) {
				extensionScore++;
			}
		}

		if (extensionScore > bestKmerHit.extensionScore 
		|| (extensionScore == bestKmerHit.extensionScore && hitLocation < bestKmerHit.indexOnReference) ) {
			bestKmerHit.extensionScore = extensionScore;
			bestKmerHit.indexOnReference = hitLocation;
			bestKmerHit.indexOnRead = indexOnRead;
		}
	}

	return bestKmerHit;
}


/*
Solution searchAndExtendSingleReadNoHashTable(char *referenceString, int referenceLength, char *readString, int readLength, int k) {
    Solution bestHit;
	bestHit.extensionScore = -1; 

	Solution bestKmerHit;
	char kmer[MAX_READ_LENGTH];
	for (int i = 0; i <= readLength - k; i++) {

		for (int j = 0; j < k; j++)
			kmer[j] = readString[i + j];
		kmer[k] = '\0';

		list *hitLocs = createList();
		bestKmerHit = bruteForceKmerSearch(referenceString, referenceLength, readString, readLength, k, i, hitLocs);

		if (bestKmerHit.extensionScore > bestHit.extensionScore 
				|| (bestKmerHit.extensionScore == bestHit.extensionScore && bestKmerHit.indexOnReference < bestHit.indexOnReference) ) {
			bestHit.extensionScore = bestKmerHit.extensionScore;
			bestHit.indexOnReference = bestKmerHit.indexOnReference;
			bestHit.indexOnRead = bestKmerHit.indexOnRead;
		}
	}

	return bestHit;
}
*/

Solution searchAndExtendSingleRead(char *referenceString, int referenceLength, char *readString, int readLength, int k) {
    Solution bestHit;
	bestHit.extensionScore = -1; 

	Solution bestKmerHit;
	char kmer[MAX_READ_LENGTH];
	for (int i = 0; i <= readLength - k; i++) {

		for (int j = 0; j < k; j++)
			kmer[j] = readString[i + j];
		kmer[k] = '\0';

		//Check if this kmer has already been searched in the reference string
		if(ht_get(ht, kmer)) {
			list *hitLocs = (list *) ht_get(ht, kmer);
			//printf("Hit locs count %d\n", hitLocs->size);
			bestKmerHit = justExtend(referenceString, referenceLength, readString, readLength, k, i, hitLocs);
		} else {
			list *hitLocs = createList();
			bestKmerHit = bruteForceKmerSearch(referenceString, referenceLength, readString, readLength, k, i, hitLocs);
			ht_put(ht, kmer, hitLocs);
		}

		if (bestKmerHit.extensionScore > bestHit.extensionScore 
				|| (bestKmerHit.extensionScore == bestHit.extensionScore && bestKmerHit.indexOnReference < bestHit.indexOnReference) ) {
			bestHit.extensionScore = bestKmerHit.extensionScore;
			bestHit.indexOnReference = bestKmerHit.indexOnReference;
			bestHit.indexOnRead = bestKmerHit.indexOnRead;
		}
	}

	return bestHit;
}

int main(int argc, char **argv) 
{ 
	if(argc != 5) {
        printf("Wrong arguments usage: ./kmer [REFERENCE_FILE] [READ_FILE] [k] [OUTPUT_FILE]\n" );
        return 1;
    }
	//struct timeval start, end;

	char *referenceStringFile = argv[1];
	char *readStringsFile = argv[2];
	int k = atoi(argv[3]);
	char *outputFile = argv[4];

    char *referenceString = (char*) malloc(MAX_REF_LENGTH * sizeof(char));;

	FILE* file;
	file = fopen(referenceStringFile, "r");
    if (file == 0) {
        printf("Cannot open the reference file %s, exiting the program.\n", referenceStringFile);
        return 1; 
    }
	fgets(referenceString, MAX_REF_LENGTH, file);
	fclose(file);

	StringList readStringList;

	initStringList(&readStringList, 20); 
	read_file(readStringsFile, &readStringList);

	int numReadStrings = readStringList.used;
	int referenceLength = strlen(referenceString);
    int readLength = strlen(readStringList.array[0]);

	Solution *allBestHits = (Solution *) malloc(numReadStrings * sizeof(Solution));

	ht = ht_create(1024);
	//gettimeofday(&start, NULL);
	for (int i = 0; i < numReadStrings; i++) {
		char *currReadString = readStringList.array[i];
		Solution bestHit = searchAndExtendSingleRead(referenceString, referenceLength, currReadString, readLength, k);

		allBestHits[i].extensionScore = bestHit.extensionScore;
		allBestHits[i].indexOnRead = bestHit.indexOnRead;
		allBestHits[i].indexOnReference = bestHit.indexOnReference;
	}
	//gettimeofday(&end, NULL);

	file = fopen(outputFile, "w");
    if (file == 0) {
        printf("Cannot open the output file %s, exiting the program.\n", outputFile);
        return 1; 
    }
    
	char kmer[MAX_READ_LENGTH];
    for (int i = 0; i < numReadStrings; i++) {
		char *currReadString = readStringList.array[i];
		Solution bestHit = allBestHits[i];
		if (bestHit.extensionScore < 0) {
            fprintf(file, "No hit has been found for read string %d\n", i + 1);
            continue;
		}

		for (int j = 0; j < k; j++)
			kmer[j] = currReadString[j + bestHit.indexOnRead];
		kmer[k] = '\0';

        fprintf(file, "%s, %d, %d\n", kmer, bestHit.indexOnReference, bestHit.extensionScore);
	}

	fclose(file);

	//double elapsedTime = (end.tv_sec - start.tv_sec) +
	//(end.tv_usec - start.tv_usec) / 1000000.0;

	//printf("Elapsed time: %.10f ms\n", elapsedTime * 1000);

    freeStringList(&readStringList);
	free(allBestHits);
    free(referenceString);
	return 0;
}

