#
# 'make depend' uses makedepend to automatically generate dependencies 
#               (dependencies are added to end of Makefile)
# 'make'        build executable file 'mycc'
# 'make clean'  removes all .o and executable files
#
mkfile_path := $(abspath $(lastword $(MAKEFILE_LIST)))
current_dir := $(notdir $(patsubst %/,%,$(dir $(mkfile_path))))

# define the C compiler to use
CC = gcc
CXX = g++

# define any compile-time flags
CFLAGS = -O0 -g3 -Wall -c -fmessage-length=0 -std=c++11 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"

# define any directories containing header files other than /usr/include
INCLUDES = -I$(current_dir)/include -I/usr/include/glib-2.0  -I/usr/lib/arm-linux-gnueabihf/glib-2.0/include

# define library paths in addition to /usr/lib
LFLAGS = 

# define any libraries to link into executable:
LIBS = -lglib-2.0

# define the executable file 
MAIN = TTS

#
# The following part of the makefile is generic; it can be used to 
# build any executable just by changing the definitions above and by
# deleting dependencies appended to the file from 'make depend'
#

.PHONY: depend clean
all:	$(MAIN)
	@echo  $(current_dir)

$(MAIN): $(OBJS) 
	$(CXX) -o $(MAIN) $(OBJS) $(CFLAGS) $(INCLUDES) $(LFLAGS) $(LIBS)

# this is a suffix replacement rule for building .o's from .c's
# it uses automatic variables $<: the name of the prerequisite of
# the rule(a .c file) and $@: the name of the target of the rule (a .o file) 
# (see the gnu make manual section about automatic variables)
.c .o:
	$(CXX) $(CFLAGS) $(INCLUDES) -c $<  -o $@

clean:
	$(RM) *.o *~ $(MAIN)

depend: $(SRCS)
	makedepend $(INCLUDES) $^

# DO NOT DELETE THIS LINE -- make depend needs it
