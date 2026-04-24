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

// Faster adjacency list using fixed arrays
const int MAXN = 3005;
const int MAXM = 9005; // 4500 * 2 edges for bidirectional

Edge edges[MAXM * 2];
int head[MAXN];
int next_edge[MAXM * 2];
int edge_cnt = 0;

inline void add_edge(int u, int v, int cap) {
    next_edge[edge_cnt] = head[u];
    edges[edge_cnt] = {v, cap, edge_cnt + 1};
    head[u] = edge_cnt++;

    next_edge[edge_cnt] = head[v];
    edges[edge_cnt] = {u, cap, edge_cnt - 1};
    head[v] = edge_cnt++;
}

int level[MAXN];
int ptr[MAXN];

bool bfs(int s, int t, int n) {
    for (int i = 1; i <= n; ++i) level[i] = -1;
    level[s] = 0;
    static int q[MAXN];
    int q_head = 0, q_tail = 0;
    q[q_tail++] = s;
    while (q_head < q_tail) {
        int v = q[q_head++];
        for (int i = head[v]; i != -1; i = next_edge[i]) {
            if (edges[i].cap > 0 && level[edges[i].to] == -1) {
                level[edges[i].to] = level[v] + 1;
                q[q_tail++] = edges[i].to;
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
    edge_cnt = 0;
    for (int i = 1; i <= n; ++i) head[i] = -1;
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

struct GHTreeEdge {
    int u, v, w;
};
bool compareGHTreeEdge(const GHTreeEdge& a, const GHTreeEdge& b) {
    return a.w > b.w;
}

struct DSU {
    vector<int> parent;
    vector<int> sz;
    DSU(int n) {
        parent.resize(n + 1);
        sz.resize(n + 1, 1);
        for (int i = 1; i <= n; ++i) parent[i] = i;
    }
    int find(int i) {
        if (parent[i] == i) return i;
        return parent[i] = find(parent[i]);
    }
    void unite(int i, int j) {
        int root_i = find(i);
        int root_j = find(j);
        if (root_i != root_j) {
            if (sz[root_i] < sz[root_j]) swap(root_i, root_j);
            parent[root_j] = root_i;
            sz[root_i] += sz[root_j];
        }
    }
};

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

    vector<int> node_p(n + 1, 1);
    vector<GHTreeEdge> tree_edges;
    tree_edges.reserve(n - 1);

    for (int i = 2; i <= n; ++i) {
        int s = i;
        int t = node_p[i];
        int flow = dinic(s, t, n);

        vector<bool> in_cut(n + 1, false);
        static int q[MAXN];
        int q_head = 0, q_tail = 0;
        q[q_tail++] = s;
        in_cut[s] = true;
        while (q_head < q_tail) {
            int v = q[q_head++];
            for (int e_idx = head[v]; e_idx != -1; e_idx = next_edge[e_idx]) {
                if (edges[e_idx].cap > 0 && !in_cut[edges[e_idx].to]) {
                    in_cut[edges[e_idx].to] = true;
                    q[q_tail++] = edges[e_idx].to;
                }
            }
        }

        for (int j = i + 1; j <= n; ++j) {
            if (node_p[j] == t && in_cut[j]) {
                node_p[j] = s;
            }
        }
        tree_edges.push_back({s, t, flow});
    }

    sort(tree_edges.begin(), tree_edges.end(), compareGHTreeEdge);

    DSU dsu(n);
    long long total_sum = 0;
    for (const auto& te : tree_edges) {
        total_sum += (long long)te.w * dsu.sz[dsu.find(te.u)] * dsu.sz[dsu.find(te.v)];
        dsu.unite(te.u, te.v);
    }

    cout << total_sum << endl;

    return 0;
}
