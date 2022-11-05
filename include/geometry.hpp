#ifndef GEOMETRY_H_
#define GEOMETRY_H_

#include "helpers.hpp"


/*
 * Utility functions.
 */

static inline CGAL::Bounded_side boundCheck(const Point_2& p, const std::vector<Point_2>& v){
    return CGAL::bounded_side_2(v.begin(), v.end(), p, K());
}

static inline double getAngle(glm::vec2 a, glm::vec2 b)
{
	auto dotProd = glm::dot(a, b);
	auto det = a.x*b.y - a.y*b.x;

	return atan2(det, dotProd);
}

static inline std::vector<double> getBarycenterCoordinates(std::vector<Point_2> const &t, Point_2 p)
{
	auto v1 = t[0];
	auto v2 = t[1];
	auto v3 = t[2];

	auto total = ((v2.y()-v3.y())*(v1.x()-v3.x())+(v3.x()-v2.x())*(v1.y()-v3.y()));
	auto W1 = ((v2.y() - v3.y())*(p.x() - v3.x()) + (v3.x()-v2.x())*(p.y()-v3.y()))/total;
	auto W2 = ((v3.y() - v1.y())*(p.x() - v3.x()) + (v1.x()-v3.x())*(p.y()-v3.y()))/total;
	auto W3 = 1 - W1 - W2;

	return std::vector<double>{W1, W2, W3};
}

static inline std::vector<Point_2> getBoundary(cv::Mat const &src)
{
	cv::Mat aux = src.clone();
	cv::cvtColor(src, aux, cv::COLOR_BGRA2GRAY);

	// Compute patch boundaries
	std::vector<std::vector<cv::Point> > contours;
	cv::Mat contourSrcPatch = aux.clone();
	cv::findContours(contourSrcPatch, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

	std::vector<Point_2> boundary;

	std::for_each(contours[0].begin(), contours[0].end(), [&boundary](auto p){boundary.push_back(Point_2{p.x, p.y});});

	std::reverse(boundary.begin(),boundary.end()); 

	return boundary;

}

#endif