# ?= define only if not already defined
# := A simple assignment expression is evaluated only once
# $@ is the name of the target(rule) being generated
# $< the first prerequisite (usually a source file)
# $^ the prerequisites of the rule

# When debugging use pass the pages directory with: make DEST_DIR=./ debug

# User configurable
PREFIX			?= /usr/local/bin
DEST_DIR		?= /opt


# General
TARGET			:= shortcut
TARGET_DIR 		:= shortcut-pages
PAGES_DIR 		:= $(DEST_DIR)/$(TARGET)/pages/
REPO			:= https://github.com/mt-empty/shortcut-pages


# Compiler flags
CC 		:= gcc
CCFLAG 	:= -Wall
DBGFLAG := -g -DDEBUG=1

ALL_CCFLAGS		:= $(CCFLAG) -DPAGES_DIR='"$(PAGES_DIR)"'


HAS_GIT	:= $(shell type git > /dev/null 2>&1 && echo "1" || echo "0")
ifeq (0,$(HAS_GIT))
	@echo "git not found, please install git"
endif


shortcut: shortcut.c
	@$(CC) $(ALL_CCFLAGS) -o $@ $<

.PHONY: shortcut-debug
shortcut-debug: shortcut.c
	@$(CC) $(ALL_CCFLAGS) $(DBGFLAG) -o $(TARGET) $<

.PHONY: clean
clean:
	@rm -f *.o *.h.gch
	@rm -rf $(TARGET_DIR) pages

.PHONY: clone
clone:
	@git -C $(TARGET_DIR) pull || (echo "Cloning shortcut pages" && git clone $(REPO) $(TARGET_DIR))

.PHONY: move
move: clone uninstall-helper
	@mkdir pages
	@cp -r $(TARGET_DIR)/GUI/* pages


.PHONY: check-root
check-root:
ifneq ($(shell id -u), 0)
	@echo "You are not root, run this target as root please, exiting"
	@exit 1
endif

.PHONY: install
install: check-root clean shortcut move
	
	@echo "Installing"
	
	@mkdir -p $(DEST_DIR)/$(TARGET)
	@mv pages/ $(DEST_DIR)/$(TARGET)
	@mv $(TARGET) $(DEST_DIR)/$(TARGET)
	@cp Readme.md $(DEST_DIR)/$(TARGET)

	@mkdir -p $(PREFIX)
	@ln -s $(DEST_DIR)/$(TARGET)/$(TARGET) $(PREFIX)/$(TARGET)
	@rm -rf $(TARGET_DIR)
	@echo "All done"

.PHONY: install-extra
install-extra: install
	@cp -r $(TARGET_DIR)/nonGUI/* $(DEST_DIR)/$(TARGET)/pages/

.PHONY: debug
debug: shortcut-debug clone
	@echo $(DEST_DIR)
	@echo $(PAGES_DIR)
	@mkdir -p $(PAGES_DIR)
	@cp -r $(TARGET_DIR)/GUI/* $(PAGES_DIR)
	@rm -f *.o *.h.gch

.PHONY: uninstall
uninstall: check-root uninstall-helper
	@echo "shortcut uninstalled successfully"

.PHONY: uninstall-helper
uninstall-helper:
	@rm -rf $(PREFIX)/$(TARGET)
	@rm -rf $(DEST_DIR)/$(TARGET)
