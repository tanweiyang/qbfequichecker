FILES :=    maintestsuite.cpp   \
            my_tut_reporter.cpp \
            logic_t_test.cpp    \
            truefalsenode_test.cpp  \
            variablenode_test.cpp   \
            operatorandnode_test.cpp\
            operatornotnode_test.cpp\
            operatorornode_test.cpp \
            operatorxornode_test.cpp \
            formulanodefactory_test.cpp \
            qbfequichecker_test.cpp \
            miter_test.cpp	\
            rcptr_test.cpp	

OBJDIR := ./obj
SRCDIR := ./src
LIBDIR := ../qbfequichecker/lib ../depqbf #../quantor_3_0 ../picosat-951 ../CirQit3.1.7a 
INCDIR := ./include ../qbfequichecker/include
BINDIR := ./bin
SYSDIR := ./include/tut
OBJS := $(FILES:.cpp=.o)
CC := g++
CC_FLAGS := -Wall -W -Wno-unused-function $(addprefix -I,$(INCDIR)) -g
LIB := qbfequichecker
THIRD_PARTY_LIB := qdpll #quantor picosat cirqit
OUTPUT := testqbfequichecker
AR := ar
AR_FLAGS := rv
DEP_FILES := $(FILES:.cpp=.d)
LD_FLAGS :=

vpath %.cpp $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.a $(LIBDIR)
vpath %.o $(OBJDIR)
vpath %.d $(OBJDIR)

# $(call make-depend,source-file,object-file,depend-file)
define make-depend
	$(CC)   -MMD         \
 	        -MF $3       \
            -MP          \
            -MT $2       \
            $(CC_FLAGS) -E \
			$1 > /dev/null
endef


.PHONY : all clean

all: $(BINDIR)/$(OUTPUT)


clean:
	rm -f $(BINDIR)/$(OUTPUT)
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d


$(BINDIR)/$(OUTPUT) : lib$(LIB).a $(OBJS)
	mkdir -p $(BINDIR)
	$(CC) $(addprefix $(OBJDIR)/,$(OBJS)) $(addprefix -L,$(LIBDIR)) -l$(LIB) $(addprefix -l,$(THIRD_PARTY_LIB)) -o $@


%.o : %.cpp
	mkdir -p $(OBJDIR)
	$(call make-depend,$<,$(OBJDIR)/$@,$(OBJDIR)/$(subst .o,.d,$@))
	$(CC) $(CC_FLAGS) -c $< -o $(OBJDIR)/$@


ifneq "$(MAKECMDGOALS)" "clean"
  -include $(addprefix $(OBJDIR)/,$(DEP_FILES))
endif


