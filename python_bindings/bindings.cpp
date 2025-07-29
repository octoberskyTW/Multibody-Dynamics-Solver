#include "Body.hpp"
#include "Dynamics_System.hpp"
#include "Joint.hpp"
#include "Math.hpp"

#include <memory>
#include <pybind11/numpy.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#ifdef ENABLE_VISUALIZATION
    #include "Visualization.hpp"
#endif

namespace py = pybind11;

// Enable std::shared_ptr as a holder type
PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

// Helper function to convert arma::vec to Python list
std::vector<double> arma_vec_to_vector(const arma::vec& v) {
    return std::vector<double>(v.begin(), v.end());
}

// Helper function to convert Python list to arma::vec
arma::vec vector_to_arma_vec(const std::vector<double>& v) {
    return arma::vec(v);
}

PYBIND11_MODULE(multibody_solver, m) {
    m.doc() = "Multibody Dynamics Solver Python Bindings";

    // Bind arma::vec conversion functions
    m.def("vec_to_list", &arma_vec_to_vector, "Convert arma::vec to Python list");
    m.def("list_to_vec", &vector_to_arma_vec, "Convert Python list to arma::vec");

    // Bind Body hierarchy
    py::class_<Body, std::shared_ptr<Body>>(m, "Body");
    py::class_<Ground, Body, std::shared_ptr<Ground>>(m, "Ground");
    py::class_<Mobilized_body, Body, std::shared_ptr<Mobilized_body>>(m, "MobilizedBody");
    py::class_<Joint, std::shared_ptr<Joint>>(m, "Joint");

    // Bind Dynamics_Sys class (most important)
    py::class_<Dynamics_Sys>(m, "DynamicsSystem")
        .def(py::init<double>())
        .def("get_nbody", &Dynamics_Sys::get_nbody)
        .def("get_njoint", &Dynamics_Sys::get_njoint)
        .def("Assembly", &Dynamics_Sys::Assembly)
        .def("init", &Dynamics_Sys::init)
        .def("solve", &Dynamics_Sys::solve)
        .def("get_body_positions",
             [](Dynamics_Sys& self) {
                 auto positions = self.get_body_positions();
                 std::vector<std::vector<double>> result;
                 for (const auto& pos : positions) {
                     result.push_back(arma_vec_to_vector(pos));
                 }
                 return result;
             })
        .def("get_body_angles",
             [](Dynamics_Sys& self) {
                 auto angles = self.get_body_angles();
                 std::vector<std::vector<double>> result;
                 for (const auto& ang : angles) {
                     result.push_back(arma_vec_to_vector(ang));
                 }
                 return result;
             })
        // Add system methods that take the created objects
        .def("AddBody", [](Dynamics_Sys& self, std::shared_ptr<Body> body) { self.Add(body); })
        .def("AddJoint", [](Dynamics_Sys& self, std::shared_ptr<Joint> joint) { self.Add(joint); })
        // Alias methods for easier use
        .def("Add", [](Dynamics_Sys& self, std::shared_ptr<Body> body) { self.Add(body); })
        .def("Add", [](Dynamics_Sys& self, std::shared_ptr<Joint> joint) { self.Add(joint); });

#ifdef ENABLE_VISUALIZATION
    // Bind Visualization class
    py::class_<Visualization>(m, "Visualization")
        .def(py::init<int, int>())
        .def("initialize", &Visualization::initialize)
        .def("cleanup", &Visualization::cleanup)
        .def("shouldClose", &Visualization::shouldClose)
        .def("clear", &Visualization::clear)
        .def("swapBuffers", &Visualization::swapBuffers)
        .def("processInput", &Visualization::processInput)
        .def("drawChain",
             [](Visualization& self, const std::vector<std::vector<double>>& positions) {
                 std::vector<arma::vec> arma_positions;
                 for (const auto& pos : positions) {
                     arma_positions.push_back(vector_to_arma_vec(pos));
                 }
                 self.drawChain(arma_positions);
             })
        .def("drawBody",
             [](Visualization& self, const std::vector<double>& position,
                const std::vector<double>& rotation, double mass) {
                 self.drawBody(vector_to_arma_vec(position), vector_to_arma_vec(rotation), mass);
             })
        .def("setCamera", &Visualization::setCamera);
#endif

    // Simplified factory functions for creating objects
    m.def(
        "create_ground",
        [](unsigned int num) -> std::shared_ptr<Body> { return std::make_shared<Ground>(num); },
        "Create a Ground body");

    m.def(
        "create_mobilized_body",
        [](unsigned int num, const std::vector<double>& pos, const std::vector<double>& vel,
           const std::vector<double>& acc, const std::vector<double>& ang,
           const std::vector<double>& ang_vel, const std::vector<double>& ang_acc, double mass,
           const std::vector<double>& inertia, const std::vector<double>& force,
           const std::vector<double>& torque) -> std::shared_ptr<Body> {
            return std::make_shared<Mobilized_body>(
                num, vector_to_arma_vec(pos), vector_to_arma_vec(vel), vector_to_arma_vec(acc),
                vector_to_arma_vec(ang), vector_to_arma_vec(ang_vel), vector_to_arma_vec(ang_acc),
                mass, vector_to_arma_vec(inertia), vector_to_arma_vec(force),
                vector_to_arma_vec(torque));
        },
        "Create a Mobilized body");

    m.def(
        "create_joint",
        [](int type, const std::vector<double>& pi, const std::vector<double>& pj,
           const std::vector<double>& qi, const std::vector<double>& qj,
           std::shared_ptr<Body> body_i, std::shared_ptr<Body> body_j) -> std::shared_ptr<Joint> {
            return std::make_shared<Joint>(type, vector_to_arma_vec(pi), vector_to_arma_vec(pj),
                                           vector_to_arma_vec(qi), vector_to_arma_vec(qj), body_i,
                                           body_j);
        },
        "Create a Joint");
}