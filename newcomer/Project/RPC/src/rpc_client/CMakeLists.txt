cmake_minimum_required(VERSION 3.9)
project(rpc_client)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp RpcService.h RpcServiceProxyImpl.cpp RpcServiceProxyImpl.h RpcProtocol.h RpcProtocol.cpp RpcInvoker.h RpcClientInvoker.cpp RpcClientInvoker.h RpcCoderHelper.cpp RpcCoderHelper.h)
add_executable(rpc_client ${SOURCE_FILES})
target_link_libraries(rpc_client pthread)