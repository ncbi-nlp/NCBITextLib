SRC_DIR=./
TRASHFILES = *.o *~ *.bak core	
LIB_INC=-I../include
#.KEEP_STATE:
libops.a: FBase.o XPost.o Strset.o Hash.o runn.o
	ar rus $@ $?

%.o: $(SRC_DIR)/%.C
	g++ -std=c++11 -c -gdwarf-2 $< -o $@ $(LIB_INC)

clean: rm -f $(TRASHFILES)

