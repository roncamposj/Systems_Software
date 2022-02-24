// Ronald Campos

// A virtual machine is a software based program of an instruction set architecture.  It is basically a virtual implementation of a physical computer.

#include<stdio.h>
#include<stdlib.h>

#define MAX_STACK_HEIGHT 500

//  struct that contains all the codes
struct instruction
{
    int opcode;
    int l;
    int m;
};





// Halt will tell main function when to finish cycle.
int halt = 0;

// Input and output files.
FILE *ifp;
FILE *ofp;




// Instruction register./
struct instruction ir;
int pas[MAX_STACK_HEIGHT] = {0};



//  Function that enables moving levels down in the base
int base(int level, int BP)
{

    int arb = BP;
    int counter = level;

    while(counter > 0)
    {
        arb = pas[arb];
        counter--;
    }

    return arb;
}


int main(int argc, char *argv[]){

//  Initialize pc, bp, sp
    int pc = 0;
    int bp = 0;
    int sp = 0;
    int i = 0;

    // read in from command line and assign to *ifp
    char *inputFile = argv[1];
    ifp = fopen(inputFile, "r");

    ofp = fopen("output.txt", "w");

    // If file is corrupt or empty, return 0.
    if(ifp == NULL){
        printf("File could not open. Program halted.\n");
        return 0;
    }


    char c;
    int counter = 0;
    while((c=fgetc(ifp))!=EOF) {
        if(c=='\n')
            counter++;
    }
    struct instruction readArray[counter];
    rewind(ifp);

    //  Read in text file and pass to array struct.
    int instrTracker = 0;
    int tmpTracker = instrTracker;

//Read in the lines from the input file.
    while(!feof(ifp))
    {
        tmpTracker = instrTracker;
        fscanf(ifp, "%d %d %d\n", &readArray[tmpTracker].opcode, &readArray[tmpTracker].l,
               &readArray[tmpTracker].m);
//        printf("%d\t", readArray[tmpTracker].opcode);
//        printf("%d\t", readArray[tmpTracker].l);
//        printf("%d\n", readArray[tmpTracker].m);
        instrTracker++;

    }


    sp = ((tmpTracker+1)*3)-1;
    bp = sp + 1;
    int initialBP = bp;




    // Header for program
    printf("\t\tPC   BP   SP   stack\n");
    printf("Initial values: %d    %d   %d\n", pc, bp, sp);

    fprintf(ofp,"\t\tPC   BP   SP   stack\n");
    fprintf(ofp,"Initial values: %d    %d   %d\n", pc, bp, sp);

// Fetch cycle begins

// Assign pc to temp var to keep track of pc before it increases.

    int tmpPC = pc;
    int index = 0;

    // While SYS 03 isn't called (while halt is 0), run cycle

    while(halt==0) {

        // Assign PC to instruction register
        index = pc/3;

        ir.opcode = readArray[index].opcode;
        ir.l = readArray[index].l;
        ir.m = readArray[index].m;


        tmpPC = pc;
        if (tmpPC < 10)
            printf(" ");

        // Increment pc
        pc += 3;

        // Execution cycle begins
        switch (ir.opcode) {
            // LIT
            case 1:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("LIT  ");
                fprintf(ofp, "LIT  ");
                sp = sp + 1;
                pas[sp] = ir.m;
                break;

                //OPR
            case 2:
                switch (ir.m) {
                    // RTN
                    case 0:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("RTN  ");
                        fprintf(ofp, "RTN  ");
                        //pas[bp - 1] = pas[sp];
                        sp = bp - 1;
                        bp = pas[sp + 2];
                        pc = pas[sp + 3];
                        break;
                        //NEG
                    case 1:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("NEG  ");
                        fprintf(ofp, "NEG  ");
                        pas[sp] = (-1 * pas[sp]);
                        break;
                        // ADD
                    case 2:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("ADD  ");
                        fprintf(ofp, "ADD  ");
                        sp = sp - 1;
                        pas[sp] = pas[sp] + pas[sp + 1];
                        break;
                        // SUB
                    case 3:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("SUB  ");
                        fprintf(ofp, "SUB  ");
                        sp = sp - 1;
                        pas[sp] = pas[sp] - pas[sp + 1];
                        break;
                        // MUL
                    case 4:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("MUL  ");
                        fprintf(ofp, "MUL  ");
                        sp = sp - 1;
                        pas[sp] = pas[sp] * pas[sp + 1];
                        break;
                        // DIV
                    case 5:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("DIV  ");
                        fprintf(ofp, "DIV  ");
                        sp = sp - 1;
                        pas[sp] = pas[sp] / pas[sp + 1];
                        break;
                        // ODD
                    case 6:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("ODD  ");
                        fprintf(ofp, "ODD  ");
                        pas[sp] = pas[sp] % 2;
                        break;
                        // MOD
                    case 7:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("MOD  ");
                        fprintf(ofp, "MOD  ");
                        sp = sp - 1;
                        pas[sp] = pas[sp] % pas[sp + 1];
                        // EQL
                    case 8:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("EQL  ");
                        fprintf(ofp, "EQL  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] == pas[sp + 1]);
                        break;
                        // NEQ
                    case 9:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("NEQ  ");
                        fprintf(ofp, "NEQ  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] != pas[sp + 1]);
                        break;
                        // LSS
                    case 10:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("LSS  ");
                        fprintf(ofp, "LSS  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] < pas[sp + 1]);
                        break;
                        // LEQ
                    case 11:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("LEQ  ");
                        fprintf(ofp, "LEQ  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] <= pas[sp + 1]);
                        break;
                        // GTR
                    case 12:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("GTR  ");
                        fprintf(ofp, "GTR  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] > pas[sp + 1]);
                        break;
                        // GEQ
                    case 13:
                        printf("%d ", tmpPC);
                        fprintf(ofp, "%d ", tmpPC);
                        printf("GEQ  ");
                        fprintf(ofp, "GEQ  ");
                        sp = sp - 1;
                        pas[sp] = !(pas[sp] >= pas[sp + 1]);
                        break;

                    default:
                        break;


                }
                break;

                //LOD
            case 3:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("LOD  ");
                fprintf(ofp, "LOD  ");
                sp = sp + 1;
                pas[sp] = pas[base(ir.l, bp) + ir.m];
                break;

                //STO
            case 4:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("STO  ");
                fprintf(ofp, "STO  ");
                pas[base(ir.l, bp) + ir.m] = pas[sp];
                sp = sp - 1;
                break;

                //CAL
            case 5:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("CAL  ");
                fprintf(ofp, "CAL  ");
                pas[sp + 1] = base(ir.l, bp);
                pas[sp + 2] = bp;
                pas[sp + 3] = pc;
                pas[sp + 4] = pas[sp];
                bp = sp + 1;
                pc = ir.m;
                break;

                //INC
            case 6:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("INC  ");
                fprintf(ofp, "INC  ");
                sp = sp + ir.m;
                break;

                //JMP
            case 7:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("JMP  ");
                fprintf(ofp, "JMP  ");
                pc = ir.m;
                break;

                //JPC
            case 8:
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("JPC  ");
                fprintf(ofp, "JPC  ");
                if (pas[sp] == 1)
                    pc = ir.m;

                sp = sp - 1;
                break;

                //SYS
            case 9:
                switch (ir.m) {
                    // PRINT TOP OF STACK VALUE
                    case 1:
                        printf("Output result is: ");
                        fprintf(ofp, "Output result is: ");
                        printf("%d\n", pas[sp]);
                        fprintf(ofp, "%d\n", pas[sp]);
                        sp = sp - 1;
                        break;
                        // ASK FOR INTEGER INPUT
                    case 2:
                        sp = sp + 1;
                        printf("Please Enter an Integer: ");
                        fprintf(ofp, "Please Enter an Integer: \n");
                        scanf("%d", &pas[sp]);
                        printf("\n");
                        //fprintf(ofp, "%d\n", pas[sp]);
                        break;
                        // FINISH CYCLE
                    case 3:
                        halt = 1;
                        break;

                    default:
                        break;
                }
                printf("%d ", tmpPC);
                fprintf(ofp, "%d ", tmpPC);
                printf("SYS  ");
                fprintf(ofp, "SYS  ");
                break;

        }

        // Print l, m, pc, bp, sp and take care of necessary indentation.

        printf("%d", ir.l);
        fprintf(ofp, "%d", ir.l);

        if (ir.m < 10 && ir.m >= 0) {
            printf(" ");
            fprintf(ofp, " ");
        }
        printf(" %d", ir.m);
        fprintf(ofp, " %d", ir.m);


        if (pc < 10 && pc >= 0) {
            printf(" ");
            fprintf(ofp, " ");
        }
        printf("    %d", pc);
        fprintf(ofp, "    %d", pc);


        if (bp < 10 && bp >= 0)
        {
            printf(" ");
            fprintf(ofp, " ");
        }
        printf("   %d", bp);
        fprintf(ofp, "   %d", bp);


        if (sp < 10 && sp >= 0)
        {
            printf(" ");
            fprintf(ofp, " ");
        }
        printf("   %d", sp);
        fprintf(ofp,"   %d", sp);

        printf("   ");
        fprintf(ofp,"   ");


        // Print contents of pas
        for(int j = initialBP; j <= sp; j++)
        {
            if (initialBP < bp && j == bp || (initialBP == pas[j] && pas[j] != pas[j-1]))
            {
                printf("| ");
                fprintf(ofp, "| ");
            }


            printf("%d  ", pas[j]);
            fprintf(ofp, "%d  ", pas[j]);
        }

        printf("\n");
        fprintf(ofp, "\n");


    }

// fclose both read and write *files.
    fclose(ifp);
    fclose(ofp);

    return 0;
}

