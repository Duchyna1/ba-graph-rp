#include "gendef.h"

static FILE *ergfile,*lstfile,*autfile;
static char erg[20],lst[20],aut[20];
/*********************g6-mod-start*********************/
static char g6name[20];
/*********************g6-mod-end*********************/

static SCHAR n,k,t=3,mid_max=NOTSET,splitlevel=NOTSET;
static SCHAR storeall=0,printall=0,efile=0;
static SCHAR codestdout=0,asciistdout=0;
static ULONG anz=0,tostore=0,toprint=0,count=0;
static UINT  jobs=0,jobnr=0;

/*********************gb-mod-start*********************/

extern char bipartite;     
extern char colour[MAXN];
extern int anzcolour[3]; 
extern int biplimit;       

/*********************gb-mod-end*********************/

/*********************g6-mod-start*********************/

static FILE *g6file;
static SCHAR g6all=0;
static SCHAR g6stdout=0;
static ULONG tog6=0;
static SCHAR nooutput=0;

/*********************g6-mod-end*********************/

void generate(void)
{
#ifdef DOSTIME
 clock_t start,stop;
 start=clock();
#endif
#ifdef UNIXTIME
 UINT stop;
 struct tms TMS;
#endif

 if(mid_max==NOTSET)
    mid_max=default_mid_max(n,k,t);

 if(splitlevel==NOTSET&&jobs>0)
    splitlevel=default_splitlevel(n,k,t,bipartite);

/* fprintf(ergfile,"Splitlevel=%d\n",splitlevel); */

#ifndef ENGLISH
 fprintf(ergfile,"\n          GENREG - Generator fuer regulaere Graphen\n");
 fprintf(ergfile,"          %d Knoten, Grad %d, ",n,k);
 fprintf(ergfile,"Taillenweite mind. %d\n",t);
/*********************gb-mod-start*********************/
 if (bipartite) fprintf(ergfile,"          Nur bipartite Graphen.\n");
/*********************gb-mod-end*********************/
 if(jobs)
    fprintf(ergfile,"          Erzeugung Teil %d von %d gestartet...\n",jobnr,jobs);
 else
    fprintf(ergfile,"          Erzeugung gestartet...\n");
#else
 fprintf(ergfile,"\n          GENREG - Generator for regular graphs\n");
 fprintf(ergfile,"          %d vertices, degree %d, ",n,k);
 fprintf(ergfile,"girth at least %d\n",t);
/*********************gb-mod-start*********************/
 if (bipartite) fprintf(ergfile,"          Only bipartite graphs.\n");
/*********************gb-mod-end*********************/
 if(jobs)
    fprintf(ergfile,"          Part %d of %d started...\n",jobnr,jobs);
 else
    fprintf(ergfile,"          Generation started...\n");
#endif
 fflush(ergfile);

/*********************gb-mod-start*********************/
 if (!bipartite || (n%2==0)) 
/*********************gb-mod-end*********************/
 ordstart(n,k,t,mid_max,
	  splitlevel,jobs,jobnr-1,
	  lstfile,autfile,ergfile,
	  tostore,toprint,count,
	  storeall,printall,&anz
/*********************g6-mod-start*********************/
         ,tog6,g6all,g6file,nooutput
/*********************g6-mod-end*********************/
);

#ifndef ENGLISH
 fprintf(ergfile,"          %llu Graphen erzeugt.\n",anz);
#else
 fprintf(ergfile,"          %llu graphs produced.\n",anz);
#endif

#ifdef DOSTIME
 stop=clock()-start;
#endif
#ifdef UNIXTIME
 if(times(&TMS)==-1);
 stop=(UINT)TMS.tms_utime; 
#endif
#ifdef time_factor
#ifndef ENGLISH
 fprintf(ergfile,"          Laufzeit:%.2fs  ",(float)stop/time_factor);
 if(stop>time_factor)
    fprintf(ergfile,"%.1f Repr./s",(float)anz/((float)stop/time_factor));
#else
 fprintf(ergfile,"          CPU time: %.2fs  ",(float)stop/time_factor);
 if(stop>time_factor)
    fprintf(ergfile,"%.1f graphs/s",(float)anz/((float)stop/time_factor));
#endif
 fprintf(ergfile,"\n");
#endif
}

