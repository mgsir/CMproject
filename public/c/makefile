CC = g++
CFLAGS = -g -Wno-write-strings -Wno-unused-variable
# CFLAGS = -O2
# CFLAGS = -O2 -Wall



# oracle header path
ORAINCL = -I$(ORACLE_HOME)/include

# oracle libary path
ORALIB =  -L$(ORACLE_HOME)/lib
# oci library
ORALIBS = -lclntsh

.PHONY = ftpgetfile

all: ftpgetfile 


ftplib.a: 
	gcc -c -o ftplib.a ftplib.c

ftpgetfile: ftpgetfile.cpp  ftplib.a
	$(CC) $(CFLAGS) -o ftpgetfile _public.cpp  _ftp.cpp ftpgetfile.cpp ftplib.a -lm -lc
	cp -f ftpgetfile ../bin/.

clean: ftpgetfile
	rm ftpgetfile
