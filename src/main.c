#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<string.h>
#include<time.h>

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

int validate(Flashcards const* fcs, int id);
void addFlashcard(Flashcards* fcs);
void editFlashcard(Flashcards* fcs);
void deleteFlashcard(Flashcards* fcs);
Flashcards loadFlashcardsFromFile(const char* filename);
int saveFlashcardsToFile(Flashcards* fcs, const char* filename);
void studyFlashcards(Flashcards* fcs);
void display(Flashcards const* fcs);
int store = 0;

int validate(Flashcards const* fcs, int id) {
    for (j = 0; j < fcs->count; j++) {
        if (id == fcs->cards[j].id) {
            return 1; // ID is valid
        }
    }
    printf("Invalid ID!\n");
    return 0; // ID not found
}

void addFlashcard(Flashcards* fcs)
{
    static int temp = 0;
    fcs->cards = realloc(fcs->cards, (size_t) (fcs->count + 1) * sizeof(Flashcard));

    Flashcard new_card = {
        .easinessFactor = 2.5,
        .interval = 0,
        .repetitions = 0,
        .nextReview = time(NULL),
        .id = ++temp,
    };

    printf("\nEnter Question: ");
    fgets(new_card.question,256,stdin);
    size_t newline_char_index = strcspn(new_card.question, "\n");
    new_card.question[newline_char_index] = '\0';

    printf("Enter Answer: ");
    fgets(new_card.answer,256,stdin);
    newline_char_index = strcspn(new_card.answer, "\n");
    new_card.answer[newline_char_index] = '\0';
    
    fcs->cards[fcs->count] = new_card;
    printf("Card added successfully!\n");
    store = temp;
    fcs->count++;
}

void deleteFlashcard(Flashcards* fcs)
{
    int id, i, correct;
    printf("\nEnter id of flash card you want to remove: ");
    scanf("%d",&id);
    correct = validate(fcs,id);
    if(correct) {
        for(i = j; i < fcs->count - 1; i++)
            {
                fcs->cards[i] = fcs->cards[i+1];
            }
        fcs->count--;
        fcs->cards = realloc(fcs->cards,(size_t) fcs->count*sizeof(Flashcard));
        printf("Flashcard deleted successfully!\n");
    } else {
        printf("ID does not exist!");
        return;
    }
}

void editFlashcard(Flashcards* fcs)
{
    int id, correct;
    printf("\nEnter id of flash card you want to edit: ");
    scanf("%d", &id);
    correct = validate(fcs,id);
    if(correct) {
        printf("Question: %s\n",fcs->cards[j].question);
        printf("Answer : %s\n\n",fcs->cards[j].answer);
        getchar();
        printf("Edit Question: ");
        fgets(fcs->cards[j].question,256,stdin);
        fcs->cards[j].question[strcspn(fcs->cards[j].question, "\n")] = '\0';
        printf("Edit Answer: ");
        fgets(fcs->cards[j].answer,256,stdin);
        fcs->cards[j].answer[strcspn(fcs->cards[j].answer, "\n")] = '\0';
        printf("Flashcard edited successfully!\n");
    } else {
        printf("ID does not exist!\n");
        return;
    }
}

void studyFlashcards(Flashcards* fcs)
{
    long now = time(NULL);
    for (int i = 0; i < fcs->count; i++) {
        if (fcs->cards[i].nextReview <= now) {
            printf("\nQuestion: %s\n", fcs->cards[i].question);
            char userAnswer[256];
            printf("Your Answer: ");
            fgets(userAnswer, 256, stdin);
            userAnswer[strcspn(userAnswer, "\n")] = '\0';

            printf("Correct Answer: %s\n", fcs->cards[i].answer);
            printf("Rate your response (0-5): ");
            int quality;
            scanf("%d", &quality);
            getchar();

            if (quality >= 3) {
                if (fcs->cards[i].repetitions == 0) {
                    fcs->cards[i].interval = 1;
                } else if (fcs->cards[i].repetitions == 1) {
                    fcs->cards[i].interval = 6;
                } else {
                    fcs->cards[i].interval *= (int) fcs->cards[i].easinessFactor;
                }
                fcs->cards[i].repetitions++;
            } else {
                fcs->cards[i].repetitions = 0;
                fcs->cards[i].interval = 1;
            }

            fcs->cards[i].easinessFactor += (float) (0.1 - (5 - quality) * (0.08 + (5 - quality) * 0.02));
            if (fcs->cards[i].easinessFactor < 1.3f)
                fcs->cards[i].easinessFactor = 1.3f;

            fcs->cards[i].nextReview = now + fcs->cards[i].interval * 24 * 3600;
        }
    }
}

int saveFlashcardsToFile(Flashcards* fcs, const char* filename) {
    FILE* file_handle = fopen(filename, "wb");
    if (file_handle == NULL) {
        printf("[ERROR] Could not open file '%s'. Do you have the correct permissions?", filename);
        return -1;
    }
    // Simple format: Count of cards, then all of the flashcards
    printf("Saving flashcards...\n");
    fwrite(&fcs->count, sizeof(int), 1, file_handle);
    if (fcs->cards != NULL) {
        fwrite(fcs->cards, sizeof(Flashcard), (size_t) fcs->count, file_handle);
    }
    printf("Saved flashcards\n");
    fclose(file_handle);
    return 0;
}

// NOTE: Can return a null pointer on error!
Flashcards loadFlashcardsFromFile(const char* filename) {
    FILE* file_handle = fopen(filename, "rb");
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
            saveFlashcardsToFile(&fcs, filename);
            file_handle = fopen(filename, "rb");
        }
    }

    printf("Loading flashcards from DB...\n");
    if (fread(&fcs.count, sizeof(int), 1, file_handle) == -1)
        printf("Unable to read the number of flashcards in the file: %s\n", strerror(errno));
    if (fcs.count > 0) {
        fcs.cards = malloc(sizeof(Flashcard) * (size_t) fcs.count);
        if (fread(fcs.cards, sizeof(Flashcard), (size_t) fcs.count, file_handle) == -1)
            printf("Unable to read the number of flashcards in the file: %s\n", strerror(errno));
    }
    printf("Loaded flashcards from DB\n");
    fclose(file_handle);
    return fcs;
}

void display(Flashcards const* fcs)
{
    if (fcs->cards == NULL) {
        printf("No flashcards to display.\n");
        return;
    }
    for (int i = 0; i < fcs->count; i++) {
        printf("\n%d. ", fcs->cards[i].id);
        printf("%s \n", fcs->cards[i].question);
        printf("   %s \n", fcs->cards[i].answer);
    }
}

int main() {
    Flashcards fcs = loadFlashcardsFromFile(DB_FILENAME);
    int choice;

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
        getchar();  // To clear the newline character from input buffer
        switch (choice) {
        case 1:
            addFlashcard(&fcs);
            break;
        case 2:
            deleteFlashcard(&fcs);
            break;
        case 3:
            editFlashcard(&fcs);
            break;
        case 4:
            studyFlashcards(&fcs);
            break;
        case 5:
            display(&fcs);
            break;
        default:
            saveFlashcardsToFile(&fcs, DB_FILENAME);
            if (fcs.cards != NULL)
                free(fcs.cards);
            printf("Flashcards saved and exiting...\n");
            return 0;
        }
    }

    if (fcs.cards != NULL)
        free(fcs.cards);

    return 0;
}
