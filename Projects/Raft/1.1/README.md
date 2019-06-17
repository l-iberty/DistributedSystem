# Raft 1.1
与1.0的主要区别在于附加日志项的方式: 每个节点增加一个TCP server线程`userThread`, 在指定端口上监听外界的用户请求, 当一个节点接收到请求后, 如果处于Leader状态就向`mLoglist`附加日志项, 在下一次心跳时AppendEntries RPC会将日志复制到其他节点; 如果未处于Leader状态, 就将请求重定向到Leader节点.

## 测试
通过启动脚本`start`启动集群中的5个节点后, 运行`user_client`向任意节点发送请求, 即可观察到日志复制的过程（如果向非Leader节点发送请求还可以看到请求被重定向到Leader节点的过程）.