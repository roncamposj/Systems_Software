/*
 Derek Dyer
 Ronald Campos

*/

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include "compiler.h"



lexeme *list;
int lex_index;

char* reservedWords[]={"const", "var", "procedure", "call", "if", "then", "else", "while", "do", "begin", "end", "read", "write", "odd"};

//For Reserved Special Symbols
char* specialSymbols[]={"::=", "==", "<>'",  "<",  "<=",  ">",  ">=",  "%",  "*", "/", "+",  "-",  "(",  ")",  ", " , ".",  ";",  ":="};

void printerror(int type);
void printtokens(lexeme* list, int indexLength, int size);

lexeme *lexanalyzer(char *input)
{
    list = malloc(500 * sizeof(lexeme));

    //to my knowledge, supposed to find the size of arraylist.  current output given is 8 which is incorrect
    lex_index = strlen(input);
    //printf("%d\n", lex_index);

    //for identifying errors, comments, and checking next elements in array
    int errorHolder = 0;
    int comments = 0;
    int lookAhead;

    //run through arraylist
    int size = lex_index;
    for (int i = 0; i < size; i++)
    {
        //print element as you go along
        //printf("%c\n", input[i]);

        //should take care of whitespace. skips rest of the loop if it finds it.z
        if(input[i] == ' ' || input[i] == '\t' || input[i] == '\r' || input[i] == '\n')
        {
            lookAhead = 0;
            continue;
        }

        //reads if the next character is part of the alphabet
        if(isalpha(input[i]))
        {
            char characterString[12];
            memset(characterString, 0, sizeof characterString);

            int index = 0;
            characterString[index] = input[i];
            index++;
            lookAhead = 1;

            //prints an error if the variable name is too long
            while(isalpha(input[++i]) || isdigit(input[i]))
            {

                if(index >= 11)
                {
                    printerror(4);
                    errorHolder = 1;

                    //lookAhead=0;
                    break;
                }
                characterString[index] = input[i];
                index++;
            }

            //printf("%s\n", characterString);

            //if there is an error, exit loop and don't take in token
            if(errorHolder == 1)
            {
                break;
            }

            //Compares the variable name to see if it is one of the reserved words
            int reservedSwitch = -1;

            for(int j = 0; j < 14; j++)
            {
                if(strcmp(characterString, reservedWords[j]) == 0)
                {
                    reservedSwitch = j;
                }
            }

            //If it is a reserved word, print out the correct tokentype
            switch(reservedSwitch)
            {
                //Case for const
                case 0:
                    list[lex_index].type = constsym;
                    break;

                    //Case for var
                case 1:
                    list[lex_index].type = varsym;
                    break;

                    //Case for procedure
                case 2:
                    list[lex_index].type = procsym;
                    break;

                    //Case for call
                case 3:
                    list[lex_index].type = callsym;
                    break;
                    //Case for if
                case 4:
                    list[lex_index].type = ifsym;
                    break;

                    //Case for then
                case 5:
                    list[lex_index].type = thensym;
                    break;

                    //Case for else
                case 6:
                    list[lex_index].type = elsesym;
                    break;

                    //Case for while
                case 7:
                    list[lex_index].type = whilesym;
                    break;

                    //Case for do
                case 8:
                    list[lex_index].type = dosym;
                    break;

                    //Case for begin
                case 9:
                    list[lex_index].type = beginsym;
                    break;

                    //Case for end
                case 10:
                    list[lex_index].type = endsym;
                    break;

                    //Case for read
                case 11:
                    list[lex_index].type = readsym;
                    break;

                    //Case for write
                case 12:
                    list[lex_index].type = writesym;
                    break;

                    //Case for odd
                case 13:
                    list[lex_index].type = oddsym;
                    break;

                    //Case for identifier
                default:
                    list[lex_index].type = identsym;
                    strcpy(list[lex_index].name,characterString);
                    break;
            }

            lex_index++;
            i--;
        }

            //reads if the next character is part of the 0-9 digits
        else if(isdigit(input[i]))
        {
            int number = input[i] - '0';
            int d;
            int place = 1;

            lookAhead = 1;

            //prints error  if the number is too long
            while(isdigit(input[++i]))
            {
                if(place >= 5)
                {
                    printerror(3);
                    errorHolder = 1;
                    //lookAhead=0;
                    break;
                }

                //method for concatenating numbers in c
                d = input[i] - '0';
                number = 10 * number + d;
                place++;
            }

            //prints error if the variable starts with a digit, and not a number
            if(errorHolder == 0 && isalpha(input[i]))
            {
                printerror(2);
                errorHolder = 1;
                while(isalpha(input[++i]) || isdigit(input[i]))
                {
                    i++;
                }
                break;
            }

            //If there was an error, exit loop and don't accept token
            if(errorHolder == 1)
            {
                break;
            }

            //no errors found, so assign number to list and its value
            list[lex_index].type = numbersym;
            list[lex_index].value = number;
            lex_index++;
            i--;
        }

            //reads if the next character is part of the special symbols
        else
        {
            lookAhead = 0;
            int spec = -1;

            for(int j = 0; j < 17; j++)
            {
                if(input[i] == specialSymbols[j][0])
                {
                    spec = j;
                }
            }

            //if its a special symbol, print out its type
            // 0,       1,   2,     3,     4,    5,     6,    7,    8,    9,  10,  11,   12,   13,   14,   15,   16,    17
            //{"::=", "==", "<>'",  "<",  "<=",  ">",  ">=",  "%",  "*", "/", "+",  "-",  "(",  ")",  ", " , ".",  ";",  ":="};
            switch(spec)
            {
                //case for :=
                case 0:
                    if(input[i + 1] == '=')
                    {
                        list[lex_index].type = becomessym;
                        lex_index++;
                        i++;
                    }

                        //if it is not a valid symbol
                    else
                    {
                        printerror(1);
                        errorHolder = 1;
                    }
                    break;

                    //case for ==
                case 1:
                    list[lex_index].type = eqlsym;
                    lex_index++;
                    i++;
                    break;

                    //case for <
                case 2:
                case 3:
                case 4:
                    lookAhead=1;

                    //case for <>
                    if(input[i + 1] =='>')
                    {
                        list[lex_index].type = neqsym;
                        lookAhead = 0;
                        i++;
                    }

                        //case for just <=
                    else if(input[i + 1] =='=')
                    {
                        list[lex_index].type = leqsym;
                        lookAhead=0;
                        i++;
                    }

                        //case for <
                    else
                    {
                        list[lex_index].type = lessym;
                    }
                    lex_index++;
                    break;

                    //case for >
                case 5:
                case 6:
                    lookAhead = 1;

                    //case for >=
                    if(input[i + 1] =='=')
                    {
                        list[lex_index].type = geqsym;
                        lookAhead=0;
                        i++;
                    }

                        //case for >
                    else
                    {
                        list[lex_index].type = gtrsym;
                    }

                    lex_index++;
                    break;

                    //case for %
                case 7:
                    list[lex_index].type = modsym;
                    lex_index++;
                    break;

                    //case for *
                case 8:
                    list[lex_index].type = multsym;
                    lex_index++;
                    break;

                    //case for comments or just /
                case 9:

                    lookAhead = 1;
                    if(input[i + 1] == '*')
                    {
                        comments = 1;
                        lookAhead = 0;
                        while(comments == 1 && input[i] != '\0')
                        {
                            if(input[i + 1] == '*')
                            {
                                if(input[i + 2] == '/')
                                {
                                    comments = 0;
                                    i++;
                                }
                            }
                            i++;
                        }

                        if (comments == 1)
                        {
                            printerror(5);
                            errorHolder = 1;
                            break;
                        }
                    }
                    else
                    {
                        list[lex_index].type  = slashsym;
                        lex_index++;
                    }
                    break;

                    //case for +
                case 10:
                    list[lex_index].type = plussym;
                    lex_index++;
                    break;

                    //case for -
                case 11:
                    list[lex_index].type = minussym;
                    lex_index++;
                    break;

                    //case for (
                case 12:
                    list[lex_index].type = lparentsym;
                    lex_index++;
                    break;

                    //case for )
                case 13:
                    list[lex_index].type = rparentsym;
                    lex_index++;
                    break;

                    //case for ,
                case 14:
                    list[lex_index].type = commasym;
                    lex_index++;
                    break;

                    //case for .
                case 15:
                    list[lex_index].type = periodsym;
                    lex_index++;
                    break;

                    //case for ;
                case 16:
                    list[lex_index].type = semicolonsym;
                    lex_index++;
                    break;

                    //case for :
                case 17:
                    if(input[i + 1] == '=')
                    {
                        list[lex_index].type = becomessym;
                        lex_index++;
                        i++;
                        break;
                    }


                default:
                    printerror(1);
                    errorHolder = 1;
                    break;
            }
        }

        //if there is an error, exit loop and don't take in token
        if(errorHolder == 1)
        {
            break;
        }
    }

    if (errorHolder == 1)
    {
        list = NULL;
    }

    else if (errorHolder == 0)
    {
        printtokens(list, lex_index, size);
    }

    return list;
}

