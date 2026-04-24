#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <cstring>

using namespace std;

const int INF = 1e9;

struct Edge {
    int to, cap, rev;
};

vector<vector<Edge>> adj;
vector<int> level;
vector<int> ptr;

void add_edge(int from, int to, int cap) {
    adj[from].push_back({to, cap, (int)adj[to].size()});
    adj[to].push_back({from, cap, (int)adj[from].size() - 1});
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
            if (edge.cap > 0 && level[edge.to] == -1) {
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
    for (int& cid = ptr[v]; cid < (int)adj[v].size(); ++cid) {
        auto& edge = adj[v][cid];
        int tr = edge.to;
        if (level[v] + 1 != level[tr] || edge.cap == 0) continue;
        int push = dfs(tr, t, min(pushed, edge.cap));
        if (push == 0) continue;
        edge.cap -= push;
        adj[edge.to][edge.rev].cap += push;
        return push;
    }
    return 0;
}

int dinic(int s, int t) {
    int flow = 0;
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
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    vector<pair<int, int>> original_edges;
    original_edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        original_edges.push_back({u, v});
    }

    adj.resize(n + 1);
    level.resize(n + 1);
    ptr.resize(n + 1);

    vector<int> p(n + 1, 1);
    gh_tree.assign(n + 1, vector<GHEdge>());

    for (int i = 2; i <= n; ++i) {
        int s = i;
        int t = p[i];

        for (int j = 1; j <= n; ++j) adj[j].clear();
        for (const auto& edge : original_edges) {
            add_edge(edge.first, edge.second, 1);
        }

        int flow = dinic(s, t);

        vector<bool> in_cut(n + 1, false);
        queue<int> q;
        q.push(s);
        in_cut[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (auto& edge : adj[v]) {
                if (edge.cap > 0 && !in_cut[edge.to]) {
                    in_cut[edge.to] = true;
                    q.push(edge.to);
                }
            }
        }

        for (int j = i + 1; j <= n; ++j) {
            if (p[j] == t && in_cut[j]) {
                p[j] = s;
            }
        }
        gh_tree[s].push_back({t, flow});
        gh_tree[t].push_back({s, flow});
    }

    long long sum_flow = 0;
    for (int i = 1; i <= n; ++i) {
        vector<int> dists(n + 1, INF);
        get_all_pairs_flow(i, -1, INF, dists);
        for (int j = i + 1; j <= n; ++j) {
            if (dists[j] != INF) sum_flow += dists[j];
        }
    }

    cout << sum_flow << endl;

    return 0;
}
