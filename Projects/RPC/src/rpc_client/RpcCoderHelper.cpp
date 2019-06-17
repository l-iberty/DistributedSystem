#include <cstring>
#include "RpcCoderHelper.h"

void RpcCoderHelper::decode(std::string &s, std::vector<std::string> &strs, std::vector<int> &nums)
{
    std::vector<std::string> tokens = split(s, "#");
    for (std::string token:tokens)
    {
        std::vector<std::string> toks = split(token, ":");
        if (!strcmp(toks[0].c_str(), "int"))
        {
            nums.push_back(atoi(toks[1].c_str()));
        }
        else if (!strcmp(toks[0].c_str(), "string"))
        {
            strs.push_back(toks[1]);
        }
    }
}

std::string RpcCoderHelper::encode(std::vector<std::string> &strs, std::vector<int> &nums)
{
    std::string res = "";
    int i;

    if (!strs.empty())
    {
        for (i = 0; i < strs.size() - 1; i++)
        {
            res += "string:" + strs[i] + "#";
        }
        res += "string:" + strs[i];
    }

    if (!nums.empty())
    {
        if (!strs.empty())
        {
            res += "#";
        }

        for (i = 0; i < nums.size() - 1; i++)
        {
            res += "int:" + std::to_string(nums[i]) + "#";
        }
        res += "int:" + std::to_string(nums[i]);
    }

    return res;
}

std::vector<std::string> RpcCoderHelper::split(const std::string &s, const std::string &seperator)
{
    std::vector<std::string> result;
    int i = 0;

    while (i != s.size())
    {
        // 找到字符串中首个不等于分隔符的字母
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (int x = 0; x < seperator.size(); ++x)
            {
                if (s[i] == seperator[x])
                {
                    ++i;
                    flag = 0;
                    break;
                }
            }
        }

        // 找到又一个分隔符,将两个分隔符之间的字符串取出
        flag = 0;
        int j = i;
        while (j != s.size() && flag == 0)
        {
            for (int x = 0; x < seperator.size(); ++x)
            {
                if (s[j] == seperator[x])
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) ++j;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}