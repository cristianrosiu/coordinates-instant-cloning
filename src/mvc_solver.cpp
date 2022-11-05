#include "mvc_solver.hpp"

/// <summary>
/// Function used to generate the mean-value coordinates between a fixed point p and all points on the mesh boundary.
/// </summary>
/// <param name="p">Fixed vertex inside the mesh.</param>
/// <param name="ps">List of boundary vertices.</param>
/// <returns>The list of mean-value coordinates (lambdas in the paper).</returns>
std::vector<double> MVCSolver::mvc(Point_2 const &p, const std::vector<Point_2> &ps)
{
	std::vector<double> w;
	auto x = glm::vec2(p.x(), p.y());
	
	// Case in which fixed vertex is very close to the boundary.
	int loc = -1;
	#pragma omp parallel for
	for (int i = 0; i < ps.size(); ++i)
	{
		// If distance between fixed point and current point on boundary is small break and compute lambda;
		auto p = glm::vec2(ps[i].x(), ps[i].y());
		if (glm::distance(p, x) < 1e-4)
			loc = i;
	}
	if(loc >= 0)
	{
		for (int i = 0; i < ps.size(); ++i)
			w.push_back(0.0);

		w[loc] = 1; 
		
		return w;
	}

	// Case in which the fixed vertex is relatively far from the boundary.
	auto total = 0.0;
	// #pragma omp parallel for
	for (int i = 0; i < ps.size(); i++)
	{
		// Retrieve the previous (i-1), current (i) and next (i) vertices on boundary.
		auto vi = glm::vec2(ps[i].x(), ps[i].y());
		auto vi_left = glm::vec2(ps[(i-1 + ps.size())%ps.size()].x(), ps[(i-1 + ps.size())%ps.size()].y());
		auto vi_right = glm::vec2(ps[(i+1)%ps.size()].x(), ps[(i+1)%ps.size()].y());
		
		// Calculate the vector going from current (i) checked point on boundary and fixed vertex x
		auto viP = glm::normalize(vi - x);

		// Calculate angles
		auto angle1 = getAngle(vi_left - x, viP);
		auto angle2 = getAngle(viP, vi_right - x);
		auto t1 = tan(angle2*0.5);
		auto t2 = tan(angle2*0.5);

		// Populate weights list
		w.push_back((t1+t2)/glm::distance(vi, x));
		// Calculate total
		total += w[i];

	}

	// Normalize lambdas
    float factor = 1.0/total;
	// #pragma omp parallel for
	for (int i = 0; i < ps.size(); ++i)
		w[i] *= factor;

	return w;
}

/// <summary>
/// Preprocessing stage of the algorithm. Pre-computes an adaptive mesh and the mean-value coordinates of each vertex in the mesh
/// </summary>
/// <param name="mask">ROI image.</param>
/// <param name="boundary">List of boundary vertices.</param>
/// <returns>A dictionary containing points as keys and their respective mean-value coordinates as value.</returns>
std::unordered_map<Point_2, std::vector<double>> MVCSolver::preprocessing(cv::Mat const &mask, std::vector<Point_2> const &boundary)
{
    // Compute mesh
	m_mesh.createMesh(boundary);
	auto vs = m_mesh.vertices();

	// Compute MVC coordinates for each vertex
	std::unordered_map<Point_2, std::vector<double>> coordinates;

	// #pragma omp parallel for
	// Populate the map
	for(auto const &p : vs)
	{
		auto key = std::to_string(p.x()) + std::to_string(p.y());
		// #pragma omp critical
		coordinates.insert({p, mvc(p, boundary)});
	}

	return coordinates;
}

/// <summary>
/// Main solver function. It first preprocess the mesh and mean-value coordinates
/// Pre-computes the difference in intensities between boundary pixels of source and target patches
//  Pre-computes the weighted sum using the mean value-coordinates as weights and the difference in intensity as value. 
//  Lastly, iterates over all the pixels inside the masked patch, retrieves the triangle of the mesh that contains it, interpolates its value and computes final result
/// </summary>
/// <param name="src">Source image.</param>
/// <param name="dest">Target image.</param>
/// <param name="mask">Masked region of the source that needs to be cloned over target.</param>
/// <param name="offset">Position offset of the mask inside the target image space.</param>
/// <returns>Final blended image.</returns>
cv::Mat MVCSolver::solve(cv::Mat const &src, cv::Mat const &dest, cv::Mat const &mask, glm::vec2 const &offset)
{
	auto result = dest.clone();

	// Build mesh and compute the mean-value coordinates
	auto boundary = getBoundary(mask);

	std::chrono::steady_clock::time_point time_start, time_end;
	time_start = std::chrono::steady_clock::now();
	
	auto MVC = preprocessing(mask, boundary);
	// Compute and store the difference in intensity between boundary pixels of source and target patches.
	std::vector<cv::Vec3d> intensityDiff;
	// #pragma omp parallel for	
	for(auto const &p : boundary)
	{
		cv::Vec3d a {dest.at<cv::Vec3b>(p.y() + offset.y, p.x() + offset.x)};
		cv::Vec3d b {src.at<cv::Vec3b>(p.y() , p.x())};
		// #pragma omp critical
		intensityDiff.push_back(a - b);
	}

	// Pre-compute the weighted sum of intensities and mean-value coordinates.
	std::unordered_map<Point_2, cv::Vec3d> r;
	auto vs = m_mesh.vertices();
	for(auto const &p : vs)
	{	
		auto lambda = MVC.at(p);
		cv::Vec3d c = cv::Vec3d(0.0, 0.0, 0.0);
		for (int i = 0; i < intensityDiff.size(); i++)
			c += intensityDiff[i]*lambda[i];
		
		// #pragma omp critical
		r.insert({p, c});
	}
	// For each pixel inside the target patch compute the right intensity.
	// #pragma omp parallel for collapse(2)
	for (int y = 0; y < dest.rows; ++y)
	{
		for(int x = 0; x < dest.cols; ++x)
		{
			Point_2 p{x, y};
			// Is point outside the ROI/Mask? Then skip
			if (boundCheck(p, boundary) != CGAL::ON_BOUNDED_SIDE) continue;
			// Retrieve triangle in which point p lies.
			auto t = m_mesh.getFace(p);
			auto barycentric = getBarycenterCoordinates(t, p);
			// Calculate the value of the pixel by interpolating inside triangle using barycentric coordinates.
			cv::Vec3d c = barycentric[0]*r.at(t[0]) + barycentric[1]*r.at(t[1]) + barycentric[2]*r.at(t[2]);
			cv::Vec3d srcI {src.at<cv::Vec3b>(y, x)};
			cv::Vec3d resultI = srcI + c;

			// Compute final intensity value of pixel p inside target patch
			result.at<cv::Vec3b>(y+offset.y, x+offset.x) = {cv::saturate_cast<uchar>(resultI.val[0]), cv::saturate_cast<uchar>(resultI.val[1]), cv::saturate_cast<uchar>(resultI.val[2])};
		}
	}
	time_end = std::chrono::steady_clock::now();
	std::cout << std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count() / 1e3f<< "ms" << "\n";
	
	return result;
}