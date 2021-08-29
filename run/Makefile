cleanall:
	rm *.txt
	rm -rf outputs
	mkdir outputs
clean_placer:
	rm -rf outputs/placer_outputs
	mkdir outputs/placer_outputs
clean_router:
	rm -rf outputs/router_outputs
	mkdir outputs/router_outputs
compile_placer: 
	g++ ../c_files/run_placer.cpp -I ../headers/ -o exec_placer.out
compile_router: 
	g++ ../c_files/run_router.cpp -I ../headers/ -o exec_router.out
run_placer:
	./exec_placer.out 
run_router:
	./exec_router.out
