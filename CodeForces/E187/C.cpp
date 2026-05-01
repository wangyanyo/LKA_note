#include <iostream>
#include <algorithm>
using namespace std;

#define int long long

int s, m;

bool check(int x) {
    int sum = 0;
    for (int i = 63; i >= 0; --i) {
        int mask = 1ll << i;
        if (m & mask) {
            int t = !!(s & mask);
            if (t <= x) {
                sum -= (x - t);
                sum = max(sum, 0ll);
            } else {
                sum += (t - x);
            }
        } else {
            sum += !!(s & mask);
        }
        sum *= 2;
    }
    return sum == 0;
}

void solve()
{
    int l = 0, r = 1e18 + 5;
    while (l < r) {
        int mid = (l + r) >> 1;
        if (check(mid)) r = mid;
        else l = mid + 1;
    }
    if (l == 1e18 + 5) cout << -1 << endl;
    else cout << l << endl;
}

signed main()
{
    int tt;
    cin >> tt;
    while (tt--) {
        cin >> s >> m;
        solve();
    }
    return 0;
}