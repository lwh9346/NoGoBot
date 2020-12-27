#include "./jsoncpp/json.h"
#include "bot.cpp"
#include <cstring>
#include <ctime>
#include <iostream>
#include <math.h>
#include <random>
#include <string>
using namespace std;

int main() {
    //计数器与棋盘声明，随机数初始化
    signed char board[81] = {0};
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
    for (int i = 0; i < turnID; i++) {
        x = input["requests"][i]["y"].asInt(), y = input["requests"][i]["x"].asInt();
        if (x != -1)
            board[x * 9 + y] = -1;
        x = input["responses"][i]["y"].asInt(), y = input["responses"][i]["x"].asInt();
        if (x != -1)
            board[x * 9 + y] = 1;
    }
    x = input["requests"][turnID]["y"].asInt(), y = input["requests"][turnID]["x"].asInt();
    board[x * 9 + y] = -1;

    //MCTS
    mcts::DebugData debug;
    int bestAction = mcts::GetBestAction(board, 0.95, &debug);

    //输出结果与DEBUG信息
    Json::Value ret;
    Json::Value action;
    action["x"] = bestAction % 9;
    action["y"] = bestAction / 9;
    ret["response"] = action;
    char buffer[4096];
    sprintf(buffer, "MCTS节点数:%d,当前预估胜率:%.3f", debug.nodeCount, debug.winningRate);
    ret["debug"] = buffer;
    Json::FastWriter writer;
    cout << writer.write(ret) << endl;
}