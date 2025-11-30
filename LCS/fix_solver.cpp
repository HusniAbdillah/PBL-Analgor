#include <bits/stdc++.h>
using namespace std;

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string C, D;
    cin >> C >> D;
    if (C.size() < D.size()) swap(C, D);
    int n = C.size(), m = D.size();

    vector<int> dp(m + 1, 0);

    for (int i = 1; i <= n; ++i){
        int prev = 0;
        for (int j = 1; j <= m; ++j) {
            int temp = dp[j];
            if (C[i-1] == D[j-1])
                dp[j] = prev + 1;
            else
                dp[j] = max(dp[j], dp[j-1]);
            prev = temp;
        }
    }

    cout << dp[m] << '\n';
}
