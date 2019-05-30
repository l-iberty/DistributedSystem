#include <stdint.h>
#include <assert.h>
#include <iostream>
#include <vector>
#include "RaftNode.h"

int main(int argc, char *argv[])
{
    int id, nr_nodes;
    uint16_t port;
    std::vector<uint16_t> ports;

    if (argc < 3)
    {
        printf("usage: %s node_id port_0 ... port_n\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    id = atoi(argv[1]);
    assert(id >= 0 && id < argc - 2);
    for (int i = 2; i < argc; i++)
    {
        ports.push_back(atoi(argv[i]));
    }

    RaftNode rfNode(id, ports);
    rfNode.run();
}
