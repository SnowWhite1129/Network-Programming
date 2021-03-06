all:
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp np_simple.cpp -o np_simple
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp np_single_proc.cpp -o np_single_proc
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp np_multi_proc.cpp -o np_multi_proc
test:
	g++ -std=c++14 npshell.cpp message.cpp command.cpp np_simple.cpp -o np_simple
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp np_single_proc.cpp -o np_single_proc
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp np_multi_proc.cpp -o np_multi_proc
	g++ -std=c++14 client.cpp -o client
client:
	g++ -std=c++14 client.cpp -o client
