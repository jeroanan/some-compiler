OBJDIR=obj
SRCDIR=src
NASMDIR=src/nasm

comp: $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o
	$(CC) -g -o comp $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o

$(OBJDIR)/main.o: $(SRCDIR)/main.c
	$(CC) -g -c -o $(OBJDIR)/main.o $(SRCDIR)/main.c

$(OBJDIR)/error.o: $(SRCDIR)/error.c
	$(CC) -g -c -o $(OBJDIR)/error.o $(SRCDIR)/error.c

$(OBJDIR)/nasm.o: $(NASMDIR)/nasm.c
	$(CC) -g -c -o $(OBJDIR)/nasm.o $(NASMDIR)/nasm.c

clean:
	rm comp; rm -rf obj

$(shell mkdir -p obj)
$(shell mkdir -p output)
