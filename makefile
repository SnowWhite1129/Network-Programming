all:
	g++ -std=c++14 npshell.cpp np_simple.cpp -o np_simple
	g++ -std=c++14 npshell.cpp np_single_proc.cpp -o np_single_proc
	g++ -std=c++14 npshell.cpp np_multi_proc.cpp -o np_multi_proc
test:
    g++ -std=c++14 npshell.cpp np_simple.cpp -o np_simple
    g++ -std=c++14 npshell.cpp np_single_proc.cpp -o np_single_proc
    g++ -std=c++14 npshell.cpp np_multi_proc.cpp -o np_multi_proc
    g++ -std=c++14 npshell.cpp client.cpp -o client
