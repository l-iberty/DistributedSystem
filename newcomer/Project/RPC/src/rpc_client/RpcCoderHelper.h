#ifndef RPCDECODER_H
#define RPCDECODER_H

#include <string>
#include <vector>

/**
 * RpcCoderHelper 负责对消息体中的参数(返回值)表进行编解码
 */

class RpcCoderHelper
{
public:
    static void decode(std::string &s, std::vector<std::string> &strs, std::vector<int> &nums);
    static std::string encode(std::vector<std::string> &strs,std::vector<int> &nums);

private:
    static std::vector<std::string> split(const std::string &s, const std::string &seperator);
};


#endif /* RPCDECODER_H */
