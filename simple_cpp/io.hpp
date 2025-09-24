#pragma once

#include <algorithm>
#include <cstdio>     // 用于删除文件
#include <filesystem> // C++17 文件系统库
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct MeshData {
  std::vector<double> vertices;      // xyz
  std::vector<size_t> indices;       // triangles [v1, v2, v3, ...]
  std::vector<double> normals;       // xyz
  std::vector<size_t> cornerIndices; // 拐角点id
};

// 创建文件夹（如果不存在）
void createDirectoryIfNeeded(const std::string &filePath) {
  std::filesystem::path path(filePath);
  std::filesystem::path dir = path.parent_path();
  if (!std::filesystem::exists(dir)) {
    std::filesystem::create_directories(dir);
    std::cout << "Created directory: " << dir << std::endl;
  }
}

// 删除文件
bool deleteFileIfExists(const std::string &filePath) {
  if (std::filesystem::exists(filePath)) {
    if (std::remove(filePath.c_str()) == 0) {
      return true;
    } else {
      std::cerr << "Error deleting file: " << filePath << std::endl;
      return false;
    }
  }
  return true;
}

// 保存点云到 .xyz 文件
void savePointCloudToXYZ(const std::vector<double> &pointCloud,
                         const std::string &filename, bool overwrite = false) {
  // 如果要覆盖文件，先删除已有文件
  if (overwrite && !deleteFileIfExists(filename)) {
    std::cerr << "Failed to delete existing file, aborting." << std::endl;
    return;
  }

  // 创建文件所在目录（如果不存在）
  createDirectoryIfNeeded(filename);

  // 打开文件进行写入
  std::ofstream outfile(filename);
  if (!outfile) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  // 确保点云数组的长度是 N*3
  size_t N = pointCloud.size() / 3;
  if (pointCloud.size() % 3 != 0) {
    std::cerr << "Invalid point cloud size, it should be a multiple of 3."
              << std::endl;
    return;
  }

  // 写入每个点的坐标到文件
  for (size_t i = 0; i < N; ++i) {
    double x = pointCloud[i * 3];
    double y = pointCloud[i * 3 + 1];
    double z = pointCloud[i * 3 + 2];
    outfile << x << " " << y << " " << z << "\n";
  }

  // 关闭文件
  outfile.close();
  std::cout << "Point cloud saved to: " << filename << std::endl;
}

bool loadPlyFile(const std::string &filename, MeshData &mesh_data) {
  std::ifstream plyFile(filename);
  if (!plyFile.is_open()) {
    std::cerr << "Error opening PLY file: " << filename << std::endl;
    return false;
  }

  std::string line;
  bool headerEnd = false;
  size_t numVertices = 0, numFaces = 0;
  size_t vertexCount = 0, faceCount = 0;
  bool hasNormals = false;
  bool hasFaces = false;

  // 读取头部信息
  while (std::getline(plyFile, line)) {
    std::istringstream lineStream(line);
    std::string keyword;
    lineStream >> keyword;

    // 查找头部结束
    if (keyword == "end_header") {
      headerEnd = true;
      break;
    }

    // 读取顶点数和面数
    if (line.find("element vertex") != std::string::npos) {
      lineStream >> keyword >> numVertices;
    }
    if (line.find("element face") != std::string::npos) {
      lineStream >> keyword >> numFaces;
      hasFaces = true; // 如果有面数据
    }
    if (line.find("property normal") != std::string::npos) {
      hasNormals = true; // 如果有法线数据
    }
  }

  if (!headerEnd) {
    std::cerr << "Error: PLY header doesn't end properly." << std::endl;
    return false;
  }

  mesh_data.vertices.clear();
  mesh_data.indices.clear();
  mesh_data.normals.clear();

  // 读取顶点数据
  mesh_data.vertices.reserve(numVertices * 3); // 每个顶点有3个坐标(x, y, z)
  for (size_t i = 0; i < numVertices; ++i) {
    std::getline(plyFile, line);
    std::istringstream lineStream(line);
    double x, y, z;
    lineStream >> x >> y >> z;

    mesh_data.vertices.push_back(x);
    mesh_data.vertices.push_back(y);
    mesh_data.vertices.push_back(z);
  }

  // 如果有法线数据，读取法线
  if (hasNormals) {
    mesh_data.normals.reserve(numVertices * 3); // 每个法线有3个坐标(x, y, z)
    for (size_t i = 0; i < numVertices; ++i) {
      std::getline(plyFile, line);
      std::istringstream lineStream(line);
      double nx, ny, nz;
      lineStream >> nx >> ny >> nz;
      mesh_data.normals.push_back(nx);
      mesh_data.normals.push_back(ny);
      mesh_data.normals.push_back(nz);
    }
  } else {
    // 如果没有法线数据，填充默认法线（零向量）
    mesh_data.normals.assign(numVertices * 3, 0.0);
  }

  // 读取面数据（如果有）
  if (hasFaces) {
    mesh_data.indices.reserve(numFaces * 3); // 每个面有3个索引
    for (size_t i = 0; i < numFaces; ++i) {
      std::getline(plyFile, line);
      std::istringstream lineStream(line);
      size_t vertexCountInFace;
      lineStream >> vertexCountInFace; // 第一个值是面上的顶点数

      if (vertexCountInFace != 3) {
        std::cerr << "Error: Only triangles are supported in this PLY file."
                  << std::endl;
        return false;
      }

      size_t v1, v2, v3;
      lineStream >> v1 >> v2 >> v3;
      mesh_data.indices.push_back(v1);
      mesh_data.indices.push_back(v2);
      mesh_data.indices.push_back(v3);
    }
  }

  plyFile.close();
  return true;
}
