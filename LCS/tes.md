Input: Strings C and D
Output: Length of the longest common subsequence of C and D

1. Read C, D
2. If length(C) < length(D) then
       Swap C and D   // minimizes memory usage
3. Let dp[0..length(D)] ← 0
4. For i ← 1 to length(C) do
5.     prev ← 0
6.     For j ← 1 to length(D) do
7.         temp ← dp[j]           // store previous value
8.         If C[i-1] = D[j-1] then
9.             dp[j] ← prev + 1
10.        Else
11.            dp[j] ← max(dp[j], dp[j-1])
12.        prev ← temp
13. Return dp[length(D)]
