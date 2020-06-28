#include <stdio.h>
#include <string.h>
#include <regex.h>
/**
 * pwdgen: Unix utilty to generate random passwords given a set of conditions.
 * You can either type in the word or use a file
 * It has basic error handling (minimum word length)
 * A new password is created every time you run it.
 * 
 * additional requirement: you need a master key to access a secure vault that allows
 * for storing and retrieval of passwords. While store they have to be encrypted, when 
 * retrieved they are decrypted. 
 */
int main(int argc, char *argv[])
{
    /**
     * 1. request a word to remember
     * 2. ask for nums
     * 3. ask for special chars
     * 4. ask for length
     * 5. compute password
     *      - vowels replaced by special chars or nums (if either are allowed)
     *      - first and last letter are scrambled
     *      - replacements are random
     *      - a special char is placed either at beginning or end randomly
     */
    char initial_word[100];
    int a = 0;
    int canUseSpecialChars = 0;
    int MIN_PWD_LENGTH = 8;
    regex_t regex;
    int regex_return_value;
    return_value = regcomp(&regex, "[^a-zA-Z]", 0);
    if (return_value == 0)
    {
        printf("Regular expression compiled successfully.");
    }
    else
    {
        printf("Compilation error.");
    }

    do
    {
        printf("Welcome to pwdgen, the world's best mediocre password utility!\n");
        printf("What's a word you'll remember?\n");
        scanf("> %s", initial_word);
        if (strlen(initial_word) < MIN_PWD_LENGTH)
        {
            printf("Please type in a word longer than 8 characters\n");
            scanf("> %s", initial_word);
        }
        else if (!regexec(&regex, initial_word, 0, NULL, 0))
        {
        }
        else
        {
            printf("Can you use special characters? [y or n]\n");
            scanf("> %s", &canUseSpecialChars);
        }

    } while (scanf("%d", &a) != EOF);

    return 0;
}