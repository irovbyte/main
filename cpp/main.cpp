#include <iostream>

using namespace std;

int like(int a, int b)
{
    return a + b;
}

int main(void)
{

    int x, y;

    cin >> x >> y;
    int z = like(x, y);
    cout << z;
    return 0;
}