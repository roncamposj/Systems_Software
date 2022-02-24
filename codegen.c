/*
	Author: Derek Dyer and Ronald Campos
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "compiler.h"

instruction *code;
symbol *globalTable;
lexeme *globalTokens;
lexeme *token;

//set global variables used throughout code
int code_index;
int CODE_SIZE = 500;
int symbol_index;
int token_index;

//function that advances token list
lexeme* getToken(int i);

//helper functions
void program();
void block(int lexlevel);
int constantPrime(int lexlevel);
int varPrime(int lexlevel);
void statement(int lexlevel, int savedSIndex);
void condition (int lexlevel);
void expression(int lexlevel);
void expressionPrime(int lexlevel);
void term(int lexlevel);
void termPrime(int lexlevel);
void factor(int lexlevel);
void emit(int op, int l, int m);

void printcode();

//initializes everything.  calls program()
instruction *generate_code(lexeme *tokens, symbol *symbols)
{
    code = malloc(500 * sizeof(instruction));

    code_index = 0;
    CODE_SIZE = 500;
    symbol_index = 0;
    token_index = 0;

    globalTable = symbols;
    globalTokens = tokens;


    program();

    printcode();
    return code;
}

lexeme* getToken(int i)
{
    lexeme *getToken = &globalTokens[i];
    //printf("TOKEN TYPE %d\n", getToken.type);
    return getToken;
}

//begins and halts the code generator
void program()
{
    int i = 0;
    while (globalTable[i].kind != 0)
    {
        globalTable[i].mark = 0;
        i++;
    }

    emit(7, 0, 0);

    block(0);

    emit(9, 0, 3);

    code[0].m = globalTable[0].val * 3;
}

//searches through tokens and calls constant/var/procedure check if necessary.
//also handles INC emit()
void block (int lexlevel)
{
    int numSymbols = 0;
    int numVars = 0;
    int savedSIndex = 0;
    int procedureIndex = symbol_index;

    if (globalTokens[token_index].type == constsym)
    {
        token_index += 4;
        token = getToken(token_index);

        numSymbols++;

        //unmark the const in the symbol table
        globalTable[symbol_index].mark = 0;
        savedSIndex = symbol_index;
        symbol_index += 1;

        int addSymbols = constantPrime(lexlevel);

        numSymbols += addSymbols;

        token_index += 1;
        token = getToken(token_index);
    }

    if (globalTokens[token_index].type == varsym)
    {

        token_index += 2;
        token = getToken(token_index);

        numVars++;
        numSymbols++;

        //unmark the const in the symbol table
        globalTable[symbol_index].mark = 0;
        savedSIndex = symbol_index;
        symbol_index += 1;

        int addSymbols = varPrime(lexlevel);

        numVars += addSymbols;
        numSymbols += addSymbols;

        token_index += 1;
        token = getToken(token_index);
    }

    if (globalTokens[token_index].type == procsym)
    {
        do
        {
            token_index += 3;
            token = getToken(token_index);

            //unmark the const in the symbol table
            globalTable[symbol_index].mark = 0;
            savedSIndex = symbol_index;
            symbol_index += 1;

            block(lexlevel + 1);

            token_index += 1;
            token = getToken(token_index);

            emit(2, 0, 0);
        }
        while (token->type == procsym);
    }

    globalTable[procedureIndex].val = code_index;

    emit(6, 0, numVars + 3);
    statement(lexlevel, savedSIndex);

    for (int i = procedureIndex; i < procedureIndex + numSymbols; i++)
    {
        globalTable[i].mark = 1;
    }
}


//handles constants returns number in commasym as part of constsym
int constantPrime(int lexlevel)
{
    int addSymbols = 0;

    if (token->type == commasym)
    {
        token_index += 4;
        token = getToken(token_index);

        globalTable[symbol_index].mark = 0;
        symbol_index += 1;

        addSymbols += 1;

        addSymbols += constantPrime(lexlevel);

        token_index += 1;
        token = getToken(token_index);
    }

    return addSymbols;
}


//returns number of symbols in commasym as part of varsym
int varPrime(int lexlevel)
{
    int addSymbols = 0;
    if (token->type == commasym)
    {
        token_index += 2;
        token = getToken(token_index);

        globalTable[symbol_index].mark = 0;
        symbol_index += 1;

        addSymbols += 1;

        addSymbols += varPrime(lexlevel);
    }

    return addSymbols;
}

//handles ident/if/else/call/read/write/while/begin situations and their emit()
void statement(int lexlevel, int savedSIndex)
{
    token = getToken(token_index);

    if (token->type == identsym)
    {
        int varIndex = 0;
        for (int i = symbol_index; i > 0; i--)
        {
            if (strcmp(globalTable[i].name, token->name) == 0)
            {
                if (globalTable[i].level <= lexlevel)
                {
                    if (globalTable[i].kind == 2)
                    {
                        varIndex = i;
                        break;
                    }
                }
            }
        }


        token_index += 2;
        token = getToken(token_index);


        expression(lexlevel);

        emit(4, lexlevel - globalTable[varIndex].level, globalTable[varIndex].addr);
    }

    if (token->type == callsym)
    {
        token_index += 1;
        token = getToken(token_index);

        //save the symbol table index of the procedure entry unmarked and closest in lexlevel??
        int procIndex = 0;
        for (int i = symbol_index; i > 0; i--)
        {
            if (strcmp(globalTable[i].name, token->name) == 0)
            {
                if (globalTable[i].level <= lexlevel)
                {
                    if (globalTable[i].kind == 3)
                    {
                        procIndex = i;
                        break;
                    }
                }
            }
        }

        token_index += 1;
        token = getToken(token_index);

        emit(5, lexlevel - globalTable[procIndex].level, globalTable[procIndex].val * 3);
    }

    if (token->type == writesym)
    {
        token_index += 1;
        token = getToken(token_index);

        expression(lexlevel);
        emit(9, 0, 1);
    }

    if (token->type == readsym)
    {
        token_index += 1;
        token = getToken(token_index);

        //save sym_index for var unmarked and closest in lexlevel??
        int varIndex = 0;
        for (int i = symbol_index; i > 0; i--)
        {
            if (strcmp(globalTable[i].name, token->name) == 0)
            {
                if (globalTable[i].level <= lexlevel)
                {
                    if (globalTable[i].kind == 2)
                    {
                        varIndex = i;
                        break;
                    }
                }
            }
        }

        token_index += 1;
        token = getToken(token_index);

        emit(9, 0, 2);
        emit(4, lexlevel - globalTable[varIndex].level, globalTable[varIndex].addr);
    }

    if (token->type == beginsym)
    {
        token_index += 1;
        token = getToken(token_index);

        statement(lexlevel, savedSIndex);

        while (token->type == semicolonsym)
        {
            token_index += 1;
            token = getToken(token_index);

            statement(lexlevel, savedSIndex);
        }

        token_index += 1;
        token = getToken(token_index);
    }

    if (token->type == ifsym)
    {
        token_index += 1;
        token = getToken(token_index);

        condition(lexlevel);

        //save the code index for jpc?
        int jpcIndex = code_index;

        emit(8, 0, 0);

        token_index += 1;
        token = getToken(token_index);

        statement(lexlevel, savedSIndex);

        if (token->type == elsesym)
        {
            token_index += 1;
            token = getToken(token_index);

            //save the current code index for jmp
            int jmpIndex = code_index;

            emit(7, 0, 0);

            //fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex???
            code[jpcIndex].m = code_index * 3;

            statement(lexlevel, savedSIndex);

            //fix the jmp from earlier, savedCodeIndexForJMP.M = currentcodeindex???
            code[jmpIndex].m = code_index * 3;
        }

        else
        {
            //fix the jpc from earlier, savedCodeIndexForJPC.M = currentcodeindex??
            code[jpcIndex].m = code_index * 3;
        }
    }

    if (token->type == whilesym)
    {
        token_index += 1;
        token = getToken(token_index);

        //save the code index for condition
        int jmpIndex = code_index;

        condition(lexlevel);

        token_index += 1;
        token = getToken(token_index);

        //save the code index for jump
        int jpcIndex = code_index;

        emit(8, 0, 0);

        statement(lexlevel, savedSIndex);

        emit(7, 0, jmpIndex * 3);

        //fix the JPC from earlier, savedCodeIndexForJump.M = currentcodeindex  ??
        code[jpcIndex].m = code_index * 3;
    }
}

//handles oddsym/==/</<=/>/>= emit()
void condition (int lexlevel)
{

    if (globalTokens[token_index].type == oddsym)
    {
        token_index += 1;
        token = getToken(token_index);

        expression(lexlevel);
        emit(2, 0, 6);
    }
    else
    {
        expression(lexlevel);
        lexeme* relOp = token;

        switch(relOp->type)
        {
            case eqlsym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 8);
                break;

            case neqsym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 9);
                break;

            case lessym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 10);
                break;

            case leqsym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 11);
                break;

            case gtrsym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 12);
                break;
            case geqsym:
                token_index += 1;
                token = getToken(token_index);

                expression(lexlevel);
                emit(2, 0, 13);
                break;
        }
    }
}

//handles negative emit() and/or calls expressionPrime()
void expression(int lexlevel)
{

    if (globalTokens[token_index].type == plussym)
    {
        token_index += 1;
        token = getToken(token_index);

        term(lexlevel);
        expressionPrime(lexlevel);
    }
    else if (globalTokens[token_index].type == minussym)
    {
        token_index += 1;
        token = getToken(token_index);

        term(lexlevel);

        emit(2,0,1);
        expressionPrime( lexlevel);
    }
    else
    {
        term(lexlevel);
        expressionPrime( lexlevel);
    }
}

//recursive function that handles + and - emit()
void expressionPrime(int lexlevel)
{
    if (token->type == plussym)
    {
        token_index += 1;
        token = getToken(token_index);

        term(lexlevel);

        emit(2,0,2);
        expressionPrime( lexlevel);
    }
    if (token->type == minussym)
    {
        token_index += 1;
        token = getToken(token_index);

        term(lexlevel);

        emit(2,0,3);
        expressionPrime(lexlevel);
    }
}

void term(int lexlevel)
{
    factor(lexlevel);
    termPrime(lexlevel);

}

// handles * , / , and %
void termPrime(int lexlevel)
{
    if (token->type == multsym)
    {
        token_index += 1;
        token = getToken(token_index);

        factor(lexlevel);

        emit(2,0,4);
        termPrime(lexlevel);
    }
    if (token->type == slashsym)
    {
        token_index += 1;
        token = getToken(token_index);

        factor(lexlevel);

        emit(2,0,5);
        termPrime(lexlevel);
    }
    if (token->type == modsym)
    {
        token_index += 1;
        token = getToken(token_index);

        factor(lexlevel);

        emit(2,0,7);
        termPrime(lexlevel);
    }
}

//handles certain LIT and LOD emit() cases
void factor(int lexlevel)
{
    if (token->type == identsym)
    {
        //save the symbol table index of the var or const entry unmarked and closest in lexlevel
        int savedIndex = 0;
        for (int i = symbol_index; i > 0; i--)
        {
            if (strcmp(globalTable[i].name, token->name) == 0)
            {
                if (globalTable[i].level <= lexlevel)
                {
                    if (globalTable[i].kind == 1 || globalTable[i].kind == 2)
                    {
                        savedIndex = i;
                        break;
                    }
                }
            }
        }

        if (globalTable[savedIndex].kind == 1)
        {
            emit(1, 0, globalTable[savedIndex].val);
        }

        else if (globalTable[savedIndex].kind == 2)
        {
            emit(3, lexlevel - globalTable[savedIndex].level, globalTable[savedIndex].addr);
        }

        token_index += 1;
        token = getToken(token_index);
    }

    else if (token->type == numbersym)
    {
        emit(1,  0, token->value);

        token_index += 1;
        token = getToken(token_index);
    }

    else
    {
        token_index += 1;
        token = getToken(token_index);

        expression(lexlevel);

        token_index += 1;
        token = getToken(token_index);
    }

}

//stores op, l m into code[] generated to be printed
void emit(int op, int l, int m)
{
    if (code_index > CODE_SIZE)
    {
        printf("Too much code.\n");
    }
    else
    {
        code[code_index].opcode = op;
        code[code_index].l = l;
        code[code_index].m = m;
        code_index += 1;
    }
}

void printcode()
{
    int i;
    printf("Line\tOP Code\tOP Name\tL\tM\n");
    for (i = 0; i < code_index; i++)
    {
        printf("%d\t", i);
        printf("%d\t", code[i].opcode);
        switch (code[i].opcode)
        {
            case 1:
                printf("LIT\t");
                break;
            case 2:
                switch (code[i].m)
                {
                    case 0:
                        printf("RTN\t");
                        break;
                    case 1:
                        printf("NEG\t");
                        break;
                    case 2:
                        printf("ADD\t");
                        break;
                    case 3:
                        printf("SUB\t");
                        break;
                    case 4:
                        printf("MUL\t");
                        break;
                    case 5:
                        printf("DIV\t");
                        break;
                    case 6:
                        printf("ODD\t");
                        break;
                    case 7:
                        printf("MOD\t");
                        break;
                    case 8:
                        printf("EQL\t");
                        break;
                    case 9:
                        printf("NEQ\t");
                        break;
                    case 10:
                        printf("LSS\t");
                        break;
                    case 11:
                        printf("LEQ\t");
                        break;
                    case 12:
                        printf("GTR\t");
                        break;
                    case 13:
                        printf("GEQ\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            case 3:
                printf("LOD\t");
                break;
            case 4:
                printf("STO\t");
                break;
            case 5:
                printf("CAL\t");
                break;
            case 6:
                printf("INC\t");
                break;
            case 7:
                printf("JMP\t");
                break;
            case 8:
                printf("JPC\t");
                break;
            case 9:
                switch (code[i].m)
                {
                    case 1:
                        printf("WRT\t");
                        break;
                    case 2:
                        printf("RED\t");
                        break;
                    case 3:
                        printf("HAL\t");
                        break;
                    default:
                        printf("err\t");
                        break;
                }
                break;
            default:
                printf("err\t");
                break;
        }
        printf("%d\t%d\n", code[i].l, code[i].m);
    }
}