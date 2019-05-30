# Raft

## 实现功能
- [x] Leader election
- [x] Log replication

## 测试
### 1. 启动
运行启动脚本`start`创建5个terminal, 每个节点的初始状态为Follower, 选举超时后经过一轮选举会选出一个Leader, Leader先给自己附加日志项(详见`log.txt`), 然后向其他节点发送`AppendEntries`来宣示自己的地位并完成日志复制(每次复制多少个日志项由`RaftAlg.h`的`MAX_ENTRIES`宏决定).

### 2. Leader崩溃
终止当前Leader进程后总会又有新的Leader诞生, 但是当只剩下两个节点时每个节点至多只能获得2张选票, 而在5个节点的Raft集群中至少要获得3张选票的Candidate才能成为Leader, 所以此时Raft集群因无法选出Leader而无法正常运作.

### 3. Follower选举超时 (参见`RaftNode.cpp`对`doAppendEntries()`的注释)
例如: 当系统正常运行时, 键入命令`echo 1 > block1`可能导致状态为Follower的节点[1]发生选举超时(该命令可能要连续执行多次才有效果), 节点[1]增加`currentTerm`并开始新的Leader election, 导致当前Leader转换为Follower, 节点[1]成为新的Leader, 然后又会给自己附加日志项, 开始Leader的流程.
