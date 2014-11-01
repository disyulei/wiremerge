#include "Design.h"
#include "main.h"

void printWelcome();
void printUsage();

int
main(int argc, char* argv[])
{
    printWelcome();

    Design* LC = new Design();
    if (false == LC->parseParameters(argc, argv))
    {
        printUsage();
        exit(1);
    }
    LC->readAll();
    LC->mergeWires();
    LC->outputAscii();
} 


void
printWelcome()
{
    printf("\n");
    printf("==================   wireMerge - Version 2.1  ====================\n");
    printf("   Author      :  Bei Yu  (UT Austin)                             \n");
    printf("   Last Update :  11/2014                                         \n");
    printf("   Function    :  remove overlappings in layout                   \n");
    printf("==================================================================\n");
}


void
printUsage()
{
    printf("\n");
    printf("==================   Command-Line Usage   ====================\n");
    printf("\"-in\"         : followed by input library (default: \"input.ascii\"). \n");
    printf("\"-out\"        : followed by input library (default: \"output.ascii\"). \n");
    printf("\n");
}
