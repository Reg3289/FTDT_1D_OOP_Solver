#include "../include/Grid1D.h"
#include "../include/Boundary.h"
#include "../include/Source.h"
#include "../include/BraggGrating.h"
#include "../include/FabryPerotCavity.h" // 引入新器件
#include "../include/Simulation.h"

int main() {
    Grid1D grid(1000);

    // ==========================================
    // 工业级软件的体现：面向接口编程
    // 我们声明一个 Device 指针，它可以指向任何光学器件！
    // ==========================================
    Device* myDevice = nullptr;

    // 选项 A：测试单个光栅
    // BraggGrating grating(400, 540, 7, 2.0, 1.5);
    // myDevice = &grating;

    // 选项 B：测试 FP 谐振腔
    // 起点 400, 两侧镜子各长 70 (即 5 对周期), 中间空出 30 个网格作为谐振腔, 单层厚度 7
    FabryPerotCavity fp_cavity(400, 70, 30, 7, 2.0, 1.5);
    myDevice = &fp_cavity;

    // 统一调用烧录接口 (多态威力)
    if (myDevice) {
        myDevice->buildOnGrid(grid);
    }

    // 剩下的引擎配置毫无变化
    MurABC myBoundary;
    UnidirectionalSource mySource(100);

    Simulation engine(grid, 15000);
    engine.setBoundary(&myBoundary);
    engine.setSource(&mySource);
    engine.setProbes(250, 700);

    engine.run("fdtd_probes_results.csv");

    return 0;
}