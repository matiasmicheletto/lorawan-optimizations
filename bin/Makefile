CPP      = g++
CFLAGS   = -Wall -pthread
SRCDIR   = src
OBJDIR   = obj
LIBDIR   = $(SRCDIR)/lib

TARGETS         = gpprs datgen
GPPRS_SOURCES   = $(wildcard $(SRCDIR)/gpprs.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
DATGEN_SOURCES  = $(wildcard $(SRCDIR)/datgen.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
GPPRS_OBJECTS   = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(GPPRS_SOURCES))
DATGEN_OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(DATGEN_SOURCES))
HEADERS         = $(wildcard $(LIBDIR)/**/*.h)

INCLUDES = -I$(LIBDIR) -I$(LIBDIR)/util -I$(LIBDIR)/model -I$(LIBDIR)/optimization

all: $(TARGETS)

gpprs: $(GPPRS_OBJECTS)
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@

datgen: $(DATGEN_OBJECTS)
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -rf $(OBJDIR) ./gpprs ./datgen

.PHONY: all clean