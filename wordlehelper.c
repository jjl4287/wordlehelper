/* 
 * wordlehelper.c - a program that will help me solve wordles
 *                  utilizes tty modes, and regex functions
 * 
 * author - Jakob Langtry
 */

// enormous number of includes
#include <sys/types.h>
#include <sys/stat.h>
#include <termios.h>
#include <stdlib.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <regex.h>
#include <fcntl.h>

// prototyping functions
char * genEx(char *, char *, char *);
void   setCrNoEchoMode();
void   ttyMode(int);
int    yesNo();
int    check(char *, char *, char *);

int main()
{
    int  run = 0;                // loop stopper
    FILE *rp;                    // read file 
    FILE *wp;                    // write file
    char *grey;                  // grey letters
    char *green;                 // green letters
    char *yellow;                // yellow letters
    char *string;                // word string
    char *pattern;               // regex string
    char *tmp = NULL;            // for in out flipping
    char *inp = "./wordlewords"; // read file name
    char *out = "./wordleansrs"; // write file name

    // copies dict/words to local file for manipulation
    wp = fopen("./wordlewords", "w");
    rp = fopen("/usr/share/dict/words", "r");
    while(fscanf(rp, "%ms", &string) != EOF)
    {
        fprintf(wp, "%s\n", string);
    }
    fclose(rp);
    fclose(wp);

    // loop that runs until user specifies n to run again
    while(!run)
    {
        // open new input, output
        wp = fopen(out, "w");
        rp = fopen(inp, "r");

        /*
         * We ask for three strings. this means we call our yesno 
         * three times,  setting the tty  mode to normal  to  get 
         * the strings, then setting it  back for  next  question
         */
        ttyMode(0);
        setCrNoEchoMode();
        fprintf(stdout, "Grey characters? (y/n) ");
        if (!yesNo())
        {
            ttyMode(1);
            fprintf(stdout, "\nEnter them here: ");
            fscanf(stdin, "%ms", &grey);
            ttyMode(0);
            setCrNoEchoMode();
        }
        else
        {
            grey = (char *)malloc(2);
            strcpy(grey, "0");
            fprintf(stdout, "\n");
        }
        
        fprintf(stdout, "Yellow characters? (y/n) ");
        if (!yesNo())
        {
            ttyMode(1);
            fprintf(stdout, "\nEnter them here: ");
            fscanf(stdin, "%ms", &yellow);
            ttyMode(0);
            setCrNoEchoMode();
        }
        else
        {
            yellow = (char *)malloc(6);
            strcpy(yellow, "00000");
            fprintf(stdout, "\n");
        }
        
        fprintf(stdout, "Green characters? (y/n) ");
        if (!yesNo())
        {
            ttyMode(1);
            fprintf(stdout, "\nEnter them here: ");
            fscanf(stdin, "%ms", &green);
            ttyMode(0);
            setCrNoEchoMode();
        }
        else
        {
            green = (char *)malloc(6);
            strcpy(green, ".....");
            fprintf(stdout, "\n");
        }

        // we do this here because the pattern will be the same
        // for every check - no  need to  run with  every  call
        pattern = genEx(grey, yellow, green);

        // while input words exist
        while (fscanf(rp, "%ms", &string) != EOF)
        {   
            // save them, pass them with pattern and the  yellows
            // for comparing in check, if check passes print them 
            // to stdout for reading and output for input of next
            // run
            if (!check(string, pattern, yellow))
            {
                fprintf(stdout, "%s\n", string);
                fprintf(wp, "%s\n", string);
            }
        }

        // ask to run again, if not increment run to get out of loop
        fprintf(stdout, "Run Again? (y/n) ");
        if (yesNo())
        {
            run++;
        }
        fprintf(stdout, "\n");
        
        // reset tty mode and close files
        ttyMode(1);
        fclose(rp);
        fclose(wp);
        // switch in and out files for next run
        tmp = out;
        out = inp;
        inp = tmp;
    }
    
    // clean up
    free(string);
    free(yellow);
    free (green);
    free  (grey);
    unlink (out);
    unlink (inp);
    return   (0);
}

/*
 * genEx - a function which takes in given filter strings and 
 *         creates a large regex which limits to 5 char words
 *         without capitals, "'", the grey  letters, and  the 
 *         position specific yellow chars
 *
 */
char *genEx(char *grey, char *yellow, char *green)
{
    // malloc for maximum string size
    int size = (strlen(grey)*5) + strlen(yellow) + 45;
    char *pattern = (char *)malloc(size);
    // create pattern
    sprintf(pattern, "^");
    for (int i = 0; i < 5; i++)
    {
        if(green[i] == '.')
        {
            sprintf(pattern + strlen(pattern), "[^A-Z\'%s%c]", grey, yellow[i]);
        }
        else
        {
            sprintf(pattern + strlen(pattern), "%c", green[i]);
        }
    }
    sprintf(pattern + strlen(pattern), "$");

    return pattern;
}

/*
 * check - a function that takes in two regexs to compare against a string
 *         uses regex c functions to compile regex and compare
 * 
 *         returns 0 on match, 1 on non-match
 */
int check(char *string, char *pattern, char *yellow)
{
    regex_t     preg;
    int  rc      = 0;
    char  *yellowpat = (char *)malloc(4);

    // this takes our super string from genEx and compiles it
    if ((rc = regcomp(&preg, pattern, REG_NOSUB|REG_NEWLINE)) != 0)
    {
        printf("regcomp() failed, returning nonzero (%d)\n", rc);
        exit(1);
    }
    // this does the comparing 
    else if ((rc = regexec(&preg, string, 0, NULL, 0)) != 0)
    {
        return 1;
    }

    // this loop creates and compares a regex for all the yellow letters
    // example: [a]
    for (int i = 0; i < 5; i++)
    {
        if (yellow[i] != '0')
        {
            // create regex
            sprintf(yellowpat, "[%c]", yellow[i]);

            // compile
            if ((rc = regcomp(&preg, yellowpat, REG_NOSUB)) != 0)
            {
                printf("regcomp() failed, returning nonzero (%d)\n", rc);
                exit(1);
            }
            // compare
            else if ((rc = regexec(&preg, string, 0, NULL, 0)) != 0)
            {
                return 1;
            }
        }
    }
    regfree(&preg);
    free(yellowpat);
    return 0;
}

/*
 * ttyMode - when how is 0 non-canonical mode is set
 *            when how is 1 default is  set and saved
 *            also handles fcntl flags
 */
void ttyMode(int how)
{
    static struct termios original_mode;
    static int            original_flags;
    static int            stored = 0;
    if (!how)
    {
        // load bits, change them, save them
        tcgetattr(0, &original_mode);
        original_flags = fcntl(0, F_GETFL);
        stored++;
    }
    else
    {
        tcsetattr(0, TCSANOW, &original_mode);
        fcntl(0, F_SETFL, original_flags);
    }
}

/*
 * setCrNoEchoMode - puts fd 0 into chr by chr mode
 */
void setCrNoEchoMode()
{
    struct termios ttystate;

    // load bits, change them, save them
    tcgetattr(0, &ttystate);
    ttystate.c_lflag    &= ICANON;
    ttystate.c_lflag    &= ECHO;
    ttystate.c_cc[VMIN] =  1;
    tcsetattr(0, TCSANOW, &ttystate);
}

/*
 * yesNo - gets input on yes or no questions
 */
int yesNo()
{
    while(1)
    {
        switch(getchar())
        {
            case 'y': return 0;
            case 'n': 
            case EOF: return 1;
        }
    }
}