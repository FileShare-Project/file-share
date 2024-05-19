define printSection
	$(eval SIZE=$(shell echo -n $1 | wc -c))
	$(eval BORDER=$(shell printf '\*%.0s' $$(seq 1 $(SIZE))))
	@echo "\033[1;36m"
	@echo $(BORDER)
	@echo "* $1 *"
	@echo $(BORDER)
	@echo "\033[0m"
endef

.PHONY: help h
help h:
	@echo "\033[1;36mUsage: make [target]\033[0m"
	@echo ""
	@echo "\033[1;33mTargets:\033[0m"
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "help, h:" "Display this help message."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "build, b:" "Build the project."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "run, r:" "Run the project."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "build-run, br:" "Build and run the project."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "clean, c:" "Clean the project."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "clean-build, cb:" "Clean and build the project."
	@printf "\033[0;32m  %-30s \033[1;34m%s\033[0m\n" "clean-build-run, cbr:" "Clean, build and run the project."

.PHONY: build b
build b:
	cmake -B ./build && make -C ./build

.PHONY: run r
run r:
	./build/file-share

.PHONY: build-run br
build-run br: build
	$(call printSection,"Build completé. Lancement du programme...")
	@$(MAKE) run

.PHONY: clean c
clean c:
	rm -rf ./build ./file-share

.PHONY: clean-build cb
clean-build cb: clean build

.PHONY: clean-build-run cbr
clean-build-run cbr: clean build-run
