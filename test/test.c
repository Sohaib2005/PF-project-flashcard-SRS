#include<stdio.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>
typedef struct 
{
    int id;
    char question[256];
    char answer[256];
    float easinessFactor;
    int interval;
    int repetitions;
    long nextReview; // Unix timestamp for the next review date
} Flashcard;
int j;
// Function Prototypes
void addFlashcard(Flashcard** cards, int* count);
void editFlashcard(Flashcard* cards, int count);
void deleteFlashcard(Flashcard** cards, int* count);
Flashcard* loadFlashcardsFromFile(int* count, const char* filename);
void saveFlashcardsToFile(Flashcard* cards, int count, const char* filename);
void studyFlashcards(Flashcard* cards, int count);
