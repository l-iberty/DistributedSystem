#ifndef RAFT_STRINGTOKENIZER_H
#define RAFT_STRINGTOKENIZER_H

#include <string>
#include <vector>

class StringTokenizer
{
public:
    static std::vector<std::string> split(const std::string &s, const std::string &separator);
};


#endif /* RAFT_STRINGTOKENIZER_H */
