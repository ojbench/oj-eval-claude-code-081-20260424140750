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

// Use a more compact adjacency list
vector<Edge> edges;
vector<int> head;
vector<int> next_edge;

void add_edge(int u, int v, int cap) {
    next_edge.push_back(head[u]);
    head[u] = edges.size();
    edges.push_back({v, cap, (int)edges.size() + 1});

    next_edge.push_back(head[v]);
    head[v] = edges.size();
    edges.push_back({u, cap, (int)edges.size() - 1});
}

vector<int> level;
vector<int> ptr;

bool bfs(int s, int t, int n) {
    fill(level.begin(), level.end(), -1);
    level[s] = 0;
    static queue<int> q;
    while(!q.empty()) q.pop();
    q.push(s);
    while (!q.empty()) {
        int v = q.front();
        q.pop();
        for (int i = head[v]; i != -1; i = next_edge[i]) {
            if (edges[i].cap > 0 && level[edges[i].to] == -1) {
                level[edges[i].to] = level[v] + 1;
                q.push(edges[i].to);
                if (edges[i].to == t) return true;
            }
        }
    }
    return level[t] != -1;
}

int dfs(int v, int t, int pushed) {
    if (pushed == 0 || v == t) return pushed;
    for (int& i = ptr[v]; i != -1; i = next_edge[i]) {
        int tr = edges[i].to;
        if (level[v] + 1 != level[tr] || edges[i].cap == 0) continue;
        int push = dfs(tr, t, min(pushed, edges[i].cap));
        if (push == 0) continue;
        edges[i].cap -= push;
        edges[edges[i].rev].cap += push;
        return push;
    }
    return 0;
}

struct RawEdge {
    int u, v;
};
vector<RawEdge> raw_edges;

int dinic(int s, int t, int n) {
    edges.clear();
    next_edge.clear();
    fill(head.begin(), head.end(), -1);
    for (const auto& re : raw_edges) {
        add_edge(re.u, re.v, 1);
    }

    int flow = 0;
    while (bfs(s, t, n)) {
        for (int i = 1; i <= n; ++i) ptr[i] = head[i];
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

void get_dists(int u, int p, int min_w, vector<int>& dists) {
    for (const auto& edge : gh_tree[u]) {
        if (edge.to != p) {
            int current_min = min(min_w, edge.weight);
            dists[edge.to] = current_min;
            get_dists(edge.to, u, current_min, dists);
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);

    int n, m;
    if (!(cin >> n >> m)) return 0;

    raw_edges.reserve(m);
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        raw_edges.push_back({u, v});
    }

    head.assign(n + 1, -1);
    level.resize(n + 1);
    ptr.resize(n + 1);
    gh_tree.assign(n + 1, vector<GHEdge>());

    vector<int> node_p(n + 1, 1);
    for (int i = 2; i <= n; ++i) {
        int s = i;
        int t = node_p[i];
        int flow = dinic(s, t, n);

        vector<bool> in_cut(n + 1, false);
        queue<int> q;
        q.push(s);
        in_cut[s] = true;
        while (!q.empty()) {
            int v = q.front();
            q.pop();
            for (int e_idx = head[v]; e_idx != -1; e_idx = next_edge[e_idx]) {
                if (edges[e_idx].cap > 0 && !in_cut[edges[e_idx].to]) {
                    in_cut[edges[e_idx].to] = true;
                    q.push(edges[e_idx].to);
                }
            }
        }

        for (int j = i + 1; j <= n; ++j) {
            if (node_p[j] == t && in_cut[j]) {
                node_p[j] = s;
            }
        }
        gh_tree[s].push_back({t, flow});
        gh_tree[t].push_back({s, flow});
    }

    long long sum_flow = 0;
    for (int i = 1; i <= n; ++i) {
        vector<int> dists(n + 1, INF);
        get_dists(i, -1, INF, dists);
        for (int j = i + 1; j <= n; ++j) {
            if (dists[j] != INF) sum_flow += dists[j];
        }
    }

    cout << sum_flow << endl;

    return 0;
}
