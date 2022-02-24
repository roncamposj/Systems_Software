/*
 Derek Dyer
 Ronald Campos

Skeleton code provided has been altered
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

symbol *table;
int sym_index;
int table_index;
int error;


lexeme *globalList;
lexeme token;


void printtable();
void errorend(int x);
void program(int level);

void block(int level);
void const_declaration(int level);
void var_declaration(int level);
void proc_declaration(int level);
void statement(int level);
lexeme getNext();
int isItInTable(char *name, int kind);


//secondary additional functions
void condition();
void expression();
void term();
void factor();


symbol *parse(lexeme *input)
{
    table = malloc(1000 * sizeof(symbol));
    sym_index = 0;
    table_index = 0;
    error = 0;
    globalList = input;

    int level = 0;

    // will need to change
    //int inputLength = 226;
    //int inputLength = sizeof(*input);

    //printf("%d\n", inputLength);

    /*
    for (sym_index = 188; sym_index < 226; sym_index++)
	{
        token = getNext();
        program(token.value);
    }
    */

    // increases sym_index until it finds where everything is
    while (globalList[sym_index].type == 0)
    {
        sym_index++;
    }

    token = getNext();
    program(token.value);

    //printf("ERROR %d\n", error);

    if (error > 0)
    {
        free(table);
        return NULL;
    }
    else
    {
        printtable();
        return table;
    }

    return NULL;
}

lexeme getNext()
{
    lexeme getToken = globalList[sym_index++];
    //printf("TOKEN TYPE %d\n", getToken.type);
    return getToken;
}

int isItInTable(char *name, int kind)
{
    for (int i = 0; i < table_index; i++)
    {
        if (strcmp(table[i].name, name) == 0 || table[i].kind == kind)
        {
            return i;
        }
    }
    return -1;
}

void program(int level)
{
    block(level);
    if (error == 0)
    {
        if (token.type != periodsym)
        {
            errorend(3);
            error = 3;
            return;
        }
    }
}

void block(int level)
{
    if (error == 0)
    {
        const_declaration(level);
    }
    if (error == 0)
    {
        var_declaration(level);
    }
    if (error == 0)
    {
        proc_declaration(level);
    }
    if (error == 0)
    {
        statement(level);
    }


}

void const_declaration(int level)
{
    if (token.type == constsym)
    {
        do
        {
            token = getNext();

            if (token.type != identsym)
            {
                errorend(4);
                error = 4;
                return;
            }
            char *name = token.name;

            //token.type is not relevant if there are const or var in different levels
            // need to find way to implement this.
            if (isItInTable(name, 1) != -1 || isItInTable(name, 2) != -1)
            {
                errorend(1);
                error = 1;
                return;
            }
            token = getNext();
            if(token.type != becomessym)
            {
                errorend(5);
                error = 5;
                return;
            }
            token = getNext();
            if (token.type != numbersym)
            {
                errorend(5);
                error = 5;
                return;
            }

            //add to symbol table
            table[table_index].kind = 3;
            strcpy(table[table_index].name, name);
            table[table_index].val = 0;
            table[table_index].level = level;
            table[table_index].addr = 0;

            table_index++;

            token = getNext();

        } while (token.type == commasym);

        if (token.type != semicolonsym)
        {
            errorend(6);
            error = 6;
            return;
        }

        token = getNext();
    }
}

void var_declaration(int level)
{
    if (token.type == varsym)
    {
        int numVar = 0;

        do
        {
            token = getNext();
            if(token.type != identsym)
            {
                errorend(4);
                error = 4;
                return;
            }
            char *name = token.name;

            if (isItInTable(name, 1) != -1 || isItInTable(name, 2) != -1)
            {
                errorend(1);
                error = 1;
                return;
            }

            table[table_index].kind = 2;
            strcpy(table[table_index].name, name);
            table[table_index].val = 0;
            table[table_index].level = level;
            table[table_index].addr = numVar + 3;

            table_index++;


            numVar++;
            token = getNext();


        } while (token.type == commasym);

        if (token.type != semicolonsym)
        {
            errorend(6);
            error = 6;
            return;
        }

        token = getNext();
    }
}

void proc_declaration(int level)
{
    while (token.type == procsym)
    {
        token = getNext();
        if (token.type != identsym)
        {
            errorend(4);
            error = 4;
            return;
        }
        char *name = token.name;

        if(isItInTable(name, 3) != -1)
        {
            errorend(1);
            error = 1;
            return;
        }

        table[table_index].kind = 3;
        strcpy(table[table_index].name, name);
        table[table_index].val = 0;
        table[table_index].level = level;
        table[table_index].addr = 0;

        table_index++;

        token = getNext();

        if (token.type != semicolonsym)
        {
            errorend(6);
            error = 6;
            return;
        }

        token = getNext();
        block(level++);

        if (token.type != semicolonsym)
        {
            errorend(6);
            error = 6;
            return;
        }

        token = getNext();

    }
}

