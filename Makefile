CC = gcc
CFLAGS = -Wall 

INCDIR = include 
OBJDIR = obj
BINDIR = bin

BINNAME = tserv

TSERVDIR = tserv
TSERVSRC = $(wildcard $(TSERVDIR)/*.c)
TSERVOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(TSERVSRC))

# future use
# TCLIDIR =
# TCLISRC =
# TCLIOBJ =

# TDBDIR =
# TDBSRC = 
# TDBOBJ =


$(OBJDIR)/$(TSERVDIR)/%.o: $(TSERVDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TSERVDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

tserv: $(TSERVOBJ) $(BINDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(BINNAME) $(TSERVOBJ)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(TSERVDIR):
	mkdir $(OBJDIR)/$(TSERVDIR)

$(BINDIR):
	mkdir $(BINDIR)

all: tserv

clean:
	rm -fr $(OBJDIR) $(BINDIR) 

