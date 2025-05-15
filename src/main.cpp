#include "../include/vector.hpp"

int main()
{
    // Create a custom vector
    Vector<int> vec;
    vec.push_back(5);
    int x = 10;
    vec.push_back(x);
    vec.push_back(-1);

    vec.print();

    Vector<int> vec2 = vec;
    vec2.print();
}