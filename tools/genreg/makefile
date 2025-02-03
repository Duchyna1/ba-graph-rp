CC = gcc -DENGLISH
OPTS = -c -march=native -O3
STDH = gendef.h
OBJS = genreg.o main.o optmid.o

genreg : $(OBJS)
	  cc -o $@ $(LDOPTS) $(OBJS) $(LIBS)

genreg.o : genreg.c $(STDH)
	    $(CC) $(OPTS) genreg.c

main.o : main.c $(STDH)
	  $(CC) $(OPTS) main.c

optmid.o : optmid.c $(STDH)
	    $(CC) $(OPTS) optmid.c

manual : manual.eng.tex
	pdflatex manual.eng.tex

tarfile :
	mkdir GENREG
	make manual
	cp genreg.c main.c optmid.c readscd.c manual.eng.tex \
	   manual.eng.pdf gendef.h README makefile GENREG
	tar cvf genreg.tar GENREG
	gzip genreg.tar
	rm -r GENREG
