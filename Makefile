BINNAME   = wl_term

# compiler
CC       = gcc
# compiling flags
CFLAGS   = -Wall -I$(INCDIR) -g

# linker
LINKER   = gcc -o
# linking flags here
LFLAGS   = -lm -lwayland-client -lwayland-egl -lcairo -lEGL -lxkbcommon

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
INCDIR   = include
#-------------------------------------------

SOURCES  := $(wildcard $(SRCDIR)/*.c)
INCLUDES := $(wildcard $(INCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
rm       = rm -f

$(BINDIR)/$(BINNAME): $(OBJECTS)
	@$(LINKER) $@ $(LFLAGS) $(OBJECTS)
	@echo "Linking of "$@" complete!"

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.c $(INCLUDES)
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo "Compiled "$<""

.PHONY: clean
clean:
	@$(rm) $(OBJECTS)
	@echo "Cleanup complete!"

.PHONY: remove
remove: clean
	@$(rm) $(BINDIR)/$(BINNAME)
	@echo "Executable removed!"
