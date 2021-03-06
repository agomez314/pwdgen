#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include <sqlite3.h>

/**
 * pwdgen: Unix utilty to generate random passwords given a set of conditions.
 * You can either type in the word or use a file
 * It has basic error handling (minimum word length)
 * A new password is created every time you run it.
 * 
 * additional requirement: you need a master key to access a secure vault that allows
 * for storing and retrieval of passwords. While store they have to be encrypted, when 
 * retrieved they are decrypted. Use sqlite to store and crypto to hash.
 */

const int MIN_PWD_LENGTH = 8;
regex_t noSpecialCharsRegex;
regex_t yesOrNoRegex;
regex_t numRegex;
regex_t vowelRegex;
sqlite3 *db;
sqlite3_stmt *res;

char password_arr[10][100]; //up to 10 pwds, 100 chars max in length
int password_ctr = 0;
// utility fn for creating random number in range provided
int randomNumberInRange(int range)
{
    return (1 + rand() / ((RAND_MAX + 1u) / range));
}

// prompts for initial word to make a password
void promptWord(char *initial_word)
{
    char error_list_arr[2][100];
    int err_counter = 0;
    printf("What's a word you'll remember?\n> ");
    scanf("%s", initial_word);
    if (strlen(initial_word) < MIN_PWD_LENGTH)
    {
        strcpy(error_list_arr[0], "Please use more than 8 characters");
        err_counter++;
    }

    if (strlen(initial_word) > 255)
    {
        strcpy(error_list_arr[0], "Please use less characters");
        err_counter++;
    }

    if (!regexec(&noSpecialCharsRegex, initial_word, 0, NULL, 0))
    {
        strcpy(error_list_arr[1], "Please don't use special characters or numbers");
        err_counter++;
    }
    if (err_counter > 0)
    {
        for (int i = 0; i < 2; i++)
            printf("%s\n", error_list_arr[i]);
        promptWord(initial_word);
    }
    return;
}

// prompts for numbers
int promptRule(char *message)
{
    char answer_arr[2];
    answer_arr[1] = '\0';
    char *answer = answer_arr;
    printf("Do you want to use %s? [y or n]\n> ", message);
    scanf("%s", answer);
    if (regexec(&yesOrNoRegex, answer, 0, NULL, 0))
    {
        printf("Please answer y or n\n");
        promptRule(message);
    }
    answer_arr[0] = tolower(answer_arr[0]);
    return strcmp(answer, "y") ? 0 : 1;
}

// transforms input word into 'secure' password
void transformPwd(char *final_word, char *initial_word, int shouldUseSpecialChars)
{
    int r = rand();
    char specialChars[] = {'!', '@', '#', '$', '*'};
    char vowels[] = {'a', 'e', 'i', 'o', 'u'};

    // swap first and last letters
    char temp = initial_word[0];
    final_word[0] = initial_word[strlen(initial_word) - 1];
    final_word[strlen(initial_word) - 1] = temp;

    for (int i = 1; i < strlen(initial_word) - 1; i++)
    {
        char c = initial_word[i];
        char *c_ptr = &c;
        if (!regexec(&vowelRegex, c_ptr, 0, NULL, 0))
        {
            if (shouldUseSpecialChars)
            {
                final_word[i] = randomNumberInRange(3) == 3 ? specialChars[randomNumberInRange(5) - 1] : initial_word[i];
            }
            else
            {
                final_word[i] = vowels[randomNumberInRange(5) - 1];
            }
        }
        else
        {
            final_word[i] = randomNumberInRange(2) == 2 ? toupper(initial_word[i]) : initial_word[i];
        }
    }
    strcpy(password_arr[password_ctr++], final_word);
}

void compileRegex();

void setupDB();

void compileRegex()
{
    /* compile regex */
    if (regcomp(&noSpecialCharsRegex, "[^a-zA-Z ]", 0) == 1 || regcomp(&yesOrNoRegex, "^[ynYN]$", 0) == 1 || regcomp(&vowelRegex, "[aeiouAEIOU]", 0) == 1 || regcomp(&numRegex, "[1-9]", 0) == 1)
    {
        printf("Regex error...\n");
    }
}

void openDB()
{
    int rc = sqlite3_open("file:test.db", &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open database: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
    }
}

void openTable(char *err_msg)
{

    char *sql = "create table if not exists Pwdgen (passwords varchar(255));";
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Could not create pwdgen table: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
}

