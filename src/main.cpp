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
} 


void
printWelcome()
{
  printf("\n");
  printf("==================   wireMerge - Version 1.2  ====================\n");
  printf("   Author      :  Bei Yu  (UT Austin)                             \n");
  printf("   Last Update :  06/2014                                         \n");
  printf("   Function    :  remove overlappings in layout                   \n");
  printf("==================================================================\n");
}


void
printUsage()
{
  printf("\n");
  printf("==================   Command-Line Usage   ====================\n");
  printf("\"-inlib\"         : followed by input library (default: \"MX_Benchmark1_clip\"). \n");
  printf("\"-layer\"         : followed by layer number (default: 10). \n");
	printf("\n");
}
