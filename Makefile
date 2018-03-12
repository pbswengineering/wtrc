# -*- Mode: Makefile; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-

SRC_DIR = src

.PHONY: default all clean indent doc valgrind

default:
	$(MAKE) -C $(SRC_DIR) default

all:
	$(MAKE) -C $(SRC_DIR) all

clean:
	$(MAKE) -C $(SRC_DIR) clean

indent:
	$(MAKE) -C $(SRC_DIR) indent

doc:
	$(MAKE) -C $(SRC_DIR) doc

valgrind:
	$(MAKE) -C $(SRC_DIR) valgrind