void statement(int level)
{

    //case for identsym

    if (token.type == identsym)
    {
        char *name = token.name;
        if(isItInTable(name, 1) == -1)
        {
            errorend(7);
            error = 7;
            return;
        }
        token = getNext();

        if(token.type != becomessym)
        {
            errorend(2);
            error = 2;
            return;
        }
        token = getNext();
        expression();
        //may need to get next token here.
        return;
    }

    //case for callsym

    if (token.type == callsym)
    {
        token = getNext();
        if (token.type != identsym)
        {
            errorend(14);
            error = 14;
            return;
        }
        char *name = token.name;
        if(isItInTable(name, 3) == -1)
        {
            errorend(7);
            error = 7;
            return;
        }
        token = getNext();
        return;
    }

    //case for beginsym
    if (token.type == beginsym)
    {
        do
        {
            token = getNext();
            statement(level);
        } while(token.type == semicolonsym);

        if (token.type != endsym)
        {
            errorend(10);
            error = 10;
            return;
        }

        token = getNext();
        return;
    }

    // case for if
    if (token.type == ifsym)
    {
        token = getNext();
        condition(level);
        if (token.type != thensym)
        {
            errorend(9);
            error = 9;
            return;
        }
        token = getNext();
        statement(level);
        if (token.type == elsesym)
        {
            statement(level);
        }

        return;
    }

    //case for while
    if (token.type == whilesym)
    {
        token = getNext();
        condition(level);
        if (token.type != dosym)
        {
            errorend(8);
            error = 8;
            return;
        }
        token = getNext();
        statement(level);

        return;
    }

    //case for read

    if (token.type == readsym)
    {
        token = getNext();
        if (token.type != identsym)
        {
            errorend(14);
            error = 14;
            return;
        }
        char *name = token.name;
        if(isItInTable(name, 1) == -1)
        {
            errorend(7);
            error = 7;
            return;
        }
        token = getNext();
        statement(level);

        return;
    }

    //case for write
    if (token.type == writesym)
    {
        token = getNext();
        expression();
        return;
    }
}

void condition()
{
    if (token.type == oddsym)
    {
        token = getNext();
        expression();
    }
    else
    {
        expression();
        if (token.type != eqlsym || token.type != neqsym || token.type != lessym
            || token.type != leqsym || token.type != gtrsym || token.type != geqsym)
        {
            errorend(12);
            error = 12;
            return;
        }
        token = getNext();
        expression();
    }
}

void expression()
{
    if (token.type == plussym || token.type == minussym)
    {
        token = getNext();
    }
    term();
    while (token.type == plussym || token.type == minussym)
    {
        token = getNext();
        term();
    }
}

void term()
{
    factor();
    while (token.type == multsym || token.type == slashsym || token.type == oddsym)
    {
        token = getNext();
        factor();
    }
}
void factor()
{
    if (token.type == identsym)
    {
        char *name = token.name;
        if (isItInTable(name, 1) == -1 || isItInTable(name, 2) == -1)
        {
            errorend(7);
            error = 7;
            return;
        }
        token = getNext();
    }
    else if (token.type == numbersym)
    {
        token = getNext();
    }
    else if (token.type == lparentsym)
    {
        token = getNext();
        expression();
        if (token.type == rparentsym)
        {
            errorend(13);
            error = 13;
            return;
        }
        token = getNext();
    }
    else
    {
        // potentially error 2.  they will make an announcement clarifying.
        errorend(11);
        error = 11;
        return;
    }
}

void errorend(int x)
{
    switch (x)
    {
        case 1:
            printf("Parser Error: Competing Symbol Declarations\n");
            break;
        case 2:
            printf("Parser Error: Unrecognized Statement Form\n");
            break;
        case 3:
            printf("Parser Error: Programs Must Close with a Period\n");
            break;
        case 4:
            printf("Parser Error: Symbols Must Be Declared with an Identifier\n");
            break;
        case 5:
            printf("Parser Error: Constants Must Be Assigned a Value at Declaration\n");
            break;
        case 6:
            printf("Parser Error: Symbol Declarations Must Be Followed By a Semicolon\n");
            break;
        case 7:
            printf("Parser Error: Undeclared Symbol\n");
            break;
        case 8:
            printf("Parser Error: while Must Be Followed By do\n");
            break;
        case 9:
            printf("Parser Error: if Must Be Followed By then\n");
            break;
        case 10:
            printf("Parser Error: begin Must Be Followed By end\n");
            break;
        case 11:
            printf("Parser Error: while and if Statements Must Contain Conditions\n");
            break;
        case 12:
            printf("Parser Error: Conditions Must Contain a Relational-Operator\n");
            break;
        case 13:
            printf("Parser Error: ( Must Be Followed By )\n");
            break;
        case 14:
            printf("Parser Error: call and read Must Be Followed By an Identifier\n");
            break;
        default:
            printf("Implementation Error: Unrecognized Error Code\n");
            break;
    }

}

void printtable()
{
    int i;
    printf("Symbol Table:\n");
    printf("Kind | Name        | Value | Level | Address\n");
    printf("------------------------------------------------------\n");
    for (i = 0; i < sym_index; i++)
    {
        printf("%4d | %11s | %5d | %5d | %5d\n", table[i].kind, table[i].name, table[i].val, table[i].level, table[i].addr);
    }
}