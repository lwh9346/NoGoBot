#include <math.h>

class treeNode
{
public:
    int board[9][9]; //棋盘,1为己方,-1为对方,0为空
    treeNode *parent;
    treeNode *children[8];
    int childrenAction[8][2];
    int childrenCount; //子节点个数
    int childrenCountMax;
    int q;     //回溯后获胜局数
    int n;     //访问次数
    int depth; //节点深度
    double score;
    treeNode(int parentBoard[9][9], int action[2], int d)
    {
        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                board[i][j] = -parentBoard[i][j]; //双方反转
            }
        }
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
            treeNode c(board, childrenAction[childrenCount], depth + 1);
            childrenCount++;
            return &c;
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
                int *a = defaultPolicy(simulationBoard);
                simulationBoard[a[0]][a[1]] = 1;
                simulationBoardR[a[0]][a[1]] = -1;
            }
            else
            {
                int *a = defaultPolicy(simulationBoardR);
                simulationBoard[a[0]][a[1]] = -1;
                simulationBoardR[a[0]][a[1]] = 1;
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
    }
    int result(int board[9][9])
    {
        //0棋局未结束
        //1棋局结束，当前方赢
        //-1棋局结束，当前方输
    }
    int *defaultPolicy(int board[9][9])
    {
        //返回落子坐标
    }
};

int main()
{
    int board[9][9] = {0};
    int action[2] = {2, 2};
    treeNode root(board, action, 0);
    while (true /*这里是停止搜索的条件*/)
    {
        treeNode *node = root.treePolicy();
        int result = node->simulation();
        node->backup(result == 1 ? 1 : 0);
    }
    int max = 0;
    int *bestAction = action;
    for (int i = 0; i < root.childrenCount; i++)
    {
        if (max < root.children[i]->n)
        {
            max = root.children[i]->n;
            bestAction = root.childrenAction[i];
        }
    }
}