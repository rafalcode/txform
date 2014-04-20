/* txform, a program to check text form */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CABUF 2
#define WBUF 10 /* word buffer */
/* Reminder: a is the array whose size is being incremented, and t is the type, a string  */
#define CONDREALLOC(x, b, c, a, t); \
    if((x) >=((b)-1)) { \
        (b) += (c); \
        (a)=realloc((a), (b)*sizeof(t)); \
    }

typedef struct
{
    char l_t; /* what type of line is iti? use first symbol only: E, empty line, I: indented line, C: commetn line, N. normal line */
    unsigned ccou;
    unsigned wcou;
    unsigned *wsza;
} ldats; /* line data */

ldats **crea_ldats(unsigned howmany)
{
    int i;
    ldats **ncpla /* numchar per line array */ = malloc(howmany*sizeof(ldats*));
    for(i=0;i<howmany;++i) {
        ncpla[i]=malloc(sizeof(ldats));
        ncpla[i]->l_t='N';
        ncpla[i]->ccou=0;
        ncpla[i]->wcou=0;
        ncpla[i]->wsza=NULL;
    }
    return ncpla;
}

void free_ldats(ldats **ncpla, unsigned howmany)
{
    int i;
    for(i=0;i<howmany;++i) {
        if(ncpla[i]->wcou)
            free(ncpla[i]->wsza);
        free(ncpla[i]);
    }
    free(ncpla);
}

int main(int argc, char *argv[])
{
    /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
    if(argc!=2) {
        printf("Error. Pls supply argument (name of text file).\n");
        exit(EXIT_FAILURE);
    }

    FILE *fin=fopen(argv[1], "r");
    int i, j, c;
    char oldc=' ';
    unsigned nc /* numchars */=0, oldnc=0, nl /*num lines */=0;

    unsigned ncpla_buf=CABUF;
    ldats **ncpla=crea_ldats(ncpla_buf);

    char *lsw /* last seen word */;
    unsigned lwbuf=WBUF;
    lsw=calloc(lwbuf, sizeof(char));
    unsigned wc=0 /* word-character count */, gwc=0 /* general word count */, oldgwc=0;
    unsigned wszabuf=0;

    for(;;) {
        c=fgetc(fin);
        if(c==EOF) break;
        if( (c == ' ') || (c == '\n') || (c == '\t') )  {
            if( (oldc != ' ') & (oldc != '\n') & (oldc != '\t')) {
                lsw[wc]='\0';
                gwc++;
                if(wszabuf == (gwc-oldgwc-1) ) { /* not first word */
//                    printf("nl:%u;gwc:%u;oldgwc:%u ", nl, gwc, oldgwc);
                    wszabuf+=1;
                    ncpla[nl]->wsza = realloc(ncpla[nl]->wsza, wszabuf*sizeof(unsigned));
                }
                ncpla[nl]->wsza[gwc-oldgwc-1] = wc;
        //        printf("%u) lw=\"%s\";sz=%u\n", gwc, lsw, wc);
                wc=0;
            } /* no else, which means consecutaive white space will be ignored */
        } else {
            CONDREALLOC(wc, lwbuf, WBUF, lsw, char);
            lsw[wc++]=c;
        }
        nc++; /* having this just after EOF detector means EOF is not coutned as a character, which is the convention */

        if(c=='\n') {
            if(nl==ncpla_buf-1) {
                ncpla_buf += CABUF;
                ncpla=realloc(ncpla, ncpla_buf*sizeof(ldats*));
                for(i=ncpla_buf-CABUF;i<ncpla_buf; ++i) {
                    ncpla[i]=malloc(sizeof(ldats));
                    ncpla[i]->ccou=0;
                    ncpla[i]->wcou=0;
                    ncpla[i]->wsza=NULL;
                }
            }
            ncpla[nl]->ccou=nc-oldnc-1; /* minus one so not to include newlines */
            ncpla[nl]->wcou=gwc-oldgwc; /* minus one so not to include newlines */
            nl++;
            wszabuf=0;
            oldnc=nc;
            oldgwc=gwc;
        }
        oldc=c;
    }
    for(i=nl;i<ncpla_buf;++i) 
        free(ncpla[i]);
    ncpla=realloc(ncpla, nl*sizeof(ldats*)); /*normalize*/

    fclose(fin);
    printf("Report for file \"%s\" - numchars: %u, nwords= %u, numlines: %u\n", argv[1], nc, gwc, nl);
    printf("Numchars, numwords, sz-per-word per line array is:\n");
    for(i=0;i<nl;++i) {
        printf("l.%u) %u words: ", i, ncpla[i]->wcou);
        for(j=0;j<ncpla[i]->wcou;++j) 
            printf("%u ", ncpla[i]->wsza[j]);
        printf("\n"); 
    }

    free(lsw);
    free_ldats(ncpla, nl);

    return 0;
}
