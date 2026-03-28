#pragma once
#include "Grid1D.h"

class Boundary {
public:
    virtual ~Boundary() {}
    virtual void saveInternal(const Grid1D& grid) {}
    virtual void apply(Grid1D& grid) = 0;
};

class MurABC : public Boundary {
private:
    double ez_1_old;
    double ez_SIZE_2_old;
public:
    MurABC() : ez_1_old(0.0), ez_SIZE_2_old(0.0) {}

    void saveInternal(const Grid1D& grid) override {
        ez_1_old = grid.ez[1];
        ez_SIZE_2_old = grid.ez[grid.size - 2];
    }

    void apply(Grid1D& grid) override {
        grid.ez[0] = ez_1_old;
        grid.ez[grid.size - 1] = ez_SIZE_2_old;
    }
};