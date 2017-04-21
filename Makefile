program: 
	$(MAKE) -C ./src
	ln -f --symbolic ../src/nfaltern ./bin	
	ln -f --symbolic ../src/deamon ./bin	

clean: 
	$(MAKE) -C ./src clean
.phony: program clean
