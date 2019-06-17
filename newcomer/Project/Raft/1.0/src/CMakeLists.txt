cmake_minimum_required(VERSION 3.9)
project(Raft)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES
        main.cpp TimerThread.cpp
        font_properties.h
        RpcProtocol.h
        RaftAlg.h
        RaftNode.cpp RaftNode.h
        Timer.cpp Timer.h
        Random.cpp Random.h
        StringTokenizer.cpp StringTokenizer.h
        RpcServer.cpp RpcServer.h
        RpcServer.cpp RpcInvoker.cpp
        RpcClient.cpp RpcClient.h)

add_executable(Raft ${SOURCE_FILES})
target_link_libraries(Raft pthread rt)