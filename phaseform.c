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

typedef struct /* phasevitdats struct */
{
    unsigned nlfile;
    unsigned numinds;
    unsigned numloci;
} phasevitdats;

typedef struct /* wdats struct */
{
    unsigned wsz;
    char w_t;
} wdats;

typedef struct
{
    char l_t; /* what type of line is iti? use first symbol only: E, empty line, I: indented line, C: commetn line, N. normal line */
    unsigned ccou;
    unsigned wcou;
    wdats *wda; /* word dat array */
} ldats; /* line data */

ldats **crea_ldats(unsigned howmany)
{
    int i;
    ldats **ncpla /* numchar per line array */ = malloc(howmany*sizeof(ldats*));
    for(i=0;i<howmany;++i) {
        ncpla[i]=malloc(sizeof(ldats));
        ncpla[i]->l_t='?';
        ncpla[i]->ccou=0;
        ncpla[i]->wcou=0;
        ncpla[i]->wda=NULL;
    }
    return ncpla;
}

void free_ldats(ldats **ncpla, unsigned howmany)
{
    int i;
    for(i=0;i<howmany;++i) {
        if(ncpla[i]->wcou)
            free(ncpla[i]->wda);
        free(ncpla[i]);
    }
    free(ncpla);
}

void assign_l_t(char *l_t, char lsw0) /* we shall assigned line type based on first char of first word */
{
    switch(lsw0) {
        case '\n':
            *l_t='E'; return;
        case ' ': case '\t':
            *l_t='I'; return;
        case '#': case '>':
            *l_t='C'; return;
        default:
            *l_t='N'; return;
    }
}

