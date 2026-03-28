#pragma once
#include "Grid1D.h"
#include <cmath>

class Source {
public:
    int position;
    Source(int pos) : position(pos) {}
    virtual ~Source() {}
    virtual void injectH(Grid1D& grid, int qTime) = 0;
    virtual void injectE(Grid1D& grid, int qTime) = 0;
};

class UnidirectionalSource : public Source {
public:
    UnidirectionalSource(int pos) : Source(pos) {}

    double getPulse(int qTime) {
        double arg = (qTime + 1.0) - 40.0;
        return std::exp(-std::pow(arg, 2) / 25.0);
    }

    void injectH(Grid1D& grid, int qTime) override {
        grid.hy[position] -= getPulse(qTime) / grid.IMP0;
    }

    void injectE(Grid1D& grid, int qTime) override {
        grid.ez[position] += getPulse(qTime);
    }
};