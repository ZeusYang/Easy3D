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


#include <easy3d/algo/surface_mesh_sampler.h>
#include <easy3d/core/surface_mesh.h>
#include <easy3d/core/point_cloud.h>


namespace easy3d {


    PointCloud* SurfaceMeshSampler::apply(const SurfaceMesh* mesh, int num /* = 1000000 */)
    {
        PointCloud* cloud = new PointCloud;
        cloud->set_name(mesh->name() + "_sampled");
        PointCloud::VertexProperty<vec3> normals = cloud->add_vertex_property<vec3>("v:normal");

        std::cout << "sampling surface..." << std::endl;

        // add all mesh vertices (even the requestred number is smaller than the
        // number of vertices in the mesh.

        auto mesh_points = mesh->get_vertex_property<vec3>("v:point");
        auto mesh_vertex_normals = mesh->get_vertex_property<vec3>("v:normal");
        for (auto p : mesh->vertices()) {
            PointCloud::Vertex v = cloud->add_vertex(mesh_points[p]);
            if (mesh_vertex_normals)
                normals[v] = mesh_vertex_normals[p];
            else
                normals[v] = mesh->compute_vertex_normal(p);
        }

        // now we may still need some points
        int num_needed = num - cloud->n_vertices();
        if (num_needed <= 0)
            return cloud;   // we got enougth points already

        // collect triangles and compute their areas
        struct Triangle : std::vector<SurfaceMesh::Vertex> {};

        std::vector<Triangle> triangles;
        std::vector<float>    triangle_areas;
        std::vector<vec3>	  triangle_normals;

        auto mesh_face_normals = mesh->get_face_property<vec3>("f:normal");
        float surface_area = 0.0;
        for (auto f : mesh->faces()) {
            const vec3& n = mesh_face_normals ? mesh_face_normals[f] : mesh->compute_face_normal(f);

            SurfaceMesh::Halfedge start = mesh->halfedge(f);
            SurfaceMesh::Halfedge cur = mesh->next_halfedge(mesh->next_halfedge(start));
            SurfaceMesh::Vertex va = mesh->to_vertex(start);
            while (cur != start) {
                SurfaceMesh::Vertex vb = mesh->from_vertex(cur);
                SurfaceMesh::Vertex vc = mesh->to_vertex(cur);

                Triangle tri;
                tri.push_back(va);
                tri.push_back(vb);
                tri.push_back(vc);
                triangles.push_back(tri);
                float area = geom::triangle_area(mesh_points[va], mesh_points[vb], mesh_points[vc]);
                triangle_areas.push_back(area);
                triangle_normals.push_back(n);
                surface_area += area;

                cur = mesh->next_halfedge(cur);
            }
        }

        float density = num_needed / surface_area;
        float samples_error = 0.0;
        std::size_t triangle_num = triangles.size();
        std::size_t num_generated = 0;
        std::size_t triangles_done = 0;
//        ProgressLogger progress(triangle_num, title());
        for (std::size_t idx = 0; idx < triangle_num; ++idx) {
            const Triangle& tri = triangles[idx];
            const vec3& n = triangle_normals[idx];

            // samples number considering the facet size (i.e., area)
            float samples_num = triangle_areas[idx] * density;
            std::size_t quant_samples_num = (std::size_t)samples_num;

            // adjust w.r.t. accumulated error
            samples_error += samples_num - quant_samples_num;
            if (samples_error > 1.0) {
                samples_error -= 1.0;
                quant_samples_num++;
            }

            if (triangles_done == triangle_num - 1)		// override number to gather all remaining points if last facet
                quant_samples_num = num_needed - num_generated;

            // generate points
            for (unsigned int j = 0; j < quant_samples_num; j++) {
                // compute barycentric coords
                double s = sqrt((double)rand() / (double)RAND_MAX);
                double t = (double)rand() / (double)RAND_MAX;
                double c[3];

                c[0] = 1.0 - s;
                c[1] = s * (1.0 - t);
                c[2] = s * t;

                vec3 p;
                for (std::size_t i = 0; i < 3; i++)
                    p = p + c[i] * mesh_points[tri[i]];

                PointCloud::Vertex v = cloud->add_vertex(p);
                normals[v] = n;
            }

            num_generated += quant_samples_num;
            triangles_done++;
//            progress.next();
        }

        std::cout << "done. resulted point cloud has " << cloud->n_vertices() << " points" << std::endl;
        return cloud;
    }

}
