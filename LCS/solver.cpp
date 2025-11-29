#include <bits/stdc++.h>
using namespace std;

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string A, B;
    cin >> A >> B;
    if (A.size() < B.size()) swap(A, B); // optimasi memori
    int n = A.size(), m = B.size();

    vector<int> dp(m + 1, 0);

    for (int i = 1; i <= n; ++i){
        int prev = 0; // simpan nilai diagonal (i-1, j-1)
        for (int j = 1; j <= m; ++j) {
            int temp = dp[j]; // simpan nilai atas (i-1, j)
            if (A[i-1] == B[j-1])
                dp[j] = prev + 1;
            else
                dp[j] = max(dp[j], dp[j-1]);
            prev = temp; // update diagonal
        }
    }

    cout << dp[m] << '\n';
}
