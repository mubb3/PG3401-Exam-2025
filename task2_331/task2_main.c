#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "task2_hash.h"
#include "task2_count.h"
#include "task2_sum.h"


#pragma pack(1)
struct TASK2_FILE_METADATA{
    char szFileName[32];
    int iFileSize;
    char byHash[4];
    int iSumOfChars;
    char aAlphaCount[26];
} TASK2_FILE_METADATA;
#pragma pack()

/*
* This origram reads metadata from the specified text file and computes filesize, sum of characters
* a hash using a DJB2 Algorhitm and the number of occurrences of characters. The metadata is stored in a packed struct and written to a binary file
*
*/


int main(int argc, char *argv[]){
    const char *inName = "pgexam25_test.txt";
    const char *outName = "pgexam25_output.bin";
    
    
    struct TASK2_FILE_METADATA file1 = {0};
    strncpy(file1.szFileName, inName, sizeof file1.szFileName -1);
    
    
    FILE *inFp = fopen(inName,"r");
    if(!inFp){
        perror("fopen");
        return 1;
    }
    
    if(Task2_SizeAndSumOfCharacters(inFp, &file1.iFileSize, &file1.iSumOfChars) != 0){
        fprintf(stderr, "Error in SizeAndSumOfCharacters\n");
        fclose(inFp);
        exit(1);
    }


    rewind(inFp);
    {unsigned int hash32 =0;
    if(Task2_SimpleDjb2Hash(inFp, &hash32) != 0){
        fprintf(stderr, "Error in SimpleDjb2Hash\n");
        fclose(inFp);
        return 1;
    }
    memcpy(file1.byHash, &hash32, sizeof(hash32));
    }
    
    rewind(inFp);
    if(Task2_CountEachCharacter(inFp, file1.aAlphaCount) != 0){
    fprintf(stderr,"Error in CountingEachCharacter\n");
    fclose(inFp);
    return 1;
    }
    
    fclose(inFp);
    
  

    FILE * outFp = fopen(outName, "wb");
    if(!outFp){
         fprintf(stderr, "Error opening file");
         return 1;
    }
    if(fwrite(&file1, sizeof(file1), 1, outFp) != 1){
        fprintf(stderr, "Error writing to %s\n", outName);
        fclose(outFp);
        return 1;
    }
    fclose(outFp);
    puts("metadata written successfully");
    return 0;
    
}


