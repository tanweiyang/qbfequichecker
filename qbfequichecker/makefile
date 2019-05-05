FILES :=    logic_t.cpp         \
            formulanode.cpp     \
            formulaparserexception.cpp  \
            formulanodefactory.cpp  \
            operatornode.cpp    \
            binaryoperatornode.cpp  \
            unaryoperatornode.cpp  \
            operatorandnode.cpp \
            operatornotnode.cpp \
            operatorornode.cpp  \
            operatorxornode.cpp \
            variablenode.cpp    \
            literalnode.cpp     \
            truenode.cpp        \
            falsenode.cpp       \
            clausenode.cpp      \
            cnfnode.cpp         \
            solveradaptorbase.cpp	\
            quantoradaptor.cpp  \
            cirqitadaptor.cpp	\
            depqbfadaptor.cpp	\
            qbfequichecker_common.cpp	\
            rcptr.cpp			\
            qbfequichecker.cpp  \
            miter.cpp			\
            formulaparser.cpp   \
            verilogparser.cpp


OBJDIR := ./obj
SRCDIR := ./src
THIRD_PARTY_LIBDIR := ../quantor_3_0 ../CirQit3.1.7a ../depqbf
INCDIR := ./include
THIRD_PARTY_INCDIR := ../quantor_3_0 ../CirQit3.1.7a ../depqbf
OUTDIR := ./lib
OBJS := $(FILES:.cpp=.o)
CC := g++
CC_FLAGS:=-Wall -W $(addprefix -I,$(INCDIR)) $(addprefix -isystem,$(THIRD_PARTY_INCDIR)) -g
LIB := libqbfequichecker.a
AR := ar
AR_FLAGS := rv
DEP_FILES := $(FILES:.cpp=.d)

vpath %.cpp $(SRCDIR)
vpath %.h $(INCDIR)
vpath %.a $(OUTDIR)
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

all: $(LIB)


clean: 
	rm -f $(OUTDIR)/*.a
	rm -f $(OBJDIR)/*.o
	rm -f $(OBJDIR)/*.d


$(LIB) : $(OBJS)
	mkdir -p $(OUTDIR)
	$(AR) $(AR_FLAGS) $(OUTDIR)/$@ $(addprefix $(OBJDIR)/,$(^F))


%.o : %.cpp
	mkdir -p $(OBJDIR)
	$(call make-depend,$<,$(OBJDIR)/$@,$(OBJDIR)/$(subst .o,.d,$@))
	$(CC) $(CC_FLAGS) -c $< -o $(OBJDIR)/$@


ifneq "$(MAKECMDGOALS)" "clean"
  -include $(addprefix $(OBJDIR)/,$(DEP_FILES))
endif

%.h :

