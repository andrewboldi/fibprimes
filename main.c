#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#define size 26212
#define vel 0x32

int g, h, j, x, o;
FILE *rawNotesFile;
FILE *rawMIDIFile;
// char length[2]; length[0] = size >> 8; length[1] = size %;
bool primes[size];
char out;
char MIDIarr[4];

char fibNotes[16] = {36, 36, 38, 40, 43, 36, 45, 47, 45, 45, 43, 41, 38, 45, 36, 47};
char fibChunk[4] = {0x04, 0x90, 0x00, vel};
char headerChunk[14] = {0x4D, 0x54, 0x68, 0x64, 0x00, 0x00, 0x00, 0x06, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01};
char trackChunkHeader[8] = {0x4D, 0x54, 0x72, 0x6B, 0x00, 0x00, 0xFF, 0xFE}; // Remember to change trackChunkHeader[6, 7] to length of track
char noteOn[4] = {0x00, 0x90, 0x00, vel}; // Remember to change noteOn[2] to actual note
char noteOff[4] = {0x00, 0x80, 0x00, vel}; // Remember to change noteOff[2] to actual note
char stopChunk[4] = {0x02, 0xFF, 0x2F, 0x00};


#define forArr(arr) for(int x = 0; x < sizeof(arr)/sizeof(arr[0]); x++) {fprintf(rawMIDIFile, "%c", arr[x]);}



void writeMIDIfile(char string[4]) {
    char notes[4];
    for (int u = 0; u < 4; u++) {
        switch(string[u]) {
            case 'A':
                notes[u] = 57;
                break;

            case 'B':
                notes[u] = 59;
                break;

            case 'C':
                notes[u] = 48;
                break;

            case 'D':
                notes[u] = 50;
                break;

            case 'E':
                notes[u] = 52;
                 break; 

            case 'F':
                notes[u] = 53;
                break;

            case 'G':
                notes[u] = 55;
                break;

        }
    }

    for (int q = 0; q < 4; q++) {
        if (notes[q] >= notes[q + 1]) {
            notes[q + 1] += 12;
        }
        if (q == 0) {
            fibChunk[0] = 0x00;
            fibChunk[1] = 0x90;
            printf("%d\n", fibNotes[o % 16]);
            fibChunk[2] = fibNotes[o % 16];
            forArr(fibChunk);
        }
        noteOn[2] = notes[q];
        // noteOn[2] = noteOff[2] = notes[q];
        forArr(noteOn);
//        forArr(noteOff);
    } 

    for (int s = 0; s < 4; s++) {
        if (s == 0) {
            fibChunk[0] = 0x04;
            fibChunk[1] = 0x80;
            forArr(fibChunk);
        } 

        if (notes[s] >= notes[s + 1]) {
            notes[s + 1] += 12;
        }
        noteOff[2] = notes[s];
        forArr(noteOff);
    }

    o++;

}

void calculatePrimes() {
//    primes = (bool *) malloc(size * sizeof(bool));
    
    printf("beginning allocation...\n");
    for (int y = 0; y < size; y++) primes[y] = true;

    printf("Done allocating memory\n");

    printf("Starting prime function...\n");
    for (g = 0; g <= floor(sqrt(size)); g++) {
        if (primes[g] == false) continue;

        if (g <= 1) {
            primes[g] = false;
            continue;
        }

        for (h = g * g; h <= size; h += g) {
            if (primes[h] == false) continue;
            primes[h] = false;
        }
    }

}

void writeToFile() {
    printf("Beginning write to file...\n");

    int z = -1;
    for (j=0; j < sizeof(primes); j++) {
        if (primes[j] == true) {
            z++;
            switch (j % 7) {
                case 0:
                    out = 'B';
                    break;
                case 1:
                    out = 'C';
                    break;
                case 2:
                    out = 'D';
                    break;
                case 3:
                    out = 'E';
                    break;
                case 4:
                    out = 'F';
                    break;
                case 5:
                    out = 'G';
                    break;
                case 6:
                    out = 'A';
                    break;
            }

            MIDIarr[z % 4] = out;
            if (z % 4 == 3) {
                fprintf(rawNotesFile, "%c\n", out);
                writeMIDIfile(MIDIarr);
                continue;
            }
            fprintf(rawNotesFile, "%c ", out);
            
        }
    }
    printf("file write complete!\n\n");
}


int main () {
    o = 0;
    printf("starting program...\n");
    calculatePrimes();
    printf("Done calculating primes!\n\n");


    rawNotesFile = fopen("26212.txt", "w");
    rawMIDIFile = fopen("26212.MID", "w");
    forArr(headerChunk);
    forArr(trackChunkHeader);
    writeToFile();
    forArr(stopChunk);
    fclose(rawNotesFile);
    fclose(rawMIDIFile);
    return EXIT_SUCCESS;
}
