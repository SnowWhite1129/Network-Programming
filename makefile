all:
	g++ -std=c++14 npshell.cpp command.cpp np_simple.cpp -o np_simple
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp nppipe.cpp np_single_proc.cpp -o np_single_proc
	g++ -std=c++14 npshell.cpp multiuser.cpp multimessage.cpp command.cpp multinppipe.cpp np_multi_proc.cpp -o np_multi_proc
simple:
	g++ -std=c++14 npshell.cpp command.cpp np_simple.cpp -o np_simple
single:
	g++ -std=c++14 npshell.cpp user.cpp message.cpp command.cpp nppipe.cpp np_single_proc.cpp -o np_single_proc
multi:
	g++ -std=c++14 npshell.cpp multiuser.cpp multimessage.cpp command.cpp multinppipe.cpp np_multi_proc.cpp -o np_multi_proc
