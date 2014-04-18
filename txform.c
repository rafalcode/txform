/* txform, a program to check text form */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
   /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
	if(argc!=2) {
		printf("Error. Pls supply argument (name of text file).\n");
		exit(EXIT_FAILURE);
	}

   FILE *fin=fopen(argv[1], "r");
   int c;
   unsigned nc /* numchars */=0, nl /*num lines */=0;

   for(;;) {
       c=fgetc(fin);
       nc++;
       if(c==EOF) break;
       if(c=='\n') nl++;
   }

   fclose(fin);

   printf("Report for file \"%s\" - num chars: %u, numlines: %u\n", argv[1], nc, nl);

   return 0;
}
