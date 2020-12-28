#include "bot.cpp"
#include <stdio.h>

int main() {
    double timeout;
    scanf("%lf",&timeout);
    signed char board[81];
    for (int i = 0; i < 81; i++) {
        scanf("%d", &board[i]);
    }
    int action=mcts::GetBestAction(board,timeout,nullptr);
    printf("%d",action);
}