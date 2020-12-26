#include "./validPosition.cpp"
#include <iostream>
#include <math.h>
#include <random>
#define branchNum 81
#define minInt -2147483648
using namespace std;

class treeNode {
public:
    //父节点
    treeNode *parent;

    //子节点
    treeNode *children[branchNum];

    //棋盘,1为己方,-1为对方,0为空
    int board[9][9];

    int childrenAction[branchNum][2]; //用于创建子节点的行动
    int childrenCount;                //当前子节点个数
    int childrenCountMax;             //待探索的子节点个数
    double q;                         //回溯后当前节点获胜局数
    int n;                            //该节点被访问次数
    double score;                     //UCT算法中的得分
    int *countPtr;                    //指向总探索次数

    //构造函数
    treeNode(int parentBoard[9][9], int action[2], treeNode *p, int *countp) {
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                board[i][j] = -parentBoard[i][j]; //双方反转
            }
        }
        board[action[0]][action[1]] = -1; //根据行动落子
        parent = p;
        q = 0;
        n = 0;
        childrenCount = 0;
        countPtr = countp;
        evaluate(); //计算生成用于创建子节点的行动，但不创建子节点
    }

    //MCTS中的树策略
    treeNode *treePolicy() {
        if (childrenCountMax == 0) //叶子节点
        {
            return this;
        }

        if (childrenCountMax > childrenCount) //未被完全探索的节点
        {
            treeNode *c = new treeNode(board, childrenAction[childrenCount], this, countPtr);
            children[childrenCount] = c;
            childrenCount++;
            return c;
        }

        //完全探索的节点
        //选取分数最高的子节点进行探索

        for (int i = 0; i < childrenCount; i++) {
            children[i]->score = children[i]->q / double(children[i]->n) + 0.2 * sqrt(log(double(*countPtr)) / double(children[i]->n)); //计算得分
        }
        int bestChild = 0;
        double maxScore = 0;
        for (int i = 0; i < childrenCount; i++) {
            if (maxScore < children[i]->score) {
                bestChild = i;
                maxScore = children[i]->score;
            }
        }
        return children[bestChild]->treePolicy();
    }

    //返回由当前节点开始模拟的结果，返回值代表当前方胜率
    double simulation() {
        int boardR[9][9]; //对方的棋盘
        int res[9][9];
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                boardR[i][j] = -board[i][j];
            }
        }
        int x = getValidPositions(board, res);
        int y = getValidPositions(boardR, res);
        if (x + y == 0) {
            return 0.0;
        }
        if (y == 0) {
            return 1.0;
        }
        if (x == 0) {
            return -1.0;
        }
        double rate = tanh(double(x - y) / double(x + y));
        return rate;
    }

    //沿树逆行，反向传播结果信息
    //delta:结果，1是当前方赢，0是当前方输
    void backup(double delta) {
        treeNode *node = this;
        int d = 0;
        while (node != nullptr) {
            if (1 == d % 2) //q值是隔层传播的
            {
                node->q += delta;
            } else {
                node->q -= delta;
            }
            node->n++;
            node = node->parent;
            d++;
        }
    }

private:
    //计算生成用于创建子节点的行动，但不创建子节点
    void evaluate() {
        int result[9][9];
        int validPositionCount = getValidPositions(board, result);
        int validPositions[81];
        int n = 0;
        for (int i = 0; i < 9; i++) {
            for (int j = 0; j < 9; j++) {
                if (result[i][j]) {
                    validPositions[n] = 9 * i + j;
                    n++;
                }
            }
        }
        childrenCountMax = validPositionCount;
        for (int i = 0; i < validPositionCount; i++) {
            childrenAction[i][0] = validPositions[i] / 9;
            childrenAction[i][1] = validPositions[i] % 9;
        }
    }
};