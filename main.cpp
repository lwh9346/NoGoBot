#include <math.h>
#include <random>
#include <iostream>
#include <ctime>
#include "./jsoncpp/json.h"
#include <string>
#include <cstring>
#define branchNum 4
using namespace std;

void gasFinding(int x, int y, int i, int j, int groupIndex, int board[9][9], char group[9][9], char groupGasState[81], char groupGasPosition[81][2], char positionState[9][9], char unicGasGroup[9][9])
{
    if (board[i][j] == 0)
    {
        switch (groupGasState[groupIndex])
        {
        case 0:
            groupGasPosition[groupIndex][0] = i;
            groupGasPosition[groupIndex][1] = j;
            groupGasState[groupIndex] = 1;
            if (board[x][y] == 1)
            {
                positionState[i][j] |= 2;
            }
            else
            {
                positionState[i][j] |= 1;
                unicGasGroup[i][j] = groupIndex;
            }
            break;
        case 1: //如果之前有唯一气的话现在就有两个气了
            groupGasState[groupIndex] = 2;
            if (board[x][y] == 1)
            {
                //己方
                positionState[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] |= 4;
            }
            else if (unicGasGroup[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] == groupIndex) //如果是自己留下来的唯一气的话，现在就不是唯一气了，否则还是
            {
                //对方
                positionState[groupGasPosition[groupIndex][0]][groupGasPosition[groupIndex][1]] &= ~1;
            }
            break;
        case 2:
            if (board[x][y] == 1)
            {
                positionState[i][j] |= 4;
            }
            //如果是对方的话可以不管，对方的非唯一气没有影响
            break;
        }
    }
}

