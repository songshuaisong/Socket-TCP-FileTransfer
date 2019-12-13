CC = gcc
CFLAGS = -g -O -Wall -Werror


TOPDIR := $(PWD)
OBJDIR := $(TOPDIR)/obj

BINDIRSER := $(TOPDIR)/bin-server
BINDIRCLT := $(TOPDIR)/bin-client

BINSER := server
BINCLT := client
 
SUBDIR := server client obj
OBJLINK := --std=c99
 
export CC TOPDIR OBJDIR BINDIRSER BINDIRCLT BINCLT BINSER OBJLINK 
 
all:CHECKDIR $(SUBDIR)
	@echo "*****************************************************************************"
	@echo "*                                                                           *"
	@echo "*                 Congratulations! Compile completed!!!                     *"
	@echo "*       Executable file name is : $(BINCLT) $(BINSER)"
	@echo "*  Executable file in directory : $(BINDIRSER) $(BINDIRCLT)"
	@echo "*                                                                           *"
	@echo "*****************************************************************************"
CHECKDIR:
	@echo "*****************************************************************************"
	@echo "*                                                                           *"
	@echo "* Create subfolders:                                                        *"
	mkdir -p $(SUBDIR) 
	mkdir -p $(BINDIRSER)
	mkdir -p $(BINDIRCLT)
	@echo "*                                                                           *"
	@echo "*               Successful Creation of subfolders!                          *"
	@echo "*                                                                           *"
	@echo "*****************************************************************************"
$(SUBDIR):RUN
	make -C $@
	@echo "*****************************************************************************"
RUN:
	@echo "*****************************************************************************"
	@echo "*                                                                           *"
	@echo "* All subdirectories are:                                                   *"
	@echo "*    "$(SUBDIR)
	@echo "* All links are:                                                            *"
	@echo "*    "$(OBJLINK)
	@echo "*                                                                           *"
	@echo "*****************************************************************************"
	@echo "*                                                                           *"
	@echo "*                            begin Compile                                  *"
	@echo "*                                                                           *"
	@echo "*****************************************************************************"

clean:
	rm -rf $(OBJDIR)/*.o $(BINDIR) $(BINDIRSER) $(BINDIRCLT)

test:
	@echo successed!
		
help:
	@echo "*********************************** help ************************************"
	@echo "*                                                                           *"
	@echo "*                            Option Description                             *"
	@echo "*                                                                           *"
	@echo "*********************************** help ************************************"
