OBJDIR=obj
SRCDIR=src
NASMDIR=src/nasm

comp: $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o $(OBJDIR)/var.o $(OBJDIR)/stringfunc.o
	$(CC) -g -o comp $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o $(OBJDIR)/var.o $(OBJDIR)/stringfunc.o

$(OBJDIR)/main.o: $(SRCDIR)/main.c 
	$(CC) -g -c -o $(OBJDIR)/main.o $(SRCDIR)/main.c 

$(OBJDIR)/var.o: $(SRCDIR)/var.c
	$(CC) -g -c -o $(OBJDIR)/var.o $(SRCDIR)/var.c 

$(OBJDIR)/error.o: $(SRCDIR)/error.c
	$(CC) -g -c -o $(OBJDIR)/error.o $(SRCDIR)/error.c

$(OBJDIR)/stringfunc.o: $(SRCDIR)/stringfunc.c
	$(CC) -g -c -o $(OBJDIR)/stringfunc.o $(SRCDIR)/stringfunc.c

$(OBJDIR)/nasm.o: $(NASMDIR)/nasm.c
	$(CC) -g -c -o $(OBJDIR)/nasm.o $(NASMDIR)/nasm.c

clean:
	rm comp; rm -rf obj

$(shell mkdir -p obj)
$(shell mkdir -p output)
