LIBPATH=../lib
INCPATH=../include
TRASHFILES = $(LIBDIR)/*.o *~ *.bak core	

.KEEP_STATE:
$(N): $(N).o ../lib/libops.a
	g++ -pg $(OS) -o $(N) $(N).o -L$(LIBPATH) -lops -lpcre -D_POSIX_C_SOURCE=199506L -lpthread

%: %.o ../lib/libops.a
	g++ -pg $(OS) -o $@ $< -L$(LIBPATH) -lops -lpcre -D_POSIX_C_SOURCE=199506L -lpthread

.SUFFIXES:
%.o: %.C
	g++ -std=c++11 -c $(OS) -I$(INCPATH) $< -o $@

clean: rm -f $(TRASHFILES)