void grouping(int x, int y, int groupIndex, int board[9][9], char group[9][9], char groupGasState[81], char groupGasPosition[81][2], char positionState[9][9], char unicGasGroup[9][9])
{
    group[x][y] = groupIndex;
    //找气
    if (x != 0)
    {
        gasFinding(x, y, x - 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 0)
    {
        gasFinding(x, y, x, y - 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (x != 8)
    {
        gasFinding(x, y, x + 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 8)
    {
        gasFinding(x, y, x, y + 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }

    //递归分组
    if (x != 0 && group[x - 1][y] == -1 && board[x][y] == board[x - 1][y])
    {
        grouping(x - 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 0 && group[x][y - 1] == -1 && board[x][y] == board[x][y - 1])
    {
        grouping(x, y - 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (x != 8 && group[x + 1][y] == -1 && board[x][y] == board[x + 1][y])
    {
        grouping(x + 1, y, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
    if (y != 8 && group[x][y + 1] == -1 && board[x][y] == board[x][y + 1])
    {
        grouping(x, y + 1, groupIndex, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
    }
}

//result里面1是可能的落子位置，0是不可能的位置，返回值为可落子位置总数
int getValidPositions(int board[9][9], int result[9][9])
{
    int r = 0;
    int groupCount = 0;
    char group[9][9];        //保存各位置棋子的分组，-1表示未分组，数字表示分组的索引
    char unicGasGroup[9][9]; //如果是对方唯一气的话，存下是哪一组的唯一气
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            unicGasGroup[i][j] = -1;
            group[i][j] = -1;
        }
    }
    char groupGasState[81] = {0};       //保存各分组的气的状态，分为0无气、1有单一气、2有多气
    char groupGasPosition[81][2] = {0}; //唯一气的位置
    char positionState[9][9] = {0};     //由低位开始，+1表示为对方唯一气，+2表示为己方唯一气，+4表示为己方非唯一气
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (group[i][j] != -1 || board[i][j] == 0) //跳过已分组的元素以及空格
            {
                continue;
            }
            grouping(i, j, groupCount, board, group, groupGasState, groupGasPosition, positionState, unicGasGroup);
            groupCount++;
        }
    }
    for (int i = 0; i < 9; i++)
    {
        for (int j = 0; j < 9; j++)
        {
            if (board[i][j] != 0) //不是空格，不能下
            {
                result[i][j] = 0;
                continue;
            }
            if (positionState[i][j] % 2 == 1) //对方唯一气，不能下
            {
                result[i][j] = 0;
                continue;
            }
            if (positionState[i][j] >> 2 == 1) //己方非唯一气，能下
            {
                result[i][j] = 1;
                r++;
                continue;
            }
            if (positionState[i][j] >> 1 % 2 == 1) //己方唯一气，不能下
            {
                result[i][j] = 0;
                continue;
            }
            //啥都不是，能下
            result[i][j] = 1;
            r++;
        }
    }
    return r;
}

class treeNode
{
public:
    int board[9][9]; //棋盘,1为己方,-1为对方,0为空
    treeNode *parent;
    treeNode *children[branchNum];
    int childrenAction[branchNum][2];
    int childrenCount; //子节点个数
    int childrenCountMax;
    int q;     //回溯后获胜局数
    int n;     //访问次数
    int depth; //节点深度
    double score;
    treeNode(int parentBoard[9][9], int action[2], int d, treeNode *p)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                board[i][j] = -parentBoard[i][j]; //双方反转
            }
        }
        parent = p;
        board[action[0]][action[1]] = -1;
        q = 0;
        n = 0;
        depth = d;
        childrenCount = 0;
        evaluate();
    }
    treeNode *treePolicy()
    {
        if (childrenCountMax == 0)
        {
            return this;
        }
        if (childrenCountMax > childrenCount)
        {
            treeNode *c = new treeNode(board, childrenAction[childrenCount], depth + 1, this);
            children[childrenCount] = c;
            childrenCount++;
            return c;
        }
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
    int simulation() //返回由当前节点开始模拟的结果
    {
        int simulationBoard[9][9];
        int simulationBoardR[9][9]; //对方的棋盘
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                simulationBoard[i][j] = board[i][j];
                simulationBoardR[i][j] = -board[i][j];
            }
        }
        int r = result(simulationBoard);
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
            r = result(simulationBoard);
        }
        return r;
    }
    void backup(int delta)
    {
        treeNode *node = this;
        int d = 1;
        while (node != nullptr)
        {
            if (delta && d % 2)
            {
                node->q++;
            }
            node->n++;
            node->score = double(node->q) / double(node->n) + 0.707 * sqrt(double(node->depth) / double(node->n)); //得分公式
            node = node->parent;
            d++;
        }
    }

private:
    void evaluate()
    {
        //填充childrenAction、countMax
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
        if (validPositionCount > branchNum)
        {
            childrenCountMax = branchNum;
            int max[branchNum] = {1 << 31, 1 << 31, 1 << 31, 1 << 31};
            int maxAction[branchNum][2];
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (result[i][j])
                    {
                        board[i][j] = 1;
                        boardR[i][j] = -1; //模拟一步
                        int temp = getValidPositions(board, resultTemp) - getValidPositions(boardR, resultTemp);
                        for (int k = 0; k < branchNum; k++)
                        {
                            if (temp > max[k])
                            {
                                max[k] = temp;
                                maxAction[k][0] = i;
                                maxAction[k][1] = j;
                                break;
                            }
                            if (temp == max[k] && rand() < 5000) //六分之一的概率替换
                            {
                                maxAction[k][0] = i;
                                maxAction[k][1] = j;
                                break;
                            }
                        }
                        board[i][j] = 0;
                        boardR[i][j] = 0; //还原
                    }
                }
            }
            for (int i = 0; i < branchNum; i++)
            {
                childrenAction[i][0] = maxAction[i][0];
                childrenAction[i][1] = maxAction[i][1];
            }
        }
        else
        {
            for (int i = 0; i < 9; i++)
            {
                for (int j = 0; j < 9; j++)
                {
                    if (result[i][j])
                    {
                        childrenAction[childrenCountMax][0] = i;
                        childrenAction[childrenCountMax][1] = j;
                        childrenCountMax++;
                    }
                }
            }
        }
    }
    int result(int board[9][9])
    {
        //0棋局未结束
        //1棋局结束，当前方赢
        //-1棋局结束，当前方输
        int result[9][9];
        int boardR[9][9]; //对方视角的棋盘
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                boardR[i][j] = -board[i][j];
            }
        }
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
    int defaultPolicy(int board[9][9])
    {
        //返回落子坐标
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
        int validPositionCount = 0;
        getValidPositions(board, result);
        int actions[81][2];
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                if (result[i][j])
                {
                    actions[validPositionCount][0] = i;
                    actions[validPositionCount][1] = j;
                    validPositionCount++;
                }
            }
        }
        int i = rand() % validPositionCount;
        return actions[i][0] * 9 + actions[i][1];
    }
};

int main()
{
    string str;
	int x,y;
	// 读入JSON
	getline(cin,str);
    int start = clock();
	//getline(cin, str);
	Json::Reader reader;
	Json::Value input;
	reader.parse(str, input); 
    int board[9][9]={0};
    int turnID = input["responses"].size();
	for (int i = 0; i < turnID; i++) 
	{
		x=input["requests"][i]["x"].asInt(), y=input["requests"][i]["y"].asInt();
		if (x!=-1) board[x][y]=-1;
		x=input["responses"][i]["x"].asInt(), y=input["responses"][i]["y"].asInt();
		if (x!=-1) board[x][y]=1;
	}
    x=input["requests"][turnID]["x"].asInt(), y=input["requests"][turnID]["y"].asInt();
    int actionR[2] = {x, y};
    srand(233);
    treeNode root(board, actionR, 0, nullptr);
    while (clock()-start < 950 /*这里是停止搜索的条件*/)
    {
        treeNode *node = root.treePolicy();
        int result = node->simulation();
        node->backup(result == 1 ? 1 : 0);
    }
    int max = 0;
    int *bestAction = root.childrenAction[0];
    for (int i = 0; i < root.childrenCount; i++)
    {
        if (max < root.children[i]->n)
        {
            max = root.children[i]->n;
            bestAction = root.childrenAction[i];
        }
    }
    Json::Value ret;
	Json::Value action;
    action["x"]=bestAction[0]; action["y"]=bestAction[1];
	ret["response"] = action;
	Json::FastWriter writer;
    cout << writer.write(ret) << endl;
}