SCHAR inputerror(void)
{
 SCHAR error=1;

#ifndef ENGLISH
 if(n>MAXN)
    fprintf(stderr,"maximal %d Knoten\n",MAXN);
 else
 if(n<MINN)
    fprintf(stderr,"mindestens %d Knoten\n",MINN);
 else
 if(k>MAXK)
    fprintf(stderr,"Grad maximal %d\n",MAXK);
 else
 if(k<MINK)
    fprintf(stderr,"Grad mindestens %d\n",MINK);
 else
 if(t>MAXT)
    fprintf(stderr,"Taillenweite maximal %d\n",MAXT);
 else
 if(t<MINT)
    fprintf(stderr,"Taillenweite mindestens %d\n",MINT);
 else
    error=0;
#else
 if(n>MAXN)
    fprintf(stderr,"maximum %d vertices\n",MAXN);
 else
 if(n<MINN)
    fprintf(stderr,"minimum %d vertices\n",MINN);
 else
 if(k>MAXK)
    fprintf(stderr,"degree maximum %d\n",MAXK);
 else
 if(k<MINK)
    fprintf(stderr,"degree minimum %d\n",MINK);
 else
 if(t>MAXT)
    fprintf(stderr,"girth maximum %d\n",MAXT);
 else
 if(t<MINT)
    fprintf(stderr,"girth minimum %d\n",MINT);
 else
    error=0;
#endif

 return error;
}

void openfiles(void)
{
    if(codestdout)
       lstfile=stdout;
    else
    if(storeall==1||tostore>0)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(lst,"%d%d_%d_%d-%d.b.%s",n/10,n%10,k,t,jobnr,APP);
	  else
/*********************gb-mod-end*********************/
       sprintf(lst,"%d%d_%d_%d-%d.%s",n/10,n%10,k,t,jobnr,APP);
       lstfile=fopen(lst,"wb");
      }

    if(asciistdout)
       autfile=stdout;
    else
    if(printall==1||toprint>0)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(aut,"%d%d_%d_%d-%d.b.asc",n/10,n%10,k,t,jobnr);
	  else
/*********************gb-mod-end*********************/
       sprintf(aut,"%d%d_%d_%d-%d.asc",n/10,n%10,k,t,jobnr);
       autfile=fopen(aut,"w");
      }

/*********************g6-mod-start*********************/
    if(g6stdout)
       g6file=stdout;
    else
    if(g6all==1||tog6>0)
      {
/*********************gb-mod-start*********************/
        if (bipartite)
            sprintf(g6name,"%d%d_%d_%d-%d.b.%s",n/10,n%10,k,t,jobnr,"g6");
          else
/*********************gb-mod-end*********************/
       sprintf(g6name,"%d%d_%d_%d-%d.%s",n/10,n%10,k,t,jobnr,"g6");
       g6file=fopen(g6name,"w");
      }
/*********************g6-mod-end*********************/

}

void closefiles(void)
{
    if((storeall==1||tostore>anz)&&!codestdout)
      {
       fclose(lstfile);
       if(anz==0)remove(lst);
      }

    if((printall==1||toprint>anz)&&!asciistdout)
      {
       fclose(autfile);
       if(anz==0)remove(aut);
      }
/*********************g6-mod-start*********************/
    if((g6all==1||tog6>anz)&&!g6stdout)
      {
       fclose(g6file);
       if(anz==0)remove(g6name);
      }
/*********************g6-mod-end*********************/
}

void movefiles(void)
{
    char neu[20];

    if((storeall==1||tostore>=anz)&&anz>0&&!codestdout)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d#%d.b.%s",n/10,n%10,k,t,jobnr,APP);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d#%d.%s",n/10,n%10,k,t,jobnr,APP);
       remove(neu);
       rename(lst,neu);
      }

    if((printall==1||toprint>=anz)&&anz>0&&!asciistdout)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d#%d.b.asc",n/10,n%10,k,t,jobnr);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d#%d.asc",n/10,n%10,k,t,jobnr);
       remove(neu);
       rename(aut,neu);
      }

/*********************g6-mod-start*********************/
    if((g6all==1||tog6>=anz)&&anz>0&&!g6stdout)
      {
/*********************gb-mod-start*********************/
        if (bipartite)
                  sprintf(neu,"%d%d_%d_%d#%d.b.%s",n/10,n%10,k,t,jobnr,"g6");
          else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d#%d.%s",n/10,n%10,k,t,jobnr,"g6");
       remove(neu);
       rename(g6name,neu);
      }
/*********************g6-mod-end*********************/
}


