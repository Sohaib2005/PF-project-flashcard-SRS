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
int store=0;

int validate(int id, int *count, Flashcard** cards) {
    for (j = 0; j < *count; j++) {
        if (id == (*cards)[j].id) {
            return 1; // ID is valid
        }
    }
    printf("Invalid ID!\n");
    return 0; // ID not found
}
void addFlashcard(Flashcard** cards, int* count)
{
    static int temp=0;
    *cards = realloc(*cards, (*count + 1) * sizeof(Flashcard));
    printf("\nEnter Question: ");
    fgets((*cards)[*count].question,256,stdin);
    (*cards)[*count].question[strcspn((*cards)[*count].question, "\n")] = '\0';
    printf("Enter Answer: ");
    fgets((*cards)[*count].answer,256,stdin);
    (*cards)[*count].answer[strcspn((*cards)[*count].answer, "\n")] = '\0';
    (*cards)[*count].easinessFactor = 2.5;
    (*cards)[*count].interval = 0;
    (*cards)[*count].repetitions = 0;
    (*cards)[*count].nextReview = time(NULL);
    (*cards)[*count].id=++temp;
    printf("Card added successfully!\n");
    store=temp;
    (*count)++;
}
void deleteFlashcard(Flashcard** cards, int* count)
{
    int id,i,correct;
    printf("\nEnter id of flash card you want to remove: ");
    scanf("%d",&id);
    correct=validate(id,count,cards);
    if(correct)
    {
    for(i=j;i<*count-1;i++)
    {
        (*cards)[i]=(*cards)[i+1];
    }
    (*count)--;
    *cards=realloc(*cards,(*count)*sizeof(Flashcard));
    printf("Flashcard deleted successfully!\n");
    }
    else
    {
        printf("ID does not exist!");
        return;
    }
}
