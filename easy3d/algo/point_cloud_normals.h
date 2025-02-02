/*
*	Copyright (C) 2015 by Liangliang Nan (liangliang.nan@gmail.com)
*	https://3d.bk.tudelft.nl/liangliang/
*
*	This file is part of Easy3D. If it is useful in your research/work, 
*   I would be grateful if you show your appreciation by citing it:
*   ------------------------------------------------------------------
*           Liangliang Nan. 
*           Easy3D: a lightweight, easy-to-use, and efficient C++
*           library for processing and rendering 3D data. 2018.
*   ------------------------------------------------------------------
*
*	Easy3D is free software; you can redistribute it and/or modify
*	it under the terms of the GNU General Public License Version 3
*	as published by the Free Software Foundation.
*
*	Easy3D is distributed in the hope that it will be useful,
*	but WITHOUT ANY WARRANTY; without even the implied warranty of
*	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
*	GNU General Public License for more details.
*
*	You should have received a copy of the GNU General Public License
*	along with this program. If not, see <http://www.gnu.org/licenses/>.
*/


#ifndef EASY3D_ALGO_POINT_CLOUD_NORMALS_H
#define EASY3D_ALGO_POINT_CLOUD_NORMALS_H

#include <string>


namespace easy3d {

    class PointCloud;

    class PointCloudNormals
    {
    public:
        /// Estimates the point cloud normals using PCA.
        /// @param k: the number of neighboring points to construct the covariance matrix.
        /// @param compute_curvature: also computes the curvature?
        void estimate(PointCloud* cloud, unsigned int k = 16, bool compute_curvature = false);

        /// Reorients the point cloud normals.
        /// This method implements the normal reorientation method described in
        /// Hoppe et al. Surface reconstruction from unorganized points. SIGGRAPH 1992.
        /// @param k: the number of neighboring points to construct the graph.
        void reorient(PointCloud* cloud, unsigned int k = 16);
    };


} // namespace easy3d


#endif  // EASY3D_ALGO_POINT_CLOUD_NORMALS_H

