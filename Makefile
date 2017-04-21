program: 
	$(MAKE) -C ./src
	ln --symbolic ../src/nfaltern ./bin	
	ln --symbolic ../src/deamon ./bin	

clean: 
	$(MAKE) -C ./src clean
.phony: program clean
