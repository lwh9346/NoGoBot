#include <math.h>
#include <random>
#include <iostream>
#include "./validPosition.cpp"
#define branchNum 16
#define minInt -2147483648
using namespace std;

class treeNode
{
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
    int q;                            //回溯后当前节点获胜局数
    int n;                            //该节点被访问次数
    double score;                     //UCT算法中的得分
    int *countPtr;                    //指向总探索次数

    //构造函数
    treeNode(int parentBoard[9][9], int action[2], treeNode *p, int *countp)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
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
    treeNode *treePolicy()
    {
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

        int bestChild = 0;
        double maxScore = 0;
        for (int i = 0; i < childrenCount; i++)
        {
            if (maxScore < children[i]->score)
            {
                bestChild = i;
                maxScore = children[i]->score;
            }
        }
        return children[bestChild]->treePolicy();
    }

    //返回由当前节点开始模拟的结果，1代表当前方赢，-1代表当前方输
    int simulation()
    {
        int simulationBoard[9][9];
        int simulationBoardR[9][9]; //对方的棋盘

        //拷贝一份棋盘
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                simulationBoard[i][j] = board[i][j];
                simulationBoardR[i][j] = -board[i][j];
            }
        }

        //开始模拟
        int r = result(simulationBoard, simulationBoardR);
        int round = 0;
        while (!r)
        {
            if (round % 2 == 0)
            {
                int a = defaultPolicy(simulationBoard);
                simulationBoard[a / 9][a % 9] = 1;
                simulationBoardR[a / 9][a % 9] = -1;
                round++;
            }
            else
            {
                int a = defaultPolicy(simulationBoardR);
                simulationBoard[a / 9][a % 9] = -1;
                simulationBoardR[a / 9][a % 9] = 1;
                round++;
            }
            r = result(simulationBoard, simulationBoardR);
        }
        return r;
    }

    //沿树逆行，反向传播结果信息
    //delta:结果，1是当前方赢，0是当前方输
    void backup(int delta)
    {
        treeNode *node = this;
        int d = 0;
        while (node != nullptr)
        {
            if (delta == d % 2) //q值是隔层传播的
            {
                node->q++;
            }
            node->n++;
            node->score = double(node->q) / double(node->n) + 1.414 * sqrt(log(double(*countPtr)) / double(node->n)); //得分公式
            node = node->parent;
            d++;
        }
    }

private:
    //计算生成用于创建子节点的行动，但不创建子节点
    void evaluate()
    {
        int result[9][9];
        int resultTemp[9][9];
        int boardR[9][9]; //对方视角的棋盘
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                boardR[i][j] = -board[i][j];
            }
        }
        int validPositionCount = getValidPositions(board, result);
        int validPositions[81];
        int n = 0;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (result[i][j])
                {
                    validPositions[n] = 9 * i + j;
                    n++;
                }
            }
        }

        if (validPositionCount <= branchNum)
        {
            childrenCountMax = validPositionCount;
            for (int i = 0; i < validPositionCount; i++)
            {
                childrenAction[i][0] = validPositions[i] / 9;
                childrenAction[i][1] = validPositions[i] % 9;
            }
        }
        else
        {
            childrenCountMax = branchNum;
            int positionMark[81]; //记录节点分数，分数越高质量越高

            //计算分数
            for (int i = 0; i < validPositionCount; i++)
            {
                int x, y;
                x = validPositions[i] / 9;
                y = validPositions[i] % 9;
                int temp = 0; //分数

                //对角有棋子，加分
                if (x != 0 && y != 0 && board[x - 1][y - 1] != 0)
                {
                    temp += 1;
                }
                if (x != 0 && y != 8 && board[x - 1][y + 1] != 0)
                {
                    temp += 1;
                }
                if (x != 8 && y != 0 && board[x + 1][y - 1] != 0)
                {
                    temp += 1;
                }
                if (x != 8 && y != 8 && board[x + 1][y + 1] != 0)
                {
                    temp += 1;
                }

                //上下左右有己方棋子，减分
                if (x != 0 && board[x - 1][y] == 1)
                {
                    temp -= 1;
                }
                if (x != 8 && board[x + 1][y] == 1)
                {
                    temp -= 1;
                }
                if (y != 0 && board[x][y - 1] == 1)
                {
                    temp -= 1;
                }
                if (y != 8 && board[x][y + 1] == 1)
                {
                    temp -= 1;
                }

                //模拟一步
                board[x][y] = 1;
                boardR[x][y] = -1;

                temp += getValidPositions(board, resultTemp) * 4;
                temp -= getValidPositions(boardR, resultTemp) * 4;

                board[x][y] = 0;
                boardR[x][y] = 0;

                //赋分
                positionMark[i] = temp;
            }

            //按分数排序
            for (int i = 0; i < branchNum; i++)
            {
                int temp = positionMark[i];
                int tempI = i;
                for (int j = i; j < validPositionCount; j++)
                {
                    if (temp < positionMark[j])
                    {
                        temp = positionMark[j];
                        tempI = j;
                    }
                }
                swap(positionMark[i], positionMark[tempI]);
                swap(validPositions[i], validPositions[tempI]);
            }

            //取出前面几名
            for (int i = 0; i < branchNum; i++)
            {
                childrenAction[i][0] = validPositions[i] / 9;
                childrenAction[i][1] = validPositions[i] % 9;
            }
        }
    }

    //返回当前棋局状态
    //0棋局未结束
    //1棋局结束，当前方赢
    //-1棋局结束，当前方输
    int result(int board[9][9], int boardR[9][9])
    {

        int result[9][9];
        int a, b;
        a = getValidPositions(board, result);
        b = getValidPositions(boardR, result);
        if (a != 0 && b != 0)
        {
            return 0;
        }
        if (a == 0)
        {
            return -1;
        }
        return 1;
    }

    //快速落子策略
    //返回落子坐标变换后的值，x=result/9，y=result%9
    int defaultPolicy(int board[9][9])
    {
        int result[9][9];
        int validPositionCount = getValidPositions(board, result);
        int validPositions[81];
        int n = 0;
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (result[i][j])
                {
                    validPositions[n] = 9 * i + j;
                    n++;
                }
            }
        }

        int max = minInt; //最大分数
        int maxI = 0;     //最大分数对应的坐标的索引

        //计算分数
        for (int i = 0; i < validPositionCount; i++)
        {
            int temp = rand();

            //快速策略，不进行模拟
            if (temp > max)
            {
                max = temp;
                maxI = i;
            }
        }

        return validPositions[maxI];
    }
};