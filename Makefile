CC = gcc
CFLAGS = -Wall -Werror

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
# TCLIHDR =
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

all: util config tserv tdb

$(OBJDIR)/$(TSERVDIR)/%.o: $(TSERVDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TSERVDIR) $(TSERVHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(TDBDIR)/%.o: $(TDBDIR)/%.c $(OBJDIR) $(OBJDIR)/$(TDBDIR) $(TDBHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(CONFDIR)/%.o: $(CONFDIR)/%.c $(OBJDIR) $(OBJDIR)/$(CONFDIR) $(CONFHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

$(OBJDIR)/$(UTILDIR)/%.o: $(UTILDIR)/%.c $(OBJDIR) $(OBJDIR)/$(UTILDIR) $(UTILHDR)
	$(CC) $(CFLAGS) -I $(INCDIR) -c -o $@ $<

tserv: $(BINDIR) $(TSERVOBJ) config util
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TSERV) $(TSERVOBJ) $(CONFOBJ) $(UTILOBJ)

tdb: $(BINDIR) $(TDBOBJ) config util
	$(CC) $(CFLAGS) -I $(INCDIR) -o $(BINDIR)/$(TDB) $(TDBOBJ) $(CONFOBJ) $(UTILOBJ)

config: $(CONFOBJ) $(UTILOBJ)

util: $(UTILOBJ)

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


clean:
	rm -fr $(OBJDIR) $(BINDIR) 

