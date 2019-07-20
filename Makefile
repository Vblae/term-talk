CC = gcc
CFLAGS = -Wall -Werror

INCDIR = include 
OBJDIR = obj
BINDIR = bin

TSERV = tserv 
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

CONFDIR = config
CONFSRC = $(wildcard $(CONFDIR)/*.c)
CONFOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(CONFSRC))

$(OBJDIR)/$(TSERVDIR)/%.o: $(TSERVDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TSERVDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(CONFDIR)/%.o: $(CONFDIR)/%.c $(OBJDIR) $(OBJDIR)/$(CONFDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

tserv: $(CONFOBJ) $(TSERVOBJ) $(BINDIR)
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TSERV) $(TSERVOBJ) $(CONFOBJ)

config: $(CONFOBJ) $(OBJDIR)/$(CONFDIR)

$(OBJDIR):
	mkdir $(OBJDIR)

$(OBJDIR)/$(TSERVDIR):
	mkdir $(OBJDIR)/$(TSERVDIR)

$(OBJDIR)/$(CONFDIR):
	mkdir $(OBJDIR)/$(CONFDIR)

$(BINDIR):
	mkdir $(BINDIR)

all: config tserv

clean:
	rm -fr $(OBJDIR) $(BINDIR) 

