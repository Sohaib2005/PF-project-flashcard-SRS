#include<stdio.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>

#define DB_FILENAME "flashcards.dat"

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
typedef struct
{
    int count;
    Flashcard* cards;
} Flashcards;
int j;

void addFlashcard(Flashcard** cards, int* count);
void editFlashcard(Flashcard* cards, int count);
void deleteFlashcard(Flashcard** cards, int* count);
Flashcards loadFlashcardsFromFile(const char* filename);
int saveFlashcardsToFile(Flashcard* cards, int count, const char* filename);
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
    static int temp = 0;
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
    (*cards)[*count].id = ++temp;
    printf("Card added successfully!\n");
    store = temp;
    (*count)++;
}

void deleteFlashcard(Flashcard** cards, int* count)
{
    int id,i,correct;
    printf("\nEnter id of flash card you want to remove: ");
    scanf("%d",&id);
    correct=validate(id,count,cards);
    if(correct) {
        for(i=j;i<*count-1;i++)
            {
                (*cards)[i]=(*cards)[i+1];
            }
        (*count)--;
        *cards=realloc(*cards,(*count)*sizeof(Flashcard));
        printf("Flashcard deleted successfully!\n");
    } else {
        printf("ID does not exist!");
        return;
    }
}

void editFlashcard(Flashcard* cards, int count)
{
    int id, correct;
    printf("\nEnter id of flash card you want to edit: ");
    scanf("%d",&id);
    correct=validate(id,&count,&cards);
    if(correct) {
        printf("Question: %s\n",cards[j].question);
        printf("Answer : %s\n\n",cards[j].answer);
        getchar();
        printf("Edit Question: ");
        fgets(cards[j].question,256,stdin);
        cards[j].question[strcspn(cards[j].question, "\n")] = '\0';
        printf("Edit Answer: ");
        fgets(cards[j].answer,256,stdin);
        cards[j].answer[strcspn(cards[j].answer, "\n")] = '\0';
        printf("Flashcard edited successfully!\n");
    } else {
        printf("ID does not exist!\n");
        return;
    }
}

void studyFlashcards(Flashcard* cards, int count)
{
    long now = time(NULL);
    for (int i = 0; i < count; i++) {
        if (cards[i].nextReview <= now) {
            printf("\nQuestion: %s\n", cards[i].question);
            char userAnswer[256];
            printf("Your Answer: ");
            fgets(userAnswer, 256, stdin);
            userAnswer[strcspn(userAnswer, "\n")] = '\0';

            printf("Correct Answer: %s\n", cards[i].answer);
            printf("Rate your response (0-5): ");
            int quality;
            scanf("%d", &quality);
            getchar();

            if (quality >= 3) {
                if (cards[i].repetitions == 0) {
                    cards[i].interval = 1;
                } else if (cards[i].repetitions == 1) {
                    cards[i].interval = 6;
                } else {
                    cards[i].interval *= cards[i].easinessFactor;
                }
                cards[i].repetitions++;
            } else {
                cards[i].repetitions = 0;
                cards[i].interval = 1;
            }

            cards[i].easinessFactor += (0.1 - (5 - quality) * (0.08 + (5 - quality) * 0.02));
            if (cards[i].easinessFactor < 1.3) cards[i].easinessFactor = 1.3;

            cards[i].nextReview = now + cards[i].interval * 24 * 3600;
        }
    }
}

int saveFlashcardsToFile(Flashcard* cards, int count, const char* filename) {
    FILE* file_handle = fopen(filename, "wb");
    if (file_handle == NULL) {
        printf("[ERROR] Could not open file '%s'. Do you have the correct permissions?", filename);
        return -1;
    }
    // Simple format: Count of cards, then all of the flashcards
    fwrite(&count, sizeof(int), 1, file_handle);
    if (cards != NULL) {
        fwrite(cards, sizeof(Flashcard), count, file_handle);
    }
    return 0;
}

// NOTE: Can return a null pointer on error!
Flashcards loadFlashcardsFromFile(const char* filename) {
    FILE* file_handle = fopen(filename, "r");
    Flashcards fcs = {
        .cards = NULL,
        .count = 0
    };
    if (file_handle == NULL) {
        printf("Could not find the database file '%s'.\n", filename);
        printf("If this is your first time running the application, press 1. Otherwise, check if you have read permissions.\n");
        printf("Create a fresh database? (1 = Yes, Anything else = No): ");
        int should_create_db = 0;
        scanf(" %d", &should_create_db);
        if (should_create_db != 1) {
            exit(-1);
        } else {
            saveFlashcardsToFile(fcs.cards, fcs.count, filename);
        }
    }
    // Check if the file has more than just an int (the count of flashchards),
    // meaning it probably has no actual cards
    fseek(file_handle, 0L, SEEK_END);
    int file_size = ftell(file_handle);
    fseek(file_handle, 0L, SEEK_SET);
    fread(&fcs.count, sizeof(int), 1, file_handle);
    printf("DB size: %d\n", file_size);
    if (file_size > sizeof(int)) {
        fread(fcs.cards, sizeof(Flashcard), fcs.count, file_handle);
    }
    return fcs;
}

void display(Flashcard* cards, int count)
{
    for (int i = 0; i < count; i++) {
        printf("\n%d. ", cards[i].id);
        printf("%s \n", cards[i].question);
        printf("   %s \n", cards[i].answer);
    }
}

int main() {
    Flashcards fcs = loadFlashcardsFromFile(DB_FILENAME);
    Flashcard* cards = fcs.cards;
    int count = fcs.count;
    int choice;

    if (cards == NULL) {
        return -1;
    }

    while (1) {
        printf("\nFlashcard System Menu:\n");
        printf("1. Add Flashcard\n");
        printf("2. Delete Flashcard\n");
        printf("3. Edit Flashcards\n");
        printf("4. Study Flashcards\n");
        printf("5. Display all flashcards\n");
        printf("Any other key to exit\n");
        printf("Enter your choice: ");
        scanf(" %d", &choice);
        if(choice<1 || choice>5) {
            printf("\nExiting program....");
            free(cards);
            exit(0);
        }
        getchar();  // To clear the newline character from input buffer
        int return_code = 0;
        switch (choice) {
        case 1:
            addFlashcard(&cards, &count);
            break;
        case 2:
            deleteFlashcard(&cards, &count);
            break;
        case 3:
            editFlashcard(cards,count);
            break;
        case 4:
            studyFlashcards(cards,count);
            break;
        case 5:
            display(cards,count);
            break;    
        case 6:
            return_code = saveFlashcardsToFile(cards, count, DB_FILENAME);
            free(cards);
            printf("Flashcards saved and exiting...\n");
            return 0;
        }
    }

    free(cards);
    return 0;
}
