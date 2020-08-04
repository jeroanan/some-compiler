OBJDIR=obj

comp: $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o
	$(CC) -g -o comp $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o

$(OBJDIR)/main.o: main.c
	$(CC) -g -c -o $(OBJDIR)/main.o main.c

$(OBJDIR)/error.o: error.c
	$(CC) -g -c -o $(OBJDIR)/error.o error.c

$(OBJDIR)/nasm.o: nasm.c
	$(CC) -g -c -o $(OBJDIR)/nasm.o nasm.c

clean:
	rm comp; rm -rf obj

$(shell mkdir -p obj)
$(shell mkdir -p output)
