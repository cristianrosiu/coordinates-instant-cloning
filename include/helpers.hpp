#pragma once
/*
This file contains useful function and definitions.
Do not ever edit this file - it will not be uploaded for evaluation.
If you want to modify any of the functions here (e.g. extend triangle test to quads),
copy the function "your_code_here.h" and give it a new name.
*/

#include <framework/disable_all_warnings.h>
DISABLE_WARNINGS_PUSH()
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/geometric.hpp>
DISABLE_WARNINGS_POP()

#include <cassert>
#include <chrono>
#include <cmath>
#include <execution>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <string>
#include <cstdlib>
#include <filesystem>

#include <algorithm>
#include <array>
#include <numeric>
#include <span>
#include <tuple>
#include <vector>
#include <thread>

#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/convex_hull_2.h>
#include <CGAL/Polygon_2.h>
#include <CGAL/Polygon_2_algorithms.h>
#include <CGAL/Constrained_Delaunay_triangulation_2.h>
#include <CGAL/Delaunay_mesher_2.h>
#include <CGAL/Delaunay_mesh_face_base_2.h>
#include <CGAL/Delaunay_mesh_size_criteria_2.h>
#include <CGAL/Delaunay_triangulation_2.h>
#include <CGAL/Triangulation_conformer_2.h>
#include <CGAL/Delaunay_triangulation_adaptation_traits_2.h>
typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_2 Point_2;
typedef CGAL::Polygon_2<K> Polygon_2;

typedef CGAL::Triangulation_vertex_base_2<K> Vb;
typedef CGAL::Delaunay_mesh_face_base_2<K> Fb;
typedef CGAL::Triangulation_data_structure_2<Vb, Fb> Tds;
typedef CGAL::Delaunay_triangulation_2<K,Tds> DT2;
typedef CGAL::Delaunay_triangulation_adaptation_traits_2<DT2> AT;
typedef Tds::Face_handle Face_handle;
typedef Tds::Vertex_handle Vertex_handle;
typedef CGAL::Constrained_Delaunay_triangulation_2<K, Tds> CDT;
typedef CGAL::Delaunay_mesh_size_criteria_2<CDT> Criteria;
typedef CDT::Vertex_handle Vertex_handle;
typedef CDT::Point CDTPoint;

#ifdef _OPENMP
// Only if OpenMP is enabled.
#include <omp.h>
#endif

#include <framework/image.h>

/// <summary>
/// Aliases for Image classes.
/// </summary>
using ImageRGB = Image<glm::vec3>;

/// <summary>
/// Prints helpful information about OpenMP.
/// </summary>
// void printOpenMPStatus() 
// {
// #ifdef _OPENMP
//     // https://stackoverflow.com/questions/38281448/how-to-check-the-version-of-openmp-on-windows
//     std::cout << "OpenMP version " << _OPENMP << " is ENABLED with " << omp_get_max_threads() << " threads." << std::endl;
// #else
//     std::cout << "OpenMP is DISABLED." << std::endl;
// #endif
// }
