all: $(BINDIR)/$(BINDIRSER)/$(BINSER) $(BINDIR)/$(BINDIRCLT)/$(BINCLT)


$(BINDIR)/$(BINDIRSER)/$(BINSER):$(BINSER).o
	$(CC) $^ -o $@  $(OBJHEAD) $(OBJLINK)

$(BINDIR)/$(BINDIRCLT)/$(BINCLT):$(BINCLT).o
	$(CC) $^ -o $@  $(OBJHEAD) $(OBJLINK)

