#include "Dynamics_System.hpp"

#include <fstream>
#include <iostream>
#include <memory>

#ifdef ENABLE_VISUALIZATION
    #include "Visualization.hpp"

    #include <chrono>
    #include <thread>
#endif

int main(int argc, char const* argv[]) {
    Dynamics_Sys* sys;
    std::shared_ptr<Ground> Ground_Body;
    JointPtr Rev_joint_1;
    JointPtr Rev_joint;

    BodyPtr Body_1;
    BodyPtr Body_prev;
    BodyPtr Body_now;
    double mass = 1.0;
    double time = 0.;
    double dt = 0.001;

    std::ofstream fout;

    fout.open("data.csv");

    sys = new Dynamics_Sys(dt);

    arma::vec pi = {0., 0., 0.};
    arma::vec pj = {0., 1., 0.};
    arma::vec qi = {0., 0., 0.};
    arma::vec qj = {0., 0., 0.};
    arma::vec POS = {0., 0., 0.};
    arma::vec VEL = {0., 0., 0.};
    arma::vec VEL1 = {0., 0., 0.};
    arma::vec ACC = {0., 0., 0.};
    arma::vec ANG = {-0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG1 = {-3. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0, -0. * 3.1415926 / 180.0};
    arma::vec ANG_VEL = {0., 0., 0.};
    arma::vec ANG_VEL1 = {0., 0., 0.};
    arma::vec ANG_ACC = {0., 0., 0.};
    arma::vec I = {1., 1., 1.};
    arma::vec F = {0., -9.8, 0.};
    arma::vec T = {0., 0., 0.};
    arma::vec T1 = {0., 0., 0.};

    Ground_Body = std::make_shared<Ground>(0);
    Body_1 =
        std::make_shared<Mobilized_body>(1, POS, VEL, ACC, ANG, ANG_VEL, ANG_ACC, mass, I, F, T);
    Rev_joint_1 = std::make_shared<Joint>(0, pi, pj, qi, qj, Ground_Body, Body_1);
    sys->Add(Ground_Body);
    sys->Add(Body_1);
    sys->Add(Rev_joint_1);

    Body_prev = Body_1;
    for (unsigned int i = 0; i < 10; i++) {
        Body_now = std::make_shared<Mobilized_body>(i + 2, POS, VEL1, ACC, ANG1, ANG_VEL1, ANG_ACC,
                                                    mass, I, F, T);
        Rev_joint = std::make_shared<Joint>(0, pi, pj, qi, qj, Body_prev, Body_now);

        sys->Add(Body_now);
        sys->Add(Rev_joint);
        Body_prev = Body_now;
    }
    sys->Assembly();
    sys->init();

#ifdef ENABLE_VISUALIZATION
    Visualization viz(800, 600);
    bool use_visualization = viz.initialize();
    if (!use_visualization) {
        std::cout << "Visualization disabled, running simulation only..." << std::endl;
    } else {
        std::cout << "=== 視覺化控制說明 / Visualization Controls ===" << std::endl;
        std::cout << "滑鼠移動 / Mouse Move: 旋轉視角 / Rotate camera" << std::endl;
        std::cout << "滑鼠滾輪 / Mouse Wheel: 縮放 / Zoom in/out" << std::endl;
        std::cout << "WASD: 移動目標點 / Move target point" << std::endl;
        std::cout << "Q/E: 上下移動目標點 / Move target up/down" << std::endl;
        std::cout << "+/-: 縮放 / Zoom in/out" << std::endl;
        std::cout << "R: 重置視角 / Reset camera view" << std::endl;
        std::cout << "ESC: 退出 / Exit" << std::endl;
        std::cout << "===============================================" << std::endl;
    }
#endif

    for (unsigned int i = 0; i < 50000; i++) {
        sys->solve();

        time += dt;

        if (i % 1 == 0) {
            fout << time << '\t';
            sys->output_data(fout);
        }

#ifdef ENABLE_VISUALIZATION
        if (use_visualization && i % 10 == 0) {
            viz.processInput();
            if (viz.shouldClose())
                break;

            viz.clear();

            std::vector<arma::vec> positions = sys->get_body_positions();
            std::vector<arma::vec> angles = sys->get_body_angles();

            viz.drawChain(positions);

            for (unsigned int k = 0; k < positions.size(); k++) {
                viz.drawBody(positions[k], angles[k], 1.0);
            }
            viz.swapBuffers();

            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
#endif
    }

#ifdef ENABLE_VISUALIZATION
    if (use_visualization) {
        viz.cleanup();
    }
#endif

    fout.close();
    return 0;
}
