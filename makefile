CC := gcc
CCFLAG := -Wall
DBGFLAG := -g

PREFIX	= /usr/local
DESTDIR	= 
TARGET	= shortcut
TARGET_DIR = shortcut-pages

REPO	= https://github.com/mt-empty/shortcut-pages

shortcut: shortcut.o
	@$(CC) $(CCFLAG) -o $@ $<

shortcut-debug: shortcut.o
	@$(CC) $(CCFLAG) $(DBGFLAG) -o $@ $<

.PHONY: clean
clean:
	@rm -f *.o *.h.gch
	@rm -rf $(TARGET_DIR) pages

.PHONY: clone
clone:
	@git -C $(TARGET_DIR) pull || (echo "Cloning shortcut pages" && git clone $(REPO) $(TARGET_DIR))

.PHONY: move
move: clone uninstall
	@mkdir pages
	@cp -r $(TARGET_DIR)/GUI/* pages


.PHONY: install
install: clean shortcut move
	
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
	@cp -r $(TARGET_DIR)/nonGUI/* $(DESTDIR)/opt/$(TARGET)/pages/

.PHONY: debug
debug: shortcut-debug clone
	@mv $< $(TARGET)
	@rm -f *.o *.h.gch

.PHONY: uninstall
uninstall:
	@rm -rf $(DESTDIR)$(PREFIX)/bin/$(TARGET)
	@rm -rf $(DESTDIR)/opt/$(TARGET)
#	@echo "All done"