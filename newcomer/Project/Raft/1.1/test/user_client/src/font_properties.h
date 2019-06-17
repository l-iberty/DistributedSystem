/**
 * refer to: https://www.cnblogs.com/frydsh/p/4139922.html
 *
 * Linux终端标准输出的字体属性控制
 */

#ifndef RAFT_FONT_COLOR_H
#define RAFT_FONT_COLOR_H

/**
 * 转义序列格式: "\e[显示方式;前景色;背景色m" OR "\033[显示方式;前景色;背景色m"
 * 释义:
 * 显示方式: 0(默认)、1(粗体,高亮)
 * 颜色值: 0(黑)、1(红)、2(绿)、3(黄)、4(蓝)、5(洋红)、6(青)、7(白)
 * 前景色为=30+颜色值，如31表示前景色为红色；背景色为=40+颜色值，如41表示背景色为红色
 */

#define STDOUT_NONE        "\e[0m"     /* 用于恢复终端默认输出属性, 否则会影响到后续输出 */
#define STDOUT_RED         "\e[31m"    /* 红色 */
#define STDOUT_L_RED       "\e[1;31m"  /* 红色, 加粗, 高亮 */
#define STDOUT_GREEN       "\e[32m"    /* 绿色 */
#define STDOUT_L_GREEN     "\e[1;32m"  /* 绿色, 加粗, 高亮 */

#endif /* RAFT_FONT_COLOR_H */