int
main(int argc, char **argv)
{
 char  neu[20],rec[20];
 FILE  *recfile;
 ULONG newanz=0;

/*********** Parameter erkennen **********/

 if(argc<3)
   {
#ifndef ENGLISH
    fprintf(stderr,"zu wenige Parameter\n");
#else
    fprintf(stderr,
     "Usage: genreg n deg [girth] [-b] [-a|-s|-g|-u] [stdout] [-e] [-m # #]\n");
#endif
    return 0;
   }

 n=(SCHAR)atoi(*++argv);
 k=(SCHAR)atoi(*++argv);
 argc-=2;

 if(argc>1&&(t=atoi(*(argv+1)))>0)
   {
    argv++;argc--;
   }
 else
    t=3;

 if(inputerror())return 1;


/********** Optionen erkennen **********/

 while(--argc>0)
      {
       ++argv;
       if(!strcmp(*argv,"-e"))
	  efile=1;
       else
       if(!strcmp(*argv,"-c"))
	 {
/*********************gb-mod-start*********************
This doesn't take into account that there may be options following,
so I deleted it

          if(argc>1)
	     count=atoi(*(argv+1));
	  if(count>0)
	    {
	     argv++;
	     argc--;
	    }
	  else 
*********************gb-mod-end*********************/

	    count=COUNTSTEP;
	 }
       else
       if(!strcmp(*argv,"-max"))
	 { 
          if(argc>1)
	     mid_max=atoi(*++argv);argc--;
	  if(mid_max==0)
	    {
#ifndef ENGLISH
	     fprintf(stderr,"%s : kein erlaubter Wert fuer Option -max\n",*argv);
#else
	     fprintf(stderr,"%s : value not permitted for option -max\n",*argv);
#endif
	     return(1);
	    }
	 }
       else
       if(!strcmp(*argv,"-split"))
	 {
          if(argc>1)
	     splitlevel=atoi(*++argv);argc--;
	  if(splitlevel==0)
	    {
#ifndef ENGLISH
	     fprintf(stderr,"%s : kein erlaubter Wert fuer Option -split\n",*argv);
#else
	     fprintf(stderr,"%s : value not permitted for option -split\n",*argv);
#endif
	     return(1);
	    }
	 }
       else
       if(!strcmp(*argv,"-s"))
	 {
	  if(argc>1)
            {
             if(!strcmp(*(argv+1),"stdout"))
	        codestdout=storeall=1;
	     else
	        tostore=atoi(*(argv+1));
            }
	  if(tostore>0||codestdout)
	    {
	     argv++;
	     argc--;
	    }
	  else storeall=1;
	 }
/*********************g6-mod-start*********************/
       else
       if(!strcmp(*argv,"-g"))
         {
          if(argc>1)
            {
             if(!strcmp(*(argv+1),"stdout"))
                g6stdout=g6all=1;
             else
                tog6=atoi(*(argv+1));
            }
          if(tog6>0||g6stdout)
            {
             argv++;
             argc--;
            }
          else g6all=1;
         }
       else
       if(!strcmp(*argv,"-u"))
	    nooutput=1;
/*********************g6-mod-end*********************/
       else
       if(!strcmp(*argv,"-a"))
	 {
          if(argc>1)
            {  
             if(!strcmp(*(argv+1),"stdout"))
	        asciistdout=printall=1;
	     else
	        toprint=atoi(*(argv+1));
            }
	  if(toprint>0||asciistdout)
	    {
	     argv++;
	     argc--;
	    }
	  else printall=1;
	 }
       else
       if(!strcmp(*argv,"-m"))
	 {
          if(argc>1)
            {
	     jobnr=atoi(*++argv);
             argc--;
            }
          if(argc>1)
            {
	     jobs=atoi(*++argv);
	     argc--;
            }
	  if(jobs==0||jobnr==0||jobnr>jobs)
	    {
#ifndef ENGLISH
	     fprintf(stderr,"%d %d : keine erlaubten Werte fuer Option -m\n",jobnr,jobs);
#else
	     fprintf(stderr,"%d %d : bad values for option -m\n",jobnr,jobs);
#endif
	     return 1;
	    }
	 }
/*********************gb-mod-start*********************/
       else
       if(!strcmp(*argv,"-b"))
	 { bipartite=1; 
	   biplimit=n/2; }
/*********************gb-mod-end*********************/
       else
	 {
#ifndef ENGLISH
	  fprintf(stderr,"%s : keine erlaubte Option\n",*argv);
#else
	  fprintf(stderr,"%s : unknown option\n",*argv);
#endif
	  return 1;
	 }
      }


/********** -m Modus **********/

 if(jobs>0)
   {
    openfiles();

    if(!efile)
       ergfile=stderr;
    else
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(erg,"%d%d_%d_%d-%d.b.erg",n/10,n%10,k,t,jobnr);
	  else
/*********************gb-mod-end*********************/
       sprintf(erg,"%d%d_%d_%d-%d.erg",n/10,n%10,k,t,jobnr);
       ergfile=fopen(erg,"w");
      }

    generate();

    /* BDM: These had arguments but the functions have no parameters. */
    // closefiles(anz);
    // movefiles(anz);
    closefiles();
    movefiles();

    if(efile)
      {
       fclose(ergfile);
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d#%d.b.erg",n/10,n%10,k,t,jobnr);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d#%d.erg",n/10,n%10,k,t,jobnr);
       remove(neu);
       rename(erg,neu);
      }
   }
 else


