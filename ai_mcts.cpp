//ai_mcts.h
#ifndef AI_MCTS_H
#define AI_MCTS_H

#include <iostream>
#include <vector>
#include <cmath>
#include <ctime>
#include <cstdlib>
#include <stack>
using namespace std;

class AI_mcts
{
    static const int columnNum = 15;
    static constexpr const double C = 1.0;//探索常数
    static const int SIMULATIONS = 1000;//模拟次数
    static constexpr const int dx[] = {-1, 1, 0, 0};
    static constexpr const int dy[] = {0, 0, 1, -1};
    bool st[columnNum][columnNum];

public:
    AI_mcts() {}
    class Node {
    public:
        Node* parent;
        vector<Node*> children;
        int visit_count;
        int win_count;
        int state[columnNum][columnNum];
        int player;
        int x, y;

        Node(Node* parent, int state[][columnNum], int player, int x, int y) : parent(parent), visit_count(0), win_count(0), player(player), x(x), y(y) {
            memcpy(this->state, state, columnNum * columnNum * sizeof(int));
        }

        ~Node() {
            for (Node* child : children) {
                delete child;
            }
        }
    };
    struct dfs_state{
        int a, b;
        int state[columnNum][columnNum];
        dfs_state(int x, int y, int state[][columnNum]){
            this->a = x; this->b = y;
            memcpy(this->state, state, sizeof state);
        }
    };
    bool inBoard(int x, int y);
    void dfs(int x, int y, int state[][columnNum]);
    int checkWin(int x, int y, int state[][columnNum]);
    vector<pair<int, int>> getLegalActions(int state[][columnNum]);
    void updateState(int x, int y, int player, int state[][columnNum]);
    Node* select(Node* node);
    void expand(Node* node);
    int simulate(Node* node);
    void backpropagate(Node* node, int winner);
    pair<int, int> mcts(int state[][columnNum], int player, int x, int y);
};

#endif // AI_MCTS_H
