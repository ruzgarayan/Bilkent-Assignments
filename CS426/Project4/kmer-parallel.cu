#include <util.h>

//This includes enough information to hold the best solutions so far
//to be later outputted in the form "<k-mer>, <index on reference>, <extension score>"
typedef struct {
    int indexOnRead;
    int indexOnReference;
    int extensionScore;
} Solution;

__global__ void kmerSearchAndExtend(char *d_ref, char *d_reads, Solution *d_solutions, int k,
                                    int referenceLength, int readLength) {
    //printf("%d %d\n", blockIdx.x, threadIdx.x);
    int indexOnRead = threadIdx.x;
    char *readString = d_reads + blockIdx.x * readLength;
    char *kmer = readString + indexOnRead;
    
    int isHit;

    Solution bestKmerHit;
    bestKmerHit.extensionScore = -1;

    //Iterate over all the possible starting indices on the reference string to check a hit
    for (int i = 0; i < referenceLength - k; i++) {
        isHit = 1;
        for (int j = 0; j < k; j++) {
            if (d_ref[i + j] != kmer[j]) {
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
                if (extendedReferenceStart + j >= 0 && d_ref[extendedReferenceStart + j] == readString[j]) {
                    extensionScore++;
                }
            }

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
    d_solutions[blockIdx.x * blockDim.x + threadIdx.x] = bestKmerHit;
}

int main(int argc, char** argv)
{
    if(argc != 5) {
        printf("Wrong argments usage: ./kmer [REFERENCE_FILE] [READ_FILE] [k] [OUTPUT_FILE]\n" );
        return 1;
    }

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

    if (fgets(referenceString, MAX_REF_LENGTH, file) == NULL) { //A single line only
        printf("Problem in file format!\n");
        return 1;
    }
    fclose(file);

    StringList readStringList;

    initStringList(&readStringList, 10);  // initially 5 elements
    int successReadFile = read_file(readStringsFile, &readStringList);

    if (successReadFile < 0) {
        printf("Cannot read the reads file %s, exiting the program!\n", readStringsFile);
        return 1;
    }

    int numReadStrings = readStringList.used;
    int referenceLength = strlen(referenceString);
    int readLength = strlen(readStringList.array[0]);


    dim3 DimGrid(numReadStrings); // 1 block for every read string
    int threadsPerBlock = readLength - k + 1;
    dim3 DimBlock(threadsPerBlock); // 1 thread for every kmer of the read string of that block

    //Flatten the read strings into a single string
    char *flattenedReads = (char*) malloc(readLength * numReadStrings * sizeof(char));;
    for (int i = 0; i < numReadStrings; i++) {
        memcpy(flattenedReads + i * readLength, readStringList.array[i], readLength);
    }

    //Declare pointers to device memory
    char *d_ref;
    char *d_reads;
    Solution *d_solutions;

    cudaMalloc((void **) &d_ref, referenceLength * sizeof(char));
    cudaMalloc((void **) &d_reads, readLength * numReadStrings * sizeof(char));
    cudaMalloc((void **) &d_solutions, threadsPerBlock * numReadStrings * sizeof(Solution));

    cudaMemcpy(d_ref, referenceString, referenceLength * sizeof(char), cudaMemcpyHostToDevice);
    cudaMemcpy(d_reads, flattenedReads, readLength * numReadStrings * sizeof(char), cudaMemcpyHostToDevice);

    kmerSearchAndExtend<<<DimGrid, DimBlock>>>(d_ref, d_reads, d_solutions, k, referenceLength, readLength);
    cudaDeviceSynchronize();

    Solution *h_solutions = (Solution *) malloc(threadsPerBlock * numReadStrings * sizeof(Solution));
    cudaMemcpy(h_solutions, d_solutions, threadsPerBlock * numReadStrings * sizeof(Solution), cudaMemcpyDeviceToHost);

    Solution *allBestHits = (Solution *) malloc(numReadStrings * sizeof(Solution));

    for (int i = 0; i < numReadStrings; i++) {
        Solution bestSoFar;
        bestSoFar.extensionScore = -1;
        for (int j = 0; j < threadsPerBlock; j++) {
            Solution currSolution = h_solutions[i * threadsPerBlock + j];
            
            if (currSolution.extensionScore > bestSoFar.extensionScore 
                || (currSolution.extensionScore == bestSoFar.extensionScore && currSolution.indexOnReference < bestSoFar.indexOnReference) ) {
                bestSoFar.extensionScore = currSolution.extensionScore;
                bestSoFar.indexOnReference = currSolution.indexOnReference;
                bestSoFar.indexOnRead = currSolution.indexOnRead;
            }
        }

        allBestHits[i] = bestSoFar;
    }

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

    free(allBestHits);
    free(h_solutions);
    freeStringList(&readStringList);
    free(flattenedReads);
    free(referenceString);
    return 0;
}