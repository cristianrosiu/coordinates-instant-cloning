#include "adaptive_mesh.hpp"


/// <summary>
/// Create an adaptive mesh using the boundary points.
/// </summary>
/// <param name="v">The list of points on the boundary of source patch</param>
/// <returns>An adaptive mesh stored inside the m_cdt member</returns>
void AdaptiveMesh::createMesh(std::vector<Point_2> const &v)
{   
    // Clear existing mesh
    m_cdt.clear();

    // Add points to mesh and define constraints
    std::vector<Vertex_handle> vh;
    for(auto const &p : v)
    	vh.push_back(m_cdt.insert(CDTPoint(p.x(), p.y())));
	 for(int i = 0; i < v.size(); i ++)
        m_cdt.insert_constraint(vh[i],vh[(i == v.size() - 1) ? 0 : (i + 1)]);

    // Generate mesh
	CGAL::refine_Delaunay_mesh_2(m_cdt, Criteria(0.125,16));

    // Add vertices to list
    for(CDT::Finite_vertices_iterator vit = m_cdt.finite_vertices_begin(); vit != m_cdt.finite_vertices_end(); vit ++){
        CDTPoint p = vit -> point();
        m_vs.push_back(Point_2{p.x(),p.y()});
    }
}

/// <summary>
/// Given a 2D point inside the patch, it finds the triangle in which said point lies.
/// </summary>
/// <param name="v">2D point within source patch.</param>
/// <returns>The triangle in which said point lies.</returns>
std::vector<Point_2> AdaptiveMesh::getFace(Point_2 const &v)
{   
    std::vector<Point_2> face;
	auto f = m_cdt.locate(CDTPoint{v.x(), v.y()});
    // #pragma omp parallel for
	for(int i = 0; i < 3; i ++){
        CDTPoint p = f->vertex(i)->point();
        // #pragma omp critical
        face.push_back(Point_2{p.x(), p.y()});
    }
	return face;
}

/// <summary>
/// Retrieves all the vertices of the adaptive mesh.
/// </summary>
/// <returns>A vector containing all vertices of the adaptive mesh.</returns>
std::vector<Point_2> AdaptiveMesh::vertices()
{
    return std::vector<Point_2>{m_vs};
}

/// <summary>
/// Draws the generated mesh on top of the source image and saves it in the output folder
/// <param name="img">Image on which mesh is overlayed</param>
/// <param name="i">The index of the mesh image filename</param>
/// </summary>
void AdaptiveMesh::save(cv::Mat const &img, int const &i)
{
    auto clone = img.clone();
    for(CDT::Finite_edges_iterator eit = m_cdt.finite_edges_begin(); eit != m_cdt.finite_edges_end(); eit ++){
        CDT::Face& f = *(eit -> first);
        int i = eit -> second;
        CDTPoint p1 = f.vertex(CDT::cw(i)) -> point();
        CDTPoint p2 = f.vertex(CDT::ccw(i)) -> point();
		cv::line(clone, cv::Point(p1.x(), p1.y()), cv::Point(p2.x(), p2.y()), cv::Scalar(255, 0, 0), 1);
    }
    cv::imwrite(outDirPath.string() + "/meshes/mesh_0" + std::to_string(i+1) + ".png", clone);
}