#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>

using namespace std;

const int INF = 1e9;

struct Edge {
    int to, cap, flow, rev;
};

vector<vector<Edge>> adj;
vector<int> level;
vector<int> ptr;

void add_edge(int from, int to, int cap) {
    adj[from].push_back({to, cap, 0, (int)adj[to].size()});
    adj[to].push_back({from, cap, 0, (int)adj[from].size() - 1});
}

bool bfs(int s, int t) {
    fill(level.begin(), level.end(), -1);
    level[s] = 0;
    queue<int> q;
    q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (auto& edge : adj[v]) {
            if (edge.cap - edge.flow > 0 && level[edge.to] == -1) {
                level[edge.to] = level[v] + 1;
                q.push(edge.to);
            }
        }
    }
    return level[t] != -1;
}

int dfs(int v, int t, int pushed) {
    if (pushed == 0) return 0;
    if (v == t) return pushed;
    for (int& cid = ptr[v]; cid < adj[v].size(); ++cid) {
        auto& edge = adj[v][cid];
        int tr = edge.to;
        if (level[v] + 1 != level[tr] || edge.cap - edge.flow == 0) continue;
        int push = dfs(tr, t, min(pushed, edge.cap - edge.flow));
        if (push == 0) continue;
        edge.flow += push;
        adj[tr][edge.rev].flow -= push;
        return push;
    }
    return 0;
}

int dinic(int s, int t) {
    int flow = 0;
    for (auto& row : adj) {
        for (auto& edge : row) {
            edge.flow = 0;
        }
    }
    while (bfs(s, t)) {
        fill(ptr.begin(), ptr.end(), 0);
        while (int pushed = dfs(s, t, INF)) {
            flow += pushed;
        }
    }
    return flow;
}

struct GHEdge {
    int to, weight;
};

vector<vector<GHEdge>> gh_tree;
void add_gh_edge(int u, int v, int w) {
    gh_tree[u].push_back({v, w});
    gh_tree[v].push_back({u, w});
}

long long total_max_flow = 0;
int n_nodes;

void get_all_pairs_flow(int u, int p, int min_w, vector<int>& dists) {
    for (auto& edge : gh_tree[u]) {
        if (edge.to != p) {
            int current_min = min(min_w, edge.weight);
            dists[edge.to] = current_min;
            get_all_pairs_flow(edge.to, u, current_min, dists);
        }
    }
}

int main() {
    int n, m;
    if (!(cin >> n >> m)) return 0;
    n_nodes = n;
    adj.assign(n + 1, vector<Edge>());
    level.resize(n + 1);
    ptr.resize(n + 1);

    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        add_edge(u, v, 1);
    }

    vector<int> parent(n + 1, 1);
    gh_tree.assign(n + 1, vector<GHEdge>());

    for (int i = 2; i <= n; ++i) {
        int s = i;
        int t = parent[i];
        int flow = dinic(s, t);

        add_gh_edge(s, t, flow);

        vector<bool> in_cut(n + 1, false);
        queue<int> q;
        q.push(s);
        in_cut[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto& edge : adj[v]) {
                if (edge.cap - edge.flow > 0 && !in_cut[edge.to]) {
                    in_cut[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }

        for (int j = i + 1; j <= n; ++j) {
            if (parent[j] == t && in_cut[j]) {
                parent[j] = s;
            }
        }
    }

    long long sum_flow = 0;
    for (int i = 1; i <= n; ++i) {
        vector<int> dists(n + 1, INF);
        get_all_pairs_flow(i, -1, INF, dists);
        for (int j = i + 1; j <= n; ++j) {
            sum_flow += dists[j];
        }
    }

    cout << sum_flow << endl;

    return 0;
}
