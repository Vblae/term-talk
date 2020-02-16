CC = gcc
CFLAGS = -Wall 

INCDIR = include
OBJDIR = obj
BINDIR = bin

TSERV = tserv 
TSERVDIR = tserv
TSERVSRC = $(wildcard $(TSERVDIR)/*.c)
TSERVHDR = $(wildcard $(INCDIR)/$(TSERVDIR)/*.h)
TSERVOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(TSERVSRC))

# future use
# TCLIDIR =
# TCLISRC =
# TCLIOBJ =

TDB = tdb
TDBDIR = tdb
TDBSRC = $(wildcard $(TDBDIR)/*.c)
TDBHDR = $(wildcard $(INCDIR)/$(TDBDIR)/*.h)
TDBOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(TDBSRC))

CONFDIR = config
CONFSRC = $(wildcard $(CONFDIR)/*.c)
CONFHDR = $(wildcard $(INCDIR)/$(CONFDIR)/*.h)
CONFOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(CONFSRC))

UTILDIR = util
UTILSRC = $(wildcard $(UTILDIR)/*.c)
UTILHDR = $(wildcard $(INCDIR)/$(UTILDIR)/*.h)
UTILOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(UTILSRC))

$(OBJDIR)/$(TSERVDIR)/%.o: $(TSERVDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TSERVDIR) $(TSERVHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(TDBDIR)/%.o: $(TDBDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TDBDIR) $(TDBHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(CONFDIR)/%.o: $(CONFDIR)/%.c $(OBJDIR) $(OBJDIR)/$(CONFDIR) $(CONFHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(UTILDIR)/%.o: $(UTILDIR)/%.c $(OBJDIR) $(OBJDIR)/$(UTILDIR) $(UTILHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

tserv: $(BINDIR) $(TSERVOBJ) $(CONFOBJ) $(UTILOBJ)
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TSERV) $(TSERVOBJ) $(CONFOBJ) $(UTILOBJ)

tdb: $(BINDIR) $(TDBOJB) $(CONFOBJ) $(UTILOBJ)
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TDB) $(TDBOBJ) $(CONFOBJ) $(UTILOBJ)

config: $(CONFOBJ) $(UTILOBJ)

util: $(UTILOBJ) $(OBJDIR)/$(UTILDIR)

$(OBJDIR):
	mkdir $@

$(OBJDIR)/$(TSERVDIR):
	mkdir $@

$(OBJDIR)/$(TDBDIR):
	mkdir $@

$(OBJDIR)/$(CONFDIR):
	mkdir $@

$(OBJDIR)/$(UTILDIR):
	mkdir $@
 
$(BINDIR):
	mkdir $(BINDIR)

all: util config tserv tdb

clean:
	rm -fr $(OBJDIR) $(BINDIR) 

