#include<iostream>
#include"math.hpp"
using namespace std;
int main() {
    int x;
    while(1) {
        cin>>x;
        cout<<"Next power of 2 is:"<<next_two_power(x)<<endl;
    }
    return 0;
}
