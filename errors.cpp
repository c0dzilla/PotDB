#include <iostream>

using namespace std;

struct error {
    string msg;
};

void throwError(error err) {
    cout << err.msg << '\n';
}