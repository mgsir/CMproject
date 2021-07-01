CC = g++
CFLAGS = -g -Wno-write-strings -Wno-unused-variable
#CFLAGS = -O2
#CFLAGS = -O2 -Wall

#Header 
PUBINC = -I../../public/c
ORAINC = -I$(ORACLE_HOME)/include
OICINC = ../../public/c/_ooci.h

#Libray 
ORALIB = -I$(ORACLE_HOME)/lib
ORALIBS = -lclntsh

#Cpp 
PUBCPP = ../../public/c/_public.cpp
OICCCP = ../../public/c/_ooci.cpp

.PHONY: crtsurfdate psurfdata
all: crtsurfdate psurfdata

crtsurfdate: crtsurfdate.cpp 
	$(CC) $(CFLAGS) -o crtsurfdate $(PUBINC) $(PUBCPP) crtsurfdate.cpp -lm -lc
	cp -f crtsurfdate  ../bin/.
psurfdata: psurfdata.cpp
	$(CC) $(CFLAGS) -o psurfdata $(PUBINC) $(ORAINC)  $(ORALIB) $(ORALIBS) $(OICINC) $(PUBCPP) $(OICCCP) psurfdata.cpp -lm -lc
	cp -f psurfdata ../bin/.
clean: 
	rm crtsurfdate psurfdata
	



