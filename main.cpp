#include <math.h>
#include <random>
#include <iostream>
#include <ctime>
#include "./jsoncpp/json.h"
#include <string>
#include <cstring>
#include "./mctNode.cpp"
using namespace std;

int main()
{
    //计数器与棋盘声明，随机数初始化
    int count = 0;
    int board[9][9] = {0};
    srand(clock());

    //读入JSON
    string str;
    getline(cin, str);

    //计时开始并计算时限
    int start = clock();
    int timeout = (int)(0.95 * (double)CLOCKS_PER_SEC);

    //解析JSON
    Json::Reader reader;
    Json::Value input;
    reader.parse(str, input);
    int turnID = input["responses"].size();
    int x, y;
    for (int i = 0; i < turnID; i++)
    {
        x = input["requests"][i]["y"].asInt(), y = input["requests"][i]["x"].asInt();
        if (x != -1)
            board[x][y] = 1;
        x = input["responses"][i]["y"].asInt(), y = input["responses"][i]["x"].asInt();
        if (x != -1)
            board[x][y] = -1;
    }
    x = input["requests"][turnID]["y"].asInt(), y = input["requests"][turnID]["x"].asInt();
    int actionR[2] = {x, y};

    //创建蒙特卡洛树根节点
    treeNode root(board, actionR, nullptr, &count);

    //开始蒙特卡洛树搜索
    while (clock() - start < timeout)
    {
        count++;
        treeNode *node = root.treePolicy();
        int result = node->simulation();
        node->backup(result == 1 ? 1 : 0);
    }

    //选取最佳节点作为下一步
    int max = 0;
    int maxI = 0;
    int *bestAction = root.childrenAction[0];
    for (int i = 0; i < root.childrenCount; i++)
    {
        if (max < root.children[i]->n)
        {
            maxI = i;
            max = root.children[i]->n;
            bestAction = root.childrenAction[i];
        }
    }

    //输出结果与DEBUG信息
    Json::Value ret;
    Json::Value action;
    action["x"] = bestAction[1];
    action["y"] = bestAction[0];
    ret["response"] = action;
    char buffer[4096];
    sprintf(buffer, "MCTS节点数:%d,当前预估胜率:%.3f", count, ((double)(root.children[maxI]->q)) / ((double)root.children[maxI]->n));
    ret["debug"] = buffer;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
}