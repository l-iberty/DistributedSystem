#include <stdint.h>
#include <iostream>
#include <vector>
#include "RaftNode.h"

int main(int argc, char* argv[])
{
    int node_id, node_num;
    std::vector<uint16_t> user_ports;
    std::vector<uint16_t> rpc_ports;

    if (argc < 5)
    {
        printf("usage: %s node_id node_num user_port0 ... user_portn rpc_port0 ... rpc_portn\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    node_id = atoi(argv[1]);
    node_num = atoi(argv[2]);
    int i, j;
    for (i = 3; i < 3 + node_num; i++)
    {
        user_ports.push_back(static_cast<uint16_t>(atoi(argv[i])));
    }
    for (j = i; j < i + node_num; j++)
    {
        rpc_ports.push_back(static_cast<uint16_t>(atoi(argv[j])));
    }

    RaftNode rfNode(node_id, rpc_ports, user_ports);
    rfNode.run();
}
