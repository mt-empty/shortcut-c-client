CC := gcc
CCFLAG := -Wall
DBGFLAG := -g
CCOBJFLAG := $(CCFLAG) -c
DEPS := cJson.h 

PREFIX	= /usr/local
DESTDIR	= 
TARGET	= shortcut

REPO	= https://github.com/mt-empty/shortcut-pages

shortcut: cJson.h
	@$(CC) $(CCFLAG) -o $@ $<


.PHONY: clean
clean:
	@rm -f *.o *.h.gch
	@rm -rf shortcut-pages pages

.PHONY: clone
clone:	clean
	@echo "Cloning repo"
	@git clone $(REPO)

.PHONY: move
move: clone uninstall
	@mkdir pages
	@cp -r shortcut-pages/GUI/* pages


.PHONY: install
install: shortcut move
	
	@echo "Installing"
	
	@mkdir -p $(DESTDIR)/opt/$(TARGET)
	@mv pages/ $(DESTDIR)/opt/$(TARGET)
	@mv $< $(DESTDIR)/opt/$(TARGET)
	@cp Readme.md $(DESTDIR)/opt/$(TARGET)

	@mkdir -p $(DESTDIR)$(PREFIX)/bin
	@ln -s $(DESTDIR)/opt/$(TARGET)/$(TARGET) $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	@echo "All done"

.PHONY: install-extra
install-extra: install
	@cp -r shortcut-pages/nonGUI/* $(DESTDIR)/opt/$(TARGET)/pages/


.PHONY: uninstall
uninstall:
	@rm -rf $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	@rm -rf $(DESTDIR)/opt/$(TARGET)
#	@echo "All done"