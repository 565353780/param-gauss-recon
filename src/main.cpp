#include "pgr_params.h"
#include "pgr_reconstructor.h"
#include <Eigen/Core>
#include <filesystem>
#include <iostream>
#include <open3d/Open3D.h>
#include <vector>

struct MeshData {
  std::vector<double> vertices;      // xyz
  std::vector<size_t> indices;       // triangles [v1, v2, v3, ...]
  std::vector<double> normals;       // xyz
  std::vector<size_t> cornerIndices; // 拐角点id
};

const bool savePcdFile(const std::vector<double> &vertices,
                       const std::string &file_path) {
  if (vertices.size() % 3 != 0) {
    std::cerr << "Error: vertices vector size should be a multiple of 3!"
              << std::endl;
    return false;
  }

  open3d::geometry::PointCloud point_cloud;

  for (size_t i = 0; i < vertices.size(); i += 3) {
    Eigen::Vector3d point(vertices[i], vertices[i + 1], vertices[i + 2]);
    point_cloud.points_.push_back(point);
  }

  return open3d::io::WritePointCloud(file_path, point_cloud, true);
}

MeshData readPcdData(const std::string &ply_file) {
  MeshData mesh_data;

  // 加载三角网格
  open3d::geometry::PointCloud pcd;
  if (!open3d::io::ReadPointCloud(ply_file, pcd)) {
    std::cerr << "Failed to read the mesh file." << std::endl;
    return mesh_data;
  }

  // 获取顶点数据
  const auto &vertices = pcd.points_;

  // 填充 MeshData 结构体
  mesh_data.vertices.reserve(vertices.size() * 3);
  for (const auto &vertex : vertices) {
    mesh_data.vertices.push_back(vertex[0]);
    mesh_data.vertices.push_back(vertex[1]);
    mesh_data.vertices.push_back(vertex[2]);
  }

  return mesh_data;
}

MeshData readMeshData(const std::string &ply_file) {
  MeshData mesh_data;

  // 加载三角网格
  open3d::geometry::TriangleMesh mesh;
  if (!open3d::io::ReadTriangleMesh(ply_file, mesh)) {
    std::cerr << "Failed to read the mesh file." << std::endl;
    return mesh_data;
  }

  // 估计法线
  mesh.ComputeVertexNormals();

  // 获取顶点数据
  const auto &vertices = mesh.vertices_;
  const auto &triangles = mesh.triangles_;
  const auto &normals = mesh.vertex_normals_;

  // 填充 MeshData 结构体
  mesh_data.vertices.reserve(vertices.size() * 3);
  for (const auto &vertex : vertices) {
    mesh_data.vertices.push_back(vertex[0]);
    mesh_data.vertices.push_back(vertex[1]);
    mesh_data.vertices.push_back(vertex[2]);
  }

  mesh_data.indices.reserve(triangles.size() * 3);
  for (const auto &triangle : triangles) {
    mesh_data.indices.push_back(triangle[0]);
    mesh_data.indices.push_back(triangle[1]);
    mesh_data.indices.push_back(triangle[2]);
  }

  mesh_data.normals.reserve(normals.size() * 3);
  for (const auto &normal : normals) {
    mesh_data.normals.push_back(normal[0]);
    mesh_data.normals.push_back(normal[1]);
    mesh_data.normals.push_back(normal[2]);
  }

  return mesh_data;
}

class DataClean {
public:
  DataClean() {};

  void CloudMesh(MeshData &ioMesh, const std::string &tmp_folder_path);
};

void DataClean::CloudMesh(MeshData &ioMesh,
                          const std::string &tmp_folder_path) {
  std::string valid_tmp_folder_path = tmp_folder_path;
  if (valid_tmp_folder_path.back() != '/') {
    valid_tmp_folder_path += '/';
  }

  const std::string save_pcd_file_path = valid_tmp_folder_path + "/pcd.ply";
  const std::string save_normalize_pcd_file_path =
      valid_tmp_folder_path + "/normalized_pcd.xyz";
  const std::string save_mesh_file_path =
      valid_tmp_folder_path + "/pgr_recon.ply";
  const bool overwrite = true;

  if (!std::filesystem::exists(valid_tmp_folder_path)) {
    std::filesystem::create_directories(valid_tmp_folder_path);
    std::cout << "Directory created: " << valid_tmp_folder_path << std::endl;
  }

  if (std::filesystem::exists(save_pcd_file_path)) {
    std::filesystem::remove(save_pcd_file_path); // 删除文件
  }
  if (std::filesystem::exists(save_normalize_pcd_file_path)) {
    std::filesystem::remove(save_normalize_pcd_file_path); // 删除文件
  }
  if (std::filesystem::exists(save_mesh_file_path)) {
    std::filesystem::remove(save_mesh_file_path); // 删除文件
  }

  savePcdFile(ioMesh.vertices, save_pcd_file_path);

  PGRParams pgr_params;

  PGRReconstructor().reconstructSurface(pgr_params, save_pcd_file_path,
                                        save_normalize_pcd_file_path,
                                        save_mesh_file_path, overwrite);
  return;
}

void testAPI(const std::string &pcd_file_path,
             const std::string &tmp_folder_path) {
  MeshData mesh_data = readPcdData(pcd_file_path);

  DataClean().CloudMesh(mesh_data, tmp_folder_path);

  std::cout << "mesh vertices size: " << mesh_data.vertices.size() / 3
            << std::endl;
  std::cout << "mesh indices size: " << mesh_data.indices.size() / 3
            << std::endl;
  std::cout << "mesh normals size: " << mesh_data.normals.size() / 3
            << std::endl;
  return;
}

int main() {
  PGRParams pgr_params;
  const std::string pcd_file_path = "/home/chli/chLi/Dataset/Famous/bunny.ply";
  const std::string save_normalize_pcd_file_path =
      "/home/chli/Downloads/bunny_normalized.xyz";
  const std::string save_mesh_file_path = "/home/chli/Downloads/bunny_pgr.ply";
  const bool overwrite = true;

  testAPI(pcd_file_path, "./output/");
  return 1;

  PGRReconstructor pgr_reconstructor;
  pgr_reconstructor.reconstructSurface(pgr_params, pcd_file_path,
                                       save_normalize_pcd_file_path,
                                       save_mesh_file_path, overwrite);

  return 1;
}
