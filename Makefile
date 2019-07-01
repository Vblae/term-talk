CC = gcc
CFLAGS = -Wall -Werror

INCDIR = header
OBJDIR = obj
BINDIR = bin

BINNAME = tserv

TSERVDIR = tserv
TSERVSRC = $(wildcard $(TSERVDIR)/*.c)
TSERVOBJ = $(patsubst %.c, $(OBJDIR)/%.o, $(TSERVSRC))


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

clean:
	rm -fr $(OBJDIR) $(BINDIR) 

