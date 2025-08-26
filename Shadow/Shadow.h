#ifndef SHADOW_H
#define SHADOW_H
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <algorithm>
#include <ctime>
#include <chrono>
#include <thread>
#include <set>
#include <cmath>
using namespace std;
class Shadow
{
public:
    Shadow(string filepath);
    virtual ~Shadow();
    vector<vector<double>> Equations(string pointsPath);

protected:

private:
    vector<vector<double>> shadowPiecewise;
};

#endif // SHADOW_H
