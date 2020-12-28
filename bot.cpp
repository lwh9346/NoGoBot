#include "gameRule.cpp"
#include <cmath>
#include <ctime>
using namespace std;

class mcts {
private:
    struct TreeNode {
        TreeNode *parent;
        TreeNode *children[81];
        signed char board[81];
        int childrenAction[81];
        int childrenCount;
        int childrenCountMax;
        double q; //位于+-n之间的实数
        int n;
        int *totalN;
        gameRule::ValidPositionResult res;
    };

    //action是上一把的家伙下的棋
    static TreeNode *createNewTreeNode(TreeNode *parent, int action) {
        TreeNode *t = new TreeNode;
        t->q = 0.0;
        t->n = 0;
        t->parent = parent;
        t->childrenCount = 0;
        t->totalN = parent->totalN;
        for (int i = 0; i < 81; i++) {
            t->board[i] = -parent->board[i];
        }
        t->board[action] = -1;
        gameRule::GetValidPosition(t->board, &t->res);
        t->childrenCountMax = 0;
        for (int i = 0; i < 81; i++) {
            if (t->res.resS[i] == 1) {
                t->childrenAction[t->childrenCountMax] = i;
                t->childrenCountMax++;
            }
        }
        return t;
    }

    //棋盘不需要反转
    //totalN是计数器
    static TreeNode *createRootTreeNode(signed char board[81], int *totalN) {
        TreeNode *t = new TreeNode;
        t->q = 0.0;
        t->n = 0;
        t->parent = nullptr;
        t->childrenCount = 0;
        t->totalN = totalN;
        for (int i = 0; i < 81; i++) {
            t->board[i] = board[i];
        }
        gameRule::GetValidPosition(t->board, &t->res);
        t->childrenCountMax = 0;
        for (int i = 0; i < 81; i++) {
            if (t->res.resS[i] == 1) {
                t->childrenAction[t->childrenCountMax] = i;
                t->childrenCountMax++;
            }
        }
        return t;
    }

    static TreeNode *treePolicy(TreeNode *t) {
        if (t->childrenCountMax == 0) {
            return t;
        }
        if (t->childrenCount < t->childrenCountMax) {
            TreeNode *c = createNewTreeNode(t, t->childrenAction[t->childrenCount]);
            t->children[t->childrenCount] = c;
            t->childrenCount++;
            return c;
        }
        double maxScore = -1.0;
        int maxI = 0;
        for (int i = 0; i < t->childrenCount; i++) {
            double s = t->children[i]->q / double(t->children[i]->n) + 0.2 * sqrt(log(double(*t->totalN)) / double(t->children[i]->n)); //UCT公式
            if (s > maxScore) {
                maxScore = s;
                maxI = i;
            }
        }
        return treePolicy(t->children[maxI]);
    }

    static double defaultPolicy(TreeNode *t) {
        if (t->res.numR + t->res.numS == 0) {
            return 0.0;
        }
        if (t->res.numR == 0) {
            return 1.0;
        }
        if (t->res.numS == 0) {
            return -1.0;
        }
        return tanh(double(t->res.numS - t->res.numR) / double(t->res.numR + t->res.numS));
    }

    static void backup(double delta, TreeNode *t) {
        int i = 0;
        while (t != nullptr) {
            t->q += i % 2 == 1 ? delta : -delta;
            t->n++;
            t = t->parent;
            i++;
        }
    }

    static void deleteTree(TreeNode *t) {
        for (int i = 0; i < t->childrenCount; i++) {
            if (t->children[i]->childrenCount == 0) {
                delete (t->children[i]);
            } else {
                deleteTree(t->children[i]);
            }
        }
        delete (t);
    }

public:
    //调试数据
    //在函数外声明后传指针给函数即可获取调试数据
    struct DebugData {
        double winningRate; //胜率（不准确）
        int nodeCount;      //mcts搜索的节点数
    };

    //获取当前棋盘下的最佳行动
    //输入的棋盘不需要正反方反转，但是需要xy反转(不然调试的时候很难受)
    //+1是己方，0是空格，-1是对方
    //时限单位为秒，超过时限后才会停止
    //debug是调试信息，在外部新建一个对象后传指针进来即可
    //如果不需要调试信息的话，可以传nullptr进来
    //该函数执行完毕后会自动释放内存
    //返回值为x*9+y
    static int GetBestAction(signed char board[81], double timeOut, DebugData *debug) {
        int totalN = 0;
        int end = (int)(timeOut * double(CLOCKS_PER_SEC)) + clock();
        TreeNode *root = createRootTreeNode(board, &totalN);
        while (clock() < end) {
            //为了减少获取时间的性能开销，执行16步以后再获取时间
            for (int i = 0; i < 16; i++) {
                totalN++;
                TreeNode *t = treePolicy(root);
                double delta = defaultPolicy(t);
                backup(delta, t);
            }
        }
        int maxN = root->children[0]->n;
        int maxI = 0;
        for (int i = 1; i < root->childrenCount; i++) {
            if (root->children[i]->n > maxN) {
                maxI = i;
                maxN = root->children[i]->n;
            }
        }
        int bestAction = root->childrenAction[maxI];
        if (debug != nullptr) {
            debug->nodeCount = totalN;
            debug->winningRate = (root->children[maxI]->q / double(root->children[maxI]->n) + 1) * 0.5;
        }
        deleteTree(root);
        return bestAction;
    }
};