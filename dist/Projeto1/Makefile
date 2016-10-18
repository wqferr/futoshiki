# By: Lucas "Iron" Alexandre Soares && William "VVilliam" Quelho Ferreira
# USP BCC 2015 23/09/2015
# 
# How to make Makefile (without cmake)
# 
# The -c flag says to generate the object file
# - means ignore error messages. If rm fails, it just goes on with the next command.
# $@ gets the file named on the left side of the :
# $^ gets the files named on the right side of the :
# $< is the first item in the dependencies list
# $* is used to get the name of the file (path included)
# @ before just means "DON'T PRINT THIS COMMAND IN COMMAND LINE"
# printf "%s" means "PRINT: "
# @printf "%s" means the world will explode
#   (not really, it just means the command is not printed, its output still is)
# 
# make should normaly be in project directory and access includes and libs relatively from there
# 
# DEFAULT DIRECTORIES TREE:
#    project/
#        build/
#        dist/
#        etc/
#            cases/
#                in/
#                out/
#                exp/
#        include/
#        lib/
#        src/
#            .obj/
#



# =======================================================
# |                                                     |
# |       SET THIS IF YOU WANT THE MAKEFILE TO WORK     |
# |                                                     |
                    NAME := Futoshiki
# |                                                     |
# =======================================================
# 
# Directories
# I guess it's ok if you touch these, if you really want to change the
# project's directory tree

INCDIR := include
LIBDIR := lib
BLDDIR := build
SRCDIR := src
OBJDIR := $(SRCDIR)/.obj
OUTPUT := $(BLDDIR)/$(NAME)
DSTDIR := dist
ETCDIR := etc
CASEDIR := $(ETCDIR)/cases
CASEIN := $(CASEDIR)/in
CASEOUT := $(CASEDIR)/out
CASEEXP := $(CASEDIR)/exp

# 
#
# Library links (example: -lm for math lib).
LIBS :=
 
# The compiler to be used
CC := gcc

# The flags to be passed to the compiler by default
CFLAGS := -Wno-unused-result -I./$(INCDIR)

# Flags to be added after <CFLAGS> when compiling
# in debug mode (i.e. <over> is defined). See below.
DBGFLAGS := -g -Wall -Wextra -O0

# Default flags when over is "valgrind" 
VALGRINDFLAGS := -v --leak-check=full --show-leak-kinds=all --read-var-info=yes --track-origins=yes
 
# IF YOU NEED HELP, READ BELOW
# 
# 
# After setting the project name, run "make create"
# (without quotes) to create the project structure.
# All of your source files (*.c) should be in <SRCDIR>*,
# all your header files (*.h) should be in <INCDIR>.
# When you run "make all" or just "make" this will 
# create an executable file in <BLDDIR>, and the object
# files will be put in <OBJDIR>, compiling only
# the new or outdated files. Any static object files
# (*.so) are to be put in <LIBDIR>.
# *file whose name contain whitespaces are not supported
# as of now
#
# "make run" will execute the output, while "make go" is
# equivalent to "make all run". "make clean" will remove
# all compiled or precompiled files from the project.
# "make rebuild" is equivalent
# to "make all", except that it compiles ALL source files
# again, regardless of wether or not they were outdated.
# "make list" will recursively list all files in any subdirec-
# tory of the main one.
# 
# To compress the project, run "make <extension>", where
# <extension> is the desired output extension.
# For example, to make a .tar.gz out of a project named
# a_cool_thing, run "make .tar.gz", and a_cool_thing.tar.gz
# will appear in <DSTDIR>.
# It is worth noting that compiled and precompiled files are
# removed prior to compression, so you will need to recompile
# your project after running the target.
# 
# The <ETCDIR> directory is completely ignored by this Makefile.
# Its intended use is to store project specifications and
# test cases.
#
# To use a debugger or overlay (ALL HAIL VALGRIND), simply add
# over=<OVERLAY> after the targets. For example, the statement:
# "make rebuild run over=gdb" would recompile all source
# files and run it with gdb as if the command was
# make rebuild run && gdb <OUTPUTFILE>.
# If an overlay is specified when a target includes compilation,
# the files compiled during this iteration will be processed with
# <DBGFLAGS>.
# If the overlay is valgrind specifically, then some useful
# flags are added by default. To change that, simply erase
# everything after the ":=" sign in the line that defines
# the variable VALGRINDFLAGS.
#
# To add arguments to your program, add args="<ARG1> <ARG2> ... <ARGN>"
# to the command after the targets. The order of "args" and "over" is
# irrelevant, and nor does it matter wether or not both are present.
#
# To use the test case system, add the variable "case" to the make command,
# i.e. put this somewhere: case="case_file_name", where case_file_name is the name
# of the input file in <CASEIN>. The output of your program is automatically saved
# in <CASEOUT>, and compared to the file of the same name as the input in <CASEEXP>.
# If you never used Vim or Vimdiff, type :qa and press enter to exit the comparison
# mode.







# FROM THIS POINT ON, LEAVE IT TO MAKE





























# THIS IS YOUR LAST CHANCE TO TURN BACK
























# OK, if you came down here, anything you touch is your responsibility





ifneq ($(MAKECMDGOALS),create)
# Every directory in the project (excluding the main one)
DIRS := $(shell find */ -type d)

