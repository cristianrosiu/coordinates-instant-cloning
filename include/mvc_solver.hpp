#ifndef MVCC_H_
#define MVCC_H_

#include "adaptive_mesh.hpp"
#include "geometry.hpp"

class MVCSolver
{
    AdaptiveMesh m_mesh;
    public:
        MVCSolver() = default;

        std::vector<double> mvc(Point_2 const &p, const std::vector<Point_2> &ps);
        std::unordered_map<Point_2, std::vector<double>> preprocessing(cv::Mat const &mask, std::vector<Point_2> const &boundary);
        cv::Mat solve(cv::Mat const &src, cv::Mat const &dest, cv::Mat const &mask, glm::vec2 const &offset);
};
#endif