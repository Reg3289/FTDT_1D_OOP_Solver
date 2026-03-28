#include "../include/Grid1D.h"
#include "../include/Boundary.h"
#include "../include/Source.h"
#include "../include/BraggGrating.h"
#include "../include/FabryPerotCavity.h"
#include "../include/Simulation.h"
#include <omp.h> // 【新增】引入 OpenMP 库，用于控制线程数

int main() {
    // ==========================================
    // 1. 扩大计算规模 (提升压测强度)
    // 之前是 1000，现在扩大 100 倍，变成 100,000 个空间网格
    // ==========================================
    Grid1D grid(100000);

    // 器件部分保持不变（放在 400 的位置完全没问题）
    Device* myDevice = nullptr;
    FabryPerotCavity fp_cavity(400, 70, 30, 7, 2.0, 1.5);
    myDevice = &fp_cavity;

    if (myDevice) {
        myDevice->buildOnGrid(grid);
    }

    MurABC myBoundary;
    UnidirectionalSource mySource(100);

    // ==========================================
    // 2. 增加时间步数
    // 之前是 15000，现在提升到 50000 步
    // ==========================================
    Simulation engine(grid, 50000);
    engine.setBoundary(&myBoundary);
    engine.setSource(&mySource);
    engine.setProbes(250, 700);

    // ==========================================
    // 3. 【核心压测控制台】
    // ==========================================

    // 🔴 第一次测试：强制使用单线程 (Single-thread) 跑一次
   // omp_set_num_threads(1);

    // 🟢 第二次测试：使用所有可用的 CPU 核心跑一次 (多线程)
    // 你可以在做完第一次测试后，把上面那行注释掉，取消下面这行的注释
     omp_set_num_threads(omp_get_max_threads()); 

    // 开始仿真
    engine.run("fdtd_probes_results.csv");

    return 0;
}