/********** Normalmodus **********/

   {
    if(codestdout)
       lstfile=stdout;
    else
    if(storeall==1||tostore>0)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(lst,"%d%d_%d_%d-U.b.%s",n/10,n%10,k,t,APP);
	  else
/*********************gb-mod-end*********************/
       sprintf(lst,"%d%d_%d_%d-U.%s",n/10,n%10,k,t,APP);
       lstfile=fopen(lst,"wb");
      }
/*********************g6-mod-start*********************/
    if(g6stdout)
       g6file=stdout;
    else
    if(g6all==1||tog6>0)
      {
/*********************gb-mod-start*********************/
        if (bipartite) sprintf(g6name,"%d%d_%d_%d-U.b.%s",n/10,n%10,k,t,"g6");
          else
/*********************gb-mod-end*********************/
       sprintf(g6name,"%d%d_%d_%d-U.%s",n/10,n%10,k,t,"g6");
       g6file=fopen(g6name,"w");
      }

/*********************g6-mod-end*********************/

    if(asciistdout)
       autfile=stdout;
    else
    if(printall==1||toprint>0)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(aut,"%d%d_%d_%d-U.b.asc",n/10,n%10,k,t);
	  else
/*********************gb-mod-end*********************/
       sprintf(aut,"%d%d_%d_%d-U.asc",n/10,n%10,k,t);
       autfile=fopen(aut,"w");
      }

    if(!efile)
       ergfile=stderr;
    else
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(erg,"%d%d_%d_%d-U.b.erg",n/10,n%10,k,t);
	  else
/*********************gb-mod-end*********************/
       sprintf(erg,"%d%d_%d_%d-U.erg",n/10,n%10,k,t);
       ergfile=fopen(erg,"w");
      }

    generate();

    if((storeall==1||tostore>anz)&&!codestdout)
      {
       fclose(lstfile);
       if(anz==0)remove(lst);
      }

    if((printall==1||toprint>anz)&&!asciistdout)
      {
       fclose(autfile);
       if(anz==0)remove(aut);
      }

/*********************g6-mod-start*********************/
    if((g6all==1||tog6>anz)&&!g6stdout)
      {
       fclose(g6file);
       if(anz==0)remove(g6name);
      }
/*********************g6-mod-end*********************/

    if(efile)
      {
       fclose(ergfile);
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d.b.erg",n/10,n%10,k,t);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d.erg",n/10,n%10,k,t);
       remove(neu);
       rename(erg,neu);
      }

    if((storeall==1||tostore>=anz)&&anz>0&&!codestdout)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d.b.%s",n/10,n%10,k,t,APP);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d.%s",n/10,n%10,k,t,APP);
       remove(neu);
       rename(lst,neu);
      }

    if((printall==1||toprint>=anz)&&anz>0&&!asciistdout)
      {
/*********************gb-mod-start*********************/
	if (bipartite) sprintf(neu,"%d%d_%d_%d.b.asc",n/10,n%10,k,t);
	  else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d.asc",n/10,n%10,k,t);
       remove(neu);
       rename(aut,neu);
      }

/*********************g6-mod-start*********************/
    if((g6all==1||tog6>=anz)&&anz>0&&!g6stdout)
      {
/*********************gb-mod-start*********************/
        if (bipartite) sprintf(neu,"%d%d_%d_%d.b.%s",n/10,n%10,k,t,"g6");
          else
/*********************gb-mod-end*********************/
       sprintf(neu,"%d%d_%d_%d.%s",n/10,n%10,k,t,"g6");
       remove(neu);
       rename(g6name,neu);
      }
/*********************g6-mod-end*********************/

   }

 return 0;
}
