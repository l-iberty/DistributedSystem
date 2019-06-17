#include "StringTokenizer.h"

std::vector<std::string> StringTokenizer::split(const std::string &s, const std::string &separator)
{
    std::vector<std::string> result;
    int i = 0;

    while (i != s.size())
    {
        /* 找到字符串中首个不等于分隔符的字母 */
        int flag = 0;
        while (i != s.size() && flag == 0)
        {
            flag = 1;
            for (int x = 0; x < separator.size(); x++)
            {
                if (s[i] == separator[x])
                {
                    i++;
                    flag = 0;
                    break;
                }
            }
        }

        /* 找到下一个分隔符,将两个分隔符之间的字符串取出 */
        flag = 0;
        int j = i;
        while (j != s.size() && flag == 0)
        {
            for (int x = 0; x < separator.size(); x++)
            {
                if (s[j] == separator[x])
                {
                    flag = 1;
                    break;
                }
            }
            if (flag == 0) j++;
        }
        if (i != j)
        {
            result.push_back(s.substr(i, j - i));
            i = j;
        }
    }
    return result;
}
