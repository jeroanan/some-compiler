OBJDIR=obj

comp: $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o
	$(CC) -g -o comp $(OBJDIR)/main.o $(OBJDIR)/error.o $(OBJDIR)/nasm.o

$(OBJDIR)/main.o: src/main.c
	$(CC) -g -c -o $(OBJDIR)/main.o src/main.c

$(OBJDIR)/error.o: src/error.c
	$(CC) -g -c -o $(OBJDIR)/error.o src/error.c

$(OBJDIR)/nasm.o: src/nasm.c
	$(CC) -g -c -o $(OBJDIR)/nasm.o src/nasm.c

clean:
	rm comp; rm -rf obj

$(shell mkdir -p obj)
$(shell mkdir -p output)
