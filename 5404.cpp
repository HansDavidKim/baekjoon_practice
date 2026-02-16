#include <iostream>
#include <vector>
#include <algorithm>

using namespace std;

vector<int> get_point() {
    vector<int> point(11);
    for (int i = 0; i < 11; i++) cin >> point[i];
    return point;
}

vector<int> operator-(const vector<int>& a, const vector<int>& b) {
    vector<int> res(a.size());
    for (size_t i = 0; i < a.size(); i++)
        res[i] = (a[i] - b[i] + 11) % 11;
    return res;
}

int modinv11(int a) {
    a %= 11; if (a < 0) a += 11;
    for (int x = 1; x < 11; ++x) if ((a * x) % 11 == 1) return x;
    return 1; // a==0이면 호출되면 안 됨
}

struct Basis {
    // pivot[col] = basis vector whose leading nonzero is at col, else empty
    vector<vector<int>> pivot; // size 11, empty means none

    Basis(): pivot(11) {}

    // reduce v by current basis (does not modify basis)
    void reduce(vector<int>& v) const {
        for (int col = 0; col < 11; ++col) {
            if (v[col] == 0) continue;
            if (pivot[col].empty()) continue;
            int factor = v[col]; // pivot[col][col] == 1
            // v = v - factor * pivot[col]
            for (int k = col; k < 11; ++k) { // k<col는 이미 0 유지됨
                int val = v[k] - factor * pivot[col][k];
                val %= 11; if (val < 0) val += 11;
                v[k] = val;
            }
        }
    }

    // insert v into basis; returns true if independent (basis grew)
    bool insert(vector<int> v) {
        reduce(v);
        int p = -1;
        for (int col = 0; col < 11; ++col) {
            if (v[col] != 0) { p = col; break; }
        }
        if (p == -1) return false; // dependent

        // normalize so pivot becomes 1
        int inv = modinv11(v[p]);
        for (int k = p; k < 11; ++k) v[k] = (v[k] * inv) % 11;

        // eliminate this pivot column from other basis vectors
        for (int col = 0; col < 11; ++col) {
            if (col == p || pivot[col].empty()) continue;
            int factor = pivot[col][p];
            if (factor == 0) continue;
            for (int k = p; k < 11; ++k) {
                int val = pivot[col][k] - factor * v[k];
                val %= 11; if (val < 0) val += 11;
                pivot[col][k] = val;
            }
        }

        pivot[p] = std::move(v);
        return true;
    }

    // check if v is in span
    bool contains(vector<int> v) const {
        reduce(v);
        for (int col = 0; col < 11; ++col) if (v[col] != 0) return false;
        return true;
    }
};

void warp_driver() {
    int n; cin >> n;

    vector<int> start_point = get_point();
    vector<int> end_point   = get_point();
    vector<int> diff = end_point - start_point;

    vector<pair<int, vector<int>>> warps;
    warps.reserve(n);
    for (int i = 0; i < n; i++) {
        vector<int> v = get_point();
        int y; cin >> y;
        warps.push_back({y, v});
    }

    sort(warps.begin(), warps.end(),
         [](const auto& a, const auto& b){ return a.first < b.first; });

    Basis B;

    int i = 0;
    while (i < n) {
        int cur_year = warps[i].first;

        // 같은 연도 벡터들 먼저 basis에 전부 추가
        while (i < n && warps[i].first == cur_year) {
            B.insert(warps[i].second);
            i++;
        }

        // 그 연도 시점에서 도달 가능 여부 검사
        if (B.contains(diff)) {
            cout << cur_year << "\n";
            return;
        }
    }

    cout << "unreachable\n";
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    int T; cin >> T;
    while (T--) warp_driver();
    return 0;
}
