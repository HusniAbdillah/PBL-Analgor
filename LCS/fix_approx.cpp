#include <bits/stdc++.h>
using namespace std;

signed main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    string C, D;
    cin >> C >> D;
    if (C.size() < D.size()) swap(C, D);
    
    int approx = 0, pos = 0;
    for (char c : C){
        while (pos < D.size() && D[pos] != c) pos++;
        if (pos < D.size()) {
            approx++;
            pos++;
        }
    }

    cout << approx << '\n';
}
