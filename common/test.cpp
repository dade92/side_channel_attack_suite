#include<iostream>
#include"math.hpp"
using namespace std;
int main() {
    float z;
    while(1) {
        cin>>z;
        cout<<"phi:"<<phi(z)<<endl;
        cout<<"phi inv:"<<phi_inv(z)<<endl;
    }
}
