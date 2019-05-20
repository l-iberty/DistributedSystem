cmake_minimum_required(VERSION 3.9)
project(rpc_server)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp RpcServiceImpl.cpp RpcServiceImpl.h RpcServer.cpp RpcInvoker.h RpcService.h RpcServer.h RpcServerInvoker.cpp RpcServerInvoker.h RpcProtocol.cpp RpcCoderHelper.cpp RpcCoderHelper.h)
add_executable(rpc_server ${SOURCE_FILES})
target_link_libraries(rpc_server pthread)