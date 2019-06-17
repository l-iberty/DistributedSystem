# Bloom Filter & Cuckoo Filter
## 几个术语
- **false positive**：过滤器判断某个元素存在，但实际上该元素并不存在.
- **false negative**：如果某个元素确实不存在，那么过滤器一定不会误判为存在.

## 共同点
1. 都支持元素的无限插入.
2. 查询操作的特点：都满足**false positive**和**false negative**.

## 不同点
1. Bloom Filter插入元素时不需要扩容；Cuckoo Filter插入元素时可能需要扩容.
2. Bloom Filter仅支持插入和查询；Cuckoo Filter支持增删改查.
3. 相比于Bloom Filter，Cuckoo Filter需要较大的存储空间，但是Cuckoo Filter在和Bloom Filter拥有相同的查询特点的前提下，支持元素的增删改查.