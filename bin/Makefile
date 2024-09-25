CPP      = g++
CFLAGS   = -Wall -pthread 
LDFLAGS  = -lgsl -lgslcblas -lm
SRCDIR   = src
OBJDIR   = obj
LIBDIR   = $(SRCDIR)/lib

TARGETS         = datgen gpprs greedy ga

DATGEN_SOURCES  = $(wildcard $(SRCDIR)/datgen.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
GPPRS_SOURCES   = $(wildcard $(SRCDIR)/gpprs.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
GREEDY_SOURCES  = $(wildcard $(SRCDIR)/greedy.cpp) $(wildcard $(LIBDIR)/**/*.cpp)
GA_SOURCES  = $(wildcard $(SRCDIR)/ga.cpp) $(wildcard $(LIBDIR)/**/*.cpp)

GPPRS_OBJECTS   = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(GPPRS_SOURCES))
DATGEN_OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(DATGEN_SOURCES))
GREEDY_OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(GREEDY_SOURCES))
GA_OBJECTS  = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(GA_SOURCES))

HEADERS         = $(wildcard $(LIBDIR)/**/*.h)

INCLUDES = -I$(LIBDIR) -I$(LIBDIR)/util -I$(LIBDIR)/model -I$(LIBDIR)/optimization

ifdef VERBOSE
    CFLAGS += -DVERBOSE=true
endif

all: $(TARGETS)

gpprs: $(GPPRS_OBJECTS)
	@echo "Compiling gpprs..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "gpprs compiled"

datgen: $(DATGEN_OBJECTS)
	@echo "Compiling datgen..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "datgen compiled"

greedy: $(GREEDY_OBJECTS)
	@echo "Compiling greedy..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "greedy compiled"

ga: $(GA_OBJECTS)
	@echo "Compiling ga..."
	$(CPP) $(CFLAGS) $(INCLUDES) $^ -o $@ $(LDFLAGS)
	@echo "ga compiled"

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp $(HEADERS)
	@mkdir -p $(@D)
	@echo "Compiling $<..."
	$(CPP) $(CFLAGS) $(INCLUDES) -c $< -o $@ $(LDFLAGS)

clean:
	rm -rf $(OBJDIR) ./gpprs ./greedy ./datgen ./ga

.PHONY: all clean