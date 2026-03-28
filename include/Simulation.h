#pragma once
#include "Grid1D.h"
#include "Boundary.h"
#include "Source.h"
#include <iostream>
#include <fstream>
#include <string>

class Simulation {
private:
    Grid1D& grid;
    int max_time;
    Boundary* boundary;
    Source* source;
    // 【新增】两个虚拟探针的位置
    int probe1_idx;
    int probe2_idx;

public:
    Simulation(Grid1D& g, int t) : grid(g), max_time(t), boundary(nullptr), source(nullptr), probe1_idx(-1), probe2_idx(-1) {}

    void setBoundary(Boundary* b) { boundary = b; }
    void setSource(Source* s) { source = s; }

    // 【新增】设置探针位置的接口
    void setProbes(int p1, int p2) {
        probe1_idx = p1;
        probe2_idx = p2;
    }

    void run(const std::string& filename) {
        std::ofstream outFile(filename);
        std::cout << "Starting OOP FDTD Engine with Time-Domain Probes..." << std::endl;

        // 【修改】写入 CSV 表头
        outFile << "TimeStep,Probe1_Ez,Probe2_Ez\n";

        for (int qTime = 0; qTime < max_time; ++qTime) {
            if (boundary) boundary->saveInternal(grid);

            for (int mm = 0; mm < grid.size - 1; ++mm) {
                grid.hy[mm] = grid.hy[mm] + (grid.ez[mm + 1] - grid.ez[mm]) / grid.IMP0;
            }

            if (source) source->injectH(grid, qTime);

            for (int mm = 1; mm < grid.size - 1; ++mm) {
                grid.ez[mm] = grid.ez[mm] + (grid.hy[mm] - grid.hy[mm - 1]) * grid.IMP0 / grid.eps_r[mm];
            }

            if (source) source->injectE(grid, qTime);
            if (boundary) boundary->apply(grid);

            // ==========================================
            // 【核心修改】只记录两个探针位置的随时间变化数据
            // ==========================================
            if (probe1_idx >= 0 && probe2_idx >= 0) {
                outFile << qTime << ","
                    << grid.ez[probe1_idx] << ","
                    << grid.ez[probe2_idx] << "\n";
            }

            if (qTime % 500 == 0) std::cout << "Step: " << qTime << " / " << max_time << std::endl;
        }
        outFile.close();
        std::cout << "Simulation Complete! Probe data written to: " << filename << std::endl;
    }
};