# 分布式RPC框架
参考博客:[http://www.cnblogs.com/metoy/p/4321311.html](http://www.cnblogs.com/metoy/p/4321311.html). 实现的是同步方式, 异步方式尚未实现.

对系统的并发性进行了极限测试, 结果表明, 在将TCP端口资源耗尽前系统可以正常工作.

## 添加新服务`xxx`的步骤
1. 在`RpcService.h`中定义接口`xxx`, 并将对`RpcService.h`的修改同步到客户端和服务端
2. 在客户端的`RpcServiceProxyImpl`类中对接口`xxx`进行代理实现
3. 在服务端的`RpcServiceImpl`类中对接口`xxx`进行具体实现
4. 在服务端的`ServiceStub.h/.cpp`中实现接口的stub函数`xxxStub`, 然后在`RpcServerInvoker`的构造函数中注册之
5. 客户端通过`RpcServiceProxyImpl`对象调用服务`xxx`