# Reserved variable for make that tells it which directories to search
# for a dependency in.
VPATH := $(subst  ,:,$(shell find ./$(INCDIR) ./$(SRCDIR) -type d))

# Can use any debbuger (like gdb) or overlay (like valgrind) (though gdb is an overlay too)
OVERLAY :=
OVFLAGS := 
ARGS :=


# Used for compressing the project
ZIPDIRS := $(shell find ./* -type d | grep -v ./$(ETCDIR) | grep -v ./$(DSTDIR) | grep -v ./$(BLDDIR)/ | grep -v ./$(OBJDIR)/)

# This will search for 'anything' terminating in .c inside SRCDIR
# If there's a file named thing.c in src/some/file/path/, then SRC
# will contain src/some/file/path/thing.c
SRC := $(shell find $(SRCDIR) -name *.c)

# For every word in SRC, replace SRCDIR/path/to/file/here/____.c by OBJDIR/____.o
OBJ := $(foreach SRCFILE,$(SRC),$(OBJDIR)/$(lastword $(subst /, ,$(SRCFILE:%.c=%.o))))

# Find all .h dependencies
DEPS := $(shell find $(INCDIR) -name *.h)

endif


# Targets that aren't actual files (not needed, but can guarantee
# they'll always run even if there's a file with the same name or if they
# aren't outdated)
# [...Never mind]
.PHONY: all run go .zip .tar.gz clean list create rebuild

# Targets whose errors are to be ignored
.IGNORE: clean .zip .tar.gz


# <NAME> only contains whitespaces
ifeq ($(strip $(NAME)),)
$(error "No project name provided (open this Makefile and set NAME)")
endif

# If you run make [target] over=____ where ____ is anything
ifdef over
# Create symbol tables when compiling
CFLAGS += $(DBGFLAGS)
OVERLAY := $(over)

ifeq ($(over),valgrind)
# Add useful valgrind flags
OVFLAGS += $(VALGRINDFLAGS)
endif
endif

ifdef args
ARGS := $(args)
endif

ifdef case
CASE := < $(CASEIN)/$(case) > $(CASEOUT)/$(case)
DIFFCMD := vimdiff -M $(CASEEXP)/$(case) $(CASEOUT)/$(case)
endif



#
#    TARGETS
#

# Compile directives

all: $(OUTPUT)

clean:
	@printf "Cleaning object files..."
	@rm -f $(OBJDIR)/*.o
	@printf "\t\tDone.\n"
	@printf "Removing backup files..."
	@rm -r $(shell find -name *~)
	@printf "\t\tDone.\n"
	@printf "Cleaning output file..."
	@if [ -e $(OUTPUT) ]; then rm $(OUTPUT); fi;
	@printf "\t\tDone.\n"
	@clear
	@clear

$(OBJDIR)/%.o: %.c $(DEPS)
	@printf "Building $*..."
	@$(CC) -c -o $@ $(shell find $(SRCDIR) -name $*.c) $(CFLAGS)
	@printf "\t\tDone.\n"

$(OUTPUT): $(OBJ)
	@printf "\nLinking object files..."
	@$(CC) -o $@ $^ $(CFLAGS) $(LIBS)
	@printf "\t\tDone.\n"

# Utility directives

# Recompile everything, not just modified files
rebuild: clean all

# Recursively lists all directories and their files,
# along with their respective sizes
list:
	@clear
	@clear
	@find . -type f

# Run directives
run:
	@$(OVERLAY) $(OVFLAGS) ./$(OUTPUT) $(ARGS) $(CASE)
	@$(DIFFCMD)

# Compile then run
go: all run

.tar.gz: clean
	@printf "Compressing files...\n\n"
	@tar -zcvf $(DSTDIR)/$(NAME).tar.gz Makefile $(ZIPDIRS:./%=%)

	@printf "\nDone.\n"

.zip: clean
	@printf "Compressing files...\n\n"
	@zip $(DSTDIR)/$(NAME).zip Makefile $(shell find $(ZIPDIRS) | grep -v ./$(DSTDIR) | grep -v ./$(ETCDIR))
	@printf "\nDone.\n"

# First directive you want to run. Sets up all directories
# correctly for this Makefile
create:
	@-mkdir $(NAME) 
	@printf "Project directory created.\n"

	@-mkdir $(NAME)/$(SRCDIR)
	@printf "Source directory created.\n"

	@-mkdir $(NAME)/$(INCDIR)
	@printf "Include directory created.\n"

	@-mkdir $(NAME)/$(LIBDIR)
	@printf "Library directory created.\n"

	@-mkdir $(NAME)/$(BLDDIR)
	@printf "Build directory created.\n"

	@-mkdir $(NAME)/$(OBJDIR)
	@printf "Object directory created.\n"

	@-mkdir $(NAME)/$(DSTDIR)
	@printf "Distribution directory created.\n"

	@-mkdir $(NAME)/$(ETCDIR)
	@printf "Misc directory created.\n"

	@-mkdir $(NAME)/$(CASEDIR)
	@-mkdir $(NAME)/$(CASEIN)
	@-mkdir $(NAME)/$(CASEOUT)
	@-mkdir $(NAME)/$(CASEEXP)
	@printf "Case directories created.\n"

	@mv ./Makefile ./$(NAME)
	@printf "\t\tDone.\n"
