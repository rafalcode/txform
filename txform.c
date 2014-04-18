/* txform, a program to check text form */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CABUF 2
/* Reminder: a is the array whose size is being incremented, and t is the type, a string  */
#define CONDREALLOC(x, b, c, a, t); \
        if((x)==((b)-1)) { \
                    (b) += (c); \
                    (a)=realloc((a), (b)*sizeof(t)); \
                }

int main(int argc, char *argv[])
{
   /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
	if(argc!=2) {
		printf("Error. Pls supply argument (name of text file).\n");
		exit(EXIT_FAILURE);
	}

   FILE *fin=fopen(argv[1], "r");
   int i, c;
   unsigned nc /* numchars */=0, oldnc=0, nl /*num lines */=0;
   unsigned *ncpla /* numchar per line array */=NULL, ncpla_buf=CABUF;
   ncpla=malloc(ncpla_buf*sizeof(unsigned));

   for(;;) {
       c=fgetc(fin);
       nc++;
       if(c==EOF) break;
       if(c=='\n') {
           CONDREALLOC(nl, ncpla_buf, CABUF, ncpla, unsigned);
           ncpla[nl++]=nc-oldnc-1; /* minus one so not to include newlines */
           oldnc=nc;
       }
   }
   ncpla=realloc(ncpla, nl*sizeof(unsigned)); /*normalize*/

   fclose(fin);
   printf("Report for file \"%s\" - num chars: %u, numlines: %u\n", argv[1], nc, nl);
   printf("numchar per line array is: ");
   for(i=0;i<nl;++i) 
       printf("%u ", ncpla[i]);
   printf("\n"); 

   free(ncpla);

   return 0;
}
