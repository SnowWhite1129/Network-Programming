cmake_minimum_required(VERSION 3.14)
project(Network_Programming)

set(CMAKE_CXX_STANDARD 14)

add_executable(Network_Programming npshell.cpp npshell.h np_simple.cpp user.cpp user.h command.cpp command.h message.cpp message.h np_simple.h np_single_proc.cpp np_single_proc.h np_multi_proc.cpp np_multi_proc.h nppipe.cpp nppipe.h sharememory.cpp sharememory.h multimessage.cpp multimessage.h)