void printtokens(lexeme *list, int indexLength, int size)
{
    int i;
    printf("Lexeme Table:\n");
    printf("lexeme\t\ttoken type\n");
    for (i = size; i < indexLength; i++)
    {

        switch (list[i].type)
        {
            case oddsym:
                printf("%11s\t%d", "odd", oddsym);
                break;
            case eqlsym:
                printf("%11s\t%d", "==", eqlsym);
                break;
            case neqsym:
                printf("%11s\t%d", "<>", neqsym);
                break;
            case lessym:
                printf("%11s\t%d", "<", lessym);
                break;
            case leqsym:
                printf("%11s\t%d", "<=", leqsym);
                break;
            case gtrsym:
                printf("%11s\t%d", ">", gtrsym);
                break;
            case geqsym:
                printf("%11s\t%d", ">=", geqsym);
                break;
            case modsym:
                printf("%11s\t%d", "%", modsym);
                break;
            case multsym:
                printf("%11s\t%d", "*", multsym);
                break;
            case slashsym:
                printf("%11s\t%d", "/", slashsym);
                break;
            case plussym:
                printf("%11s\t%d", "+", plussym);
                break;
            case minussym:
                printf("%11s\t%d", "-", minussym);
                break;
            case lparentsym:
                printf("%11s\t%d", "(", lparentsym);
                break;
            case rparentsym:
                printf("%11s\t%d", ")", rparentsym);
                break;
            case commasym:
                printf("%11s\t%d", ",", commasym);
                break;
            case periodsym:
                printf("%11s\t%d", ".", periodsym);
                break;
            case semicolonsym:
                printf("%11s\t%d", ";", semicolonsym);
                break;
            case becomessym:
                printf("%11s\t%d", ":=", becomessym);
                break;
            case beginsym:
                printf("%11s\t%d", "begin", beginsym);
                break;
            case endsym:
                printf("%11s\t%d", "end", endsym);
                break;
            case ifsym:
                printf("%11s\t%d", "if", ifsym);
                break;
            case thensym:
                printf("%11s\t%d", "then", thensym);
                break;
            case elsesym:
                printf("%11s\t%d", "else", elsesym);
                break;
            case whilesym:
                printf("%11s\t%d", "while", whilesym);
                break;
            case dosym:
                printf("%11s\t%d", "do", dosym);
                break;
            case callsym:
                printf("%11s\t%d", "call", callsym);
                break;
            case writesym:
                printf("%11s\t%d", "write", writesym);
                break;
            case readsym:
                printf("%11s\t%d", "read", readsym);
                break;
            case constsym:
                printf("%11s\t%d", "const", constsym);
                break;
            case varsym:
                printf("%11s\t%d", "var", varsym);
                break;
            case procsym:
                printf("%11s\t%d", "procedure", procsym);
                break;
            case identsym:
                printf("%11s\t%d", list[i].name, identsym);
                break;
            case numbersym:
                printf("%11d\t%d", list[i].value, numbersym);
                break;
        }
        printf("\n");
    }
    printf("\n");
    printf("Token List:\n");
    for (i = size; i < indexLength; i++)
    {
        if (list[i].type == numbersym)
            printf("%d %d ", numbersym, list[i].value);
        else if (list[i].type == identsym)
            printf("%d %s ", identsym, list[i].name);
        else
            printf("%d ", list[i].type);
    }
    printf("\n");
    list[indexLength++].type = -1;
}

void printerror(int type)
{
    if (type == 1)
        printf("Lexical Analyzer Error: Invalid Symbol\n");
    else if (type == 2)
        printf("Lexical Analyzer Error: Invalid Identifier\n");
    else if (type == 3)
        printf("Lexical Analyzer Error: Excessive Number Length\n");
    else if (type == 4)
        printf("Lexical Analyzer Error: Excessive Identifier Length\n");
    else if (type == 5)
        printf("Lexical Analyzer Error: Neverending Comment\n");
    else
        printf("Implementation Error: Unrecognized Error Type\n");

    free(list);
    return;
}