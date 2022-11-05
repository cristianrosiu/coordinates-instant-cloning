#ifndef DAPTIVEMESH_H_
#define DAPTIVEMESH_H_

#include "helpers.hpp"

static const std::filesystem::path dataDirPath { DATA_DIR };
static const std::filesystem::path outDirPath { OUTPUT_DIR };

class AdaptiveMesh
{
    CDT m_cdt;
    std::vector<Point_2> m_vs;
    public:
        AdaptiveMesh() = default;
        
        void createMesh(std::vector<Point_2> const &v);
        std::vector<Point_2> getFace(Point_2 const &v);
        std::vector<Point_2> vertices();
        void save(cv::Mat const &img, int const &i);
    
};

#endif