ldats **readinpfile(char *fname, phasevitdats *pvddats)
{
    FILE *fin=fopen(fname, "r");
    int i, c;
    char oldc=' ';
    unsigned nc /* numchars */=0, oldnc=0, nl /*num lines */=0;

    unsigned ncpla_buf=CABUF;
    ldats **ncpla=crea_ldats(ncpla_buf);

    char *lsw /* last seen word */, lswtype='u';
    unsigned lwbuf=WBUF;
    lsw=calloc(lwbuf, sizeof(char));
    unsigned wc=0 /* word-character count */, gwc=0 /* general word count */, oldgwc=0;
    unsigned wszabuf=0;

    for(;;) {
        c=fgetc(fin);
        if(c==EOF) break;
        /* deal with first character of a line here */
        if(nc==oldnc)
            assign_l_t(&(ncpla[nl]->l_t), c);
        if( (c == ' ') || (c == '\n') || (c == '\t') )  {
            if( (oldc != ' ') & (oldc != '\n') & (oldc != '\t')) {
                lsw[wc]='\0';
                gwc++;
                if(wszabuf == (gwc-oldgwc-1) ) { /* not first word */
                    wszabuf+=1;
                    ncpla[nl]->wda = realloc(ncpla[nl]->wda, wszabuf*sizeof(wdats));
                }
                ncpla[nl]->wda[gwc-oldgwc-1].wsz = wc;
                ncpla[nl]->wda[gwc-oldgwc-1].w_t = lswtype;
                if(nl==0) {
                    if(lswtype!='u')
                        printf("first word on first line is not an integer\n");
                    else
                        pvddats->numinds=atoi(lsw);
                } else if(nl==1) {
                    if(lswtype!='u')
                        printf("first word on second line is not an integer\n");
                    else
                        pvddats->numloci=atoi(lsw);
                }
                lswtype='u';
                wc=0;
            } /* no else, which means consecutive white space will be ignored */
        } else {
            CONDREALLOC(wc, lwbuf, WBUF, lsw, char);
            if( (c<48) | (c>57) ) /* if yes, means there's no chance it's a psoitive integer, u */
                lswtype='c'; /* any old characters */
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
                    ncpla[i]->wda=NULL;
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
    free(lsw);
    printf("Report for file \"%s\" - numchars: %u, nwords= %u, numlines: %u\n", fname, nc, gwc, nl);

    pvddats->nlfile=nl;
    return ncpla;
}

ldats **readknofile(char *fname, phasevitdats *pvddats) /* this function processes the known file */
{
    FILE *fin=fopen(fname, "r");
    int i, c;
    char oldc=' ';
    unsigned nc /* numchars */=0, oldnc=0, nl /*num lines */=0;

    unsigned ncpla_buf=CABUF;
    ldats **ncpla=crea_ldats(ncpla_buf);

    char *lsw /* last seen word */, lswtype='k';
    unsigned lwbuf=WBUF;
    lsw=calloc(lwbuf, sizeof(char));
    unsigned wc=0 /* word-character count */, gwc=0 /* general word count */, oldgwc=0;
    unsigned wszabuf=0;

    for(;;) {
        c=fgetc(fin);
        if(c==EOF) break;
        /* deal with first character of a line here */
        if(nc==oldnc)
            assign_l_t(&(ncpla[nl]->l_t), c);
        if( (c == ' ') || (c == '\n') || (c == '\t') )  {
            if( (oldc != ' ') & (oldc != '\n') & (oldc != '\t')) {
                lsw[wc]='\0';
                gwc++;
                if(wszabuf == (gwc-oldgwc-1) ) { /* not first word */
                    wszabuf+=1;
                    ncpla[nl]->wda = realloc(ncpla[nl]->wda, wszabuf*sizeof(wdats));
                }
                ncpla[nl]->wda[gwc-oldgwc-1].wsz = wc;
                ncpla[nl]->wda[gwc-oldgwc-1].w_t = lswtype;
                lswtype='k'; /* a known symbol, * 0 or 1 */
                wc=0;
            } /* no else, which means consecutive white space will be ignored */
        } else {
            CONDREALLOC(wc, lwbuf, WBUF, lsw, char);
            if( (c!='*') & (c!='1') & (c!='0') ) /* if yes, means there's no chance it's a psoitive integer, u */
                lswtype='w'; /* wrong symbol for a known file! */
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
                    ncpla[i]->wda=NULL;
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
    free(lsw);
    printf("Report for file \"%s\" - numchars: %u, nwords= %u, numlines: %u\n", fname, nc, gwc, nl);

    pvddats->nlfile=nl;
    return ncpla;
}

int main(int argc, char *argv[])
{
    /* argument accounting: remember argc, the number of arguments, _includes_ the executable */
    if( (argc!=2) & (argc != 3)) {
        printf("Error. Pls supply 1 or 2 arguments (name of text files).\n");
        exit(EXIT_FAILURE);
    }

    int i, j;

    phasevitdats pvddats;

    ldats **ncpla=readinpfile(argv[1], &pvddats);

    printf("For a phase input file, integer on first line ...\n");
    printf("numinds:%u;numloci:%u\n", pvddats.numinds, pvddats.numloci);

    /* numloci to positions on line 3 check */
    if(pvddats.numloci != ncpla[2]->wcou-1)
        printf("!!! oh oh, check line 3\n");
    else
        printf("Numloc to position-coords on l.3 checked and good.\n");
    /* numloci to positions on line 3 check */
    if(pvddats.numloci != ncpla[3]->ccou)
        printf("!!! oh oh, check line 4\n");
    else
        printf("Numloci to position-types on l.4 checked and good.\n");
    /* numloci to positions on line 3 check */
    unsigned supposedtlines=pvddats.numinds*3+4;
    printf("supposedlines=%u\n", supposedtlines); 
    if(supposedtlines != pvddats.nlfile)
        printf("!!! oh oh, totalines num unexpected\n");
    else
        printf("Total number of lines as expected ...all good.\n");
    /* numloci to positions on line 3 check */
    for(i=5;i<supposedtlines;i+=3) {
        if(pvddats.numloci != ncpla[i]->wcou)
            printf("!!! oh oh, check line %u\n", i);
        if(pvddats.numloci != ncpla[i+1]->wcou)
            printf("!!! oh oh, check line %u\n", i+1);
    }

        /*
    for(i=0;i<pvddats.nlfile;++i) {
        printf("l.%u/t=%c) #w=%u: ", i, ncpla[i]->l_t, ncpla[i]->wcou);
        for(j=0;j<ncpla[i]->wcou;++j) 
            printf("%u%c ", ncpla[i]->wda[j].wsz, ncpla[i]->wda[j].w_t);
        printf("\n"); 
    }
    */

    free_ldats(ncpla, pvddats.nlfile);

    /* OK. we're going to look at the knoiw file now */
    phasevitdats pvddats2;
    ncpla=readknofile(argv[2], &pvddats2);
    char error_in_knownfile=0;

    /* numloci to positions on line 3 check */
    if(pvddats2.nlfile != pvddats.numinds) {
        printf("Error. Known file must have same number of lines as numloci in inp file\n");
        error_in_knownfile=1;
    } else
        printf("Numlines in known file good.\n");

    /* go through every line in the known file making sure it has the right number of characters */ 
    for(i=0;i<pvddats2.nlfile;i++) {
        if(1 != ncpla[i]->wcou) {
            printf("!!! oh oh, line %u in the known file doesn't have right number of words (1).\n", i);
            error_in_knownfile=1;
        } 
        if(pvddats.numloci != ncpla[i]->ccou) {
            printf("!!! oh oh, line %u in the known file doesn't have right character length.\n", i);
            error_in_knownfile=1;
        } 
        for(j=0;j<ncpla[i]->wcou;j++)
            if(ncpla[i]->wda[j].w_t != 'k') {
                printf("!!! oh oh, check word %u on line %u in the known file.\n", j, i);
                error_in_knownfile=1;
            }
    }
    if(error_in_knownfile)
        printf("You need to check your known file .. it doesn't correspond exacty with what it should be.\n");
    else 
        printf("Checked and good: all lines in known file have right length, numwords and have symbols one of either */1/0\n");

    free_ldats(ncpla, pvddats2.nlfile);

    return 0;
}
