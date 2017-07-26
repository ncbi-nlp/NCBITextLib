LIBPATH=../lib
INCPATH=../include
TRASHFILES = $(LIBDIR)/*.o *~ *.bak core	

.KEEP_STATE:
%: %.o ../lib/libText.a
	g++ $(OS) -o $@ $< -L$(LIBPATH) -lText -lpcre -D_POSIX_C_SOURCE=199506L -lpthread

.SUFFIXES:
%.o: %.C
	g++ -std=c++11 -c $(OS) -I$(INCPATH) $< -o $@

all:  make_doc make_xpost run_ThemX run_HubeX run_BayeX find_neighbors

clean: rm -f $(TRASHFILES)