char retrieved_pwd_arr[256];
char *retrieved_pwd = retrieved_pwd_arr;
int callback(void *veryUsed, int argc, char **argv, char **azColName)
{
    char *retrieved_pwd = (char *)veryUsed;
    retrieved_pwd = argv[0];

    printf("%s\n", retrieved_pwd);
    return 0;
}
int compareMasterPwd(char *master_pwd)
{
    char sql_arr[256];
    char *sql = sql_arr;
    sql = "SELECT * FROM Pwdgen;";
    char *err_msg;
    int rc = sqlite3_exec(db, sql, callback, &retrieved_pwd, &err_msg);
    printf("%s\n", retrieved_pwd);
    return strcmp(master_pwd, retrieved_pwd);
}

void checkForPwdOrCreateOne(char *err_msg)
{
    char master_pwd_arr[256];
    char *master_pwd = master_pwd_arr;
    char *sql = "SELECT EXISTS(SELECT * FROM Pwdgen);";
    int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error finding record: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return;
    }
    if (rc) // if record is not found = 0
    {
        // If not, prompt user to create one
        printf("Looks like this is the first time you're opening this utility.\nPlease create a new master password:\n>");
        scanf("%s", master_pwd);
        char sql_arr[256];
        char *sql = sql_arr;
        char *insert = "INSERT into Pwdgen values('";
        strcat(sql, insert);
        strcat(sql, master_pwd);
        strcat(sql, "');");
        int rc = sqlite3_exec(db, sql, 0, 0, &err_msg);
        if (rc != SQLITE_OK)
        {
            fprintf(stderr, "Could not insert: %s\n", sqlite3_errmsg(db));
            sqlite3_close(db);
            return;
        }
        else
        {
            printf("Created new master password.\n");
        }
    }
    else // if record is found = 1
    {
        printf("Please enter your password:\n>");
        scanf("%s", master_pwd);
        int i = 0;
        while (1)
        {
            int result = compareMasterPwd(master_pwd);
            if (result)
            {
                break;
            }
        }
    }
}

void setupDB()
{
    char *err_msg = 0;

    // Open database
    openDB();

    // Open pwdgen table or create if it does not exist
    openTable(err_msg);

    // Check if master pwd exists, if not then prompt user to create one
    checkForPwdOrCreateOne(err_msg);
}

// entrypoint of a user on program init
int promptMasterPassword()
{
    char *master_pwd = "velociraptor";
    char user_input_arr[256];
    char *user_input = user_input_arr;
    printf("Please enter your password:");
    int try_counter = 0;
    while (try_counter < 3)
    {
        printf("\n>");
        scanf(" %s", user_input);
        if (strcmp(user_input, master_pwd) == 0)
        {
            printf("Successfully entered password.\n");

            return 1;
        }
        else
        {
            try_counter++;
        }
    }
    printf("You cannot pass\n");
    return 0;
}

void createNewPassword()
{
    char initial_word_arr[256];
    char *initial_word = initial_word_arr;
    promptWord(initial_word);
    int shouldUseSpecialChars = promptRule("special characters");
    char final_pwd_arr[255];
    char *final_word = final_pwd_arr;
    transformPwd(final_word, initial_word, shouldUseSpecialChars);
    printf("Your password is: %s\n", final_word);
}

// print all passwords
void printPasswords()
{
    printf("These are your passwords:\n");
    for (int i = 0; i < password_ctr; i++)
    {
        printf("%d) %s\n", i + 1, password_arr[i]);
    }
}

// main menu function where all
void mainMenu()
{
    printf("What would you like to do?\n[1] create new password\n[2] display passwords\n[3] exit\n>");
    int num = 0;
    int *input = &num;
    scanf(" %d", input);
    while (1)
    {
        if (*input == 1)
        { // create new password
            createNewPassword();
            mainMenu();
        }
        else if (*input == 2)
        { // display password
            printPasswords();
            mainMenu();
        }
        else if (*input == 3)
        { // exit
            break;
        }
        else
        { //invalid regex
            printf("Please enter 1, 2 or 3\n");
            mainMenu();
        }
    }
}

int main(int argc, char *argv[])
{
    /**
     * 1. request a word to remember
     * 2. ask for nums
     * 3. ask for special chars
     * 4. ask for length
     * 5. compute password
     *      - vowels replaced by special chars or nums (if either are allowed)`
     *      - first and last letter are scrambled
     *      - replacements are random
     *      - a special char is placed either at beginning or end randomly
     */
    srand((unsigned int)time(NULL));
    compileRegex();
    //setupDB();
    printf("Welcome to pwdgen, a password-creating utility!\n");

    int promptPwd = promptMasterPassword();
    if (!promptPwd)
        return 1;

    mainMenu();

    printf("Have a good day!");

    sqlite3_finalize(res);
    sqlite3_close(db);
    return 0;
}
