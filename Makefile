cC = gcc
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

UTILDIR = util
UTILSRC = $(wildcard $(UTILDIR)/*.c)
UTILOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(UTILSRC))

$(OBJDIR)/$(TSERVDIR)/%.o: $(TSERVDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TSERVDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(CONFDIR)/%.o: $(CONFDIR)/%.c $(OBJDIR) $(OBJDIR)/$(CONFDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(UTILDIR)/%.o: $(UTILDIR)/%.c $(OBJDIR) $(OBJDIR)/$(UTILDIR) 
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

tserv:  $(BINDIR) $(TSERVOBJ) $(CONFOBJ) $(UTILOBJ)
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TSERV) $(TSERVOBJ) $(CONFOBJ) $(UTILOBJ)

config: $(CONFOBJ) $(UTILOBJ)

util: $(UTILOBJ) $(OBJDIR)/$(UTILDIR)

$(OBJDIR):
	mkdir $@

$(OBJDIR)/$(TSERVDIR):
	mkdir $@

$(OBJDIR)/$(CONFDIR):
	mkdir $@ 

$(OBJDIR)/$(UTILDIR):
	mkdir $@

$(BINDIR):
	mkdir $(BINDIR)

all: config tserv

clean:
	rm -fr $(OBJDIR) $(BINDIR) 

