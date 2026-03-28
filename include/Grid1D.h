#pragma once
#include <vector>

class Grid1D {
public:
    int size;
    double IMP0;
    std::vector<double> ez;
    std::vector<double> hy;
    std::vector<double> eps_r;

    Grid1D(int s) : size(s), IMP0(377.0) {
        ez.assign(size, 0.0);
        hy.assign(size - 1, 0.0);
        eps_r.assign(size, 1.0);
    }
};