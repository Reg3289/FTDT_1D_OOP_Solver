#pragma once
#include "Grid1D.h"
#include "Source.h"
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <omp.h>

class Simulation {
private:
    Grid1D& grid;
    int max_time;
    Source* source;
    int probe1_idx;
    int probe2_idx;

public:
    // 【修改】移除了 Boundary 指针，现在一切由底层网格系数决定！
    Simulation(Grid1D& g, int t) : grid(g), max_time(t), source(nullptr), probe1_idx(-1), probe2_idx(-1) {}

    void setSource(Source* s) { source = s; }
    void setProbes(int p1, int p2) { probe1_idx = p1; probe2_idx = p2; }

    void run(const std::string& filename) {
        std::ofstream outFile(filename);
        std::cout << "Starting OOP FDTD Engine with PML & OpenMP..." << std::endl;
        outFile << "TimeStep,Probe1_Ez,Probe2_Ez\n";

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int qTime = 0; qTime < max_time; ++qTime) {
            // ==========================================
            // 【核心升级】由预计算的系数矩阵驱动磁场更新
            // ==========================================
#pragma omp parallel for
            for (int mm = 0; mm < grid.size - 1; ++mm) {
                grid.hy[mm] = grid.ch_a[mm] * grid.hy[mm] + grid.ch_b[mm] * (grid.ez[mm + 1] - grid.ez[mm]);
            }

            if (source) source->injectH(grid, qTime);

            // ==========================================
            // 【核心升级】由预计算的系数矩阵驱动电场更新
            // ==========================================
#pragma omp parallel for
            for (int mm = 1; mm < grid.size - 1; ++mm) {
                grid.ez[mm] = grid.ce_a[mm] * grid.ez[mm] + grid.ce_b[mm] * (grid.hy[mm] - grid.hy[mm - 1]);
            }

            if (source) source->injectE(grid, qTime);

            // 注意：这里删除了原来累赘的 boundary->apply() 代码！

            if (probe1_idx >= 0 && probe2_idx >= 0) {
                outFile << qTime << "," << grid.ez[probe1_idx] << "," << grid.ez[probe2_idx] << "\n";
            }
            if (qTime % 1000 == 0) std::cout << "Step: " << qTime << " / " << max_time << std::endl;
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double> elapsed = end_time - start_time;
        outFile.close();
        std::cout << "Simulation Complete! Total Time: " << elapsed.count() << " seconds." << std::endl;
    }
};