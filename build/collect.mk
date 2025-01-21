CWD := $(shell cd)


every_time:
	@echo CWD=$(CWD)

default: every_time
	@echo Nothing to do

main: every_time
	copy /Y dest\CBJQ_HugFenny-PC.exe collected\

prepare_dir: every_time
	if not exist collected mkdir collected

.PHONY: default prepare_dir
