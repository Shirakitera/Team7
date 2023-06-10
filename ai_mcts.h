//ai_mcts.cpp
#include "ai_mcts.h"

bool is_dead = 1;
bool AI_mcts::inBoard(int x, int y) {
    return x >= 0 && x < columnNum && y >= 0 && y < columnNum;
}

void AI_mcts::dfs(int x, int y, int state[][columnNum]){
    stack<dfs_state> stk;
    stk.push(dfs_state(x, y, state));
    st[x][y] = 1;

    while(!stk.empty()){
        dfs_state cur = stk.top(); stk.pop();
        int x = cur.a, y = cur.b;
        for(int i = 0; i < 4; i++){
            int nx = x + dx[i], ny = y + dy[i];
            if(nx < 0 || nx >= columnNum || ny < 0 || ny >= columnNum || st[nx][ny]) continue;
            if(state[nx][ny] + state[x][y] == 3) continue;

            if(state[nx][ny] == 0){
                is_dead = 0; return;
            }
            else{
                st[nx][ny] = 1;
                stk.push(dfs_state(nx, ny, state));
            }
        }
    }
}
// 判断游戏是否结束
int AI_mcts::checkWin(int x, int y, int state[][columnNum]) {
    // 根据你的游戏逻辑实现
    int player = state[x][y];
    is_dead = 1;
    dfs(x, y, state); memset(st, 0, sizeof st);
    if(is_dead) return player;

    for(int i = 0; i < 4; i++){
        int nx = x + dx[i], ny = y + dy[i];
        if(!state[nx][ny] || state[nx][ny] == state[x][y]) continue;

        if(state[nx][ny] + state[x][y] == 3){
            is_dead = 1;
            dfs(nx, ny, state); memset(st, 0, sizeof st);
            if(is_dead) return 3 - player;
        }
    }
    return 0;
}

// 获取当前玩家的合法行动列表
vector<pair<int, int>> AI_mcts::getLegalActions(int state[][columnNum]) {
    // 根据你的游戏逻辑实现
    vector<pair<int, int>> legal_actions;
    for (int i = 0; i < columnNum; i++) {
        for (int j = 0; j < columnNum; j++) {
            if (state[i][j] == 0) {
                legal_actions.push_back({i, j});
            }
        }
    }
    return legal_actions;
}

// 更新棋盘状态
void AI_mcts::updateState(int x, int y, int player, int state[][columnNum]) {
    // 根据你的游戏逻辑实现
    state[x][y] = player;
}

AI_mcts::Node* AI_mcts::select(AI_mcts::Node* node) {
    while (!node->children.empty()) {
        double max_ucb1 = -1e9;
        Node* best_child = nullptr;
        for (Node* child : node->children) {
            double ucb1 = (double)child->win_count / child->visit_count + C * sqrt(log(node->visit_count) / child->visit_count);
            if (ucb1 > max_ucb1) {
                max_ucb1 = ucb1;
                best_child = child;
            }
        }
        node = best_child;
    }
    return node;
}

void AI_mcts::expand(Node* node) {
    vector<pair<int, int>> legal_actions = getLegalActions(node->state);
    for (const auto& action : legal_actions) {
        int new_state[columnNum][columnNum];
        memcpy(new_state, node->state, columnNum * columnNum * sizeof(int));
        updateState(action.first, action.second, node->player, new_state);
        Node* child = new Node(node, new_state, 3 - node->player, action.first, action.second);
        node->children.push_back(child);
    }
}

int AI_mcts::simulate(Node* node) {
    int state[columnNum][columnNum];
    memcpy(state, node->state, columnNum * columnNum * sizeof(int));
    int player = node->player;
    while (true) {
        vector<pair<int, int>> legal_actions = getLegalActions(state);
        if (legal_actions.empty()) {
            return 0; // 平局
        }
        int random_index = rand() % legal_actions.size();
        int x = legal_actions[random_index].first;
        int y = legal_actions[random_index].second;
        updateState(x, y, player, state);
        int winner = checkWin(x, y, state);
        if (winner != 0) {
            return winner;
        }
        player = 3 - player;
    }
}

void AI_mcts::backpropagate(Node* node, int winner) {
    while (node != nullptr) {
        node->visit_count++;
        if (winner == node->player) {
            node->win_count++;
        }
        node = node->parent;
    }
}

pair<int, int> AI_mcts::mcts(int state[][columnNum], int player, int x, int y) {
    Node* root = new Node(nullptr, state, player, x, y);

    for (int i = 0; i < SIMULATIONS; i++) {
        // 选择（Selection）
        Node* selected_node = select(root);

        // 扩展（Expansion）
        if (checkWin(selected_node->x, selected_node->y, selected_node->state) == 0) {
            expand(selected_node);
        }

        // 模拟（Simulation）
        int winner = simulate(selected_node);

        // 回传（Backpropagation）
        backpropagate(selected_node, winner);
    }

    // 选择根节点的子节点中访问次数最多的节点作为最佳行动
    int max_visits = -1;
    pair<int, int> best_action;
    for (Node* child : root->children) {
        if (child->visit_count > max_visits) {
            max_visits = child->visit_count;
            for (int i = 0; i < columnNum; i++) {
                for (int j = 0; j < columnNum; j++) {
                    if (child->state[i][j] != root->state[i][j]) {
                        best_action = {i, j};
                        break;
                    }
                }
            }
        }
    }

    delete root;
    return best_action;
}
