#include <stdio.h>
#include <string.h>
#include <regex.h>
#include <stdlib.h>

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

    if (strlen(initial_word) > 254 /*includes null terminator + additional number*/)
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
int promptNumbers()
{
    char answer_arr[2];
    char *answer = answer_arr;
    printf("Do you want to use numbers? [y or n]\n> ");
    scanf("%s", answer);
    if (regexec(&yesOrNoRegex, answer, 0, NULL, 0))
    {
        printf("Please answer y or n\n");
        promptNumbers();
    }
    return strcmp(answer, "y") ? 1 : 0;
}

//prompts for special characters
int promptSpecialChars()
{
    char answer_arr[2];
    char *answer = answer_arr;
    printf("Do you want to use special characters? [y or n]\n> ");
    scanf("%s", answer);
    if (regexec(&yesOrNoRegex, answer, 0, NULL, 0))
    {
        printf("Please answer y or n\n");
        promptSpecialChars();
    }
    return strcmp(answer, "y") ? 1 : 0;
}

// transforms input word into 'secure' password
char transformPwd(char *initial_word, int shouldUseNumbers, int shouldUseSpecialChars)
{
}

// entrypoint of a user on program init
void promptPassword()
{
    /**
     * 1. make a call to sqlite to determine whether this is first time setup or not.
     * 2. if first time, setup password.
     *      - if not, prompt password
     * 3. get password
     *      - if valid, proceed
     *      - if not valid, prompt again
     */
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

    /* compile regex */
    if (regcomp(&noSpecialCharsRegex, "[^a-zA-Z ]", 0) == 1 || regcomp(&yesOrNoRegex, "^[ynYN]$", 0) == 1)
    {
        printf("Regex error...\n");
    }
    printf("Welcome to pwdgen, a password utility by a noob, for noobs!\n");
    promptPassword();
    char initial_word_arr[256];
    char *initial_word = initial_word_arr;
    promptWord(initial_word);
    int shouldUseNumbers = promptNumbers();
    int shouldUseSpecialChars = promptSpecialChars();
    char *final_pwd = transformPwd(initial_word, shouldUseNumbers, shouldUseSpecialChars);
    printf("Your password is: %s\n", final_pwd);
    printf("Have a good day!");
    return 0;
}