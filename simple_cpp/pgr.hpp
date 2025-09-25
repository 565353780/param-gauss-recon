#pragma once

#include <algorithm>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct MeshData {
  std::vector<double> vertices;
  std::vector<size_t> indices;
  std::vector<double> normals;
  std::vector<size_t> cornerIndices;
};

void createDirectoryIfNeeded(const std::string &filePath) {
  std::filesystem::path path(filePath);
  std::filesystem::path dir = path.parent_path();
  if (!std::filesystem::exists(dir)) {
    std::filesystem::create_directories(dir);
    std::cout << "Created directory: " << dir << std::endl;
  }
}

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

void savePointCloudToXYZ(const std::vector<double> &pointCloud,
                         const std::string &filename, bool overwrite = false) {
  if (overwrite && !deleteFileIfExists(filename)) {
    std::cerr << "Failed to delete existing file, aborting." << std::endl;
    return;
  }

  createDirectoryIfNeeded(filename);

  std::ofstream outfile(filename);
  if (!outfile) {
    std::cerr << "Error opening file: " << filename << std::endl;
    return;
  }

  size_t N = pointCloud.size() / 3;
  if (pointCloud.size() % 3 != 0) {
    std::cerr << "Invalid point cloud size, it should be a multiple of 3."
              << std::endl;
    return;
  }

  for (size_t i = 0; i < N; ++i) {
    double x = pointCloud[i * 3];
    double y = pointCloud[i * 3 + 1];
    double z = pointCloud[i * 3 + 2];
    outfile << x << " " << y << " " << z << "\n";
  }

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

  while (std::getline(plyFile, line)) {
    std::istringstream lineStream(line);
    std::string keyword;
    lineStream >> keyword;

    if (keyword == "end_header") {
      headerEnd = true;
      break;
    }

    if (line.find("element vertex") != std::string::npos) {
      lineStream >> keyword >> numVertices;
    }
    if (line.find("element face") != std::string::npos) {
      lineStream >> keyword >> numFaces;
      hasFaces = true;
    }
    if (line.find("property normal") != std::string::npos) {
      hasNormals = true;
    }
  }

  if (!headerEnd) {
    std::cerr << "Error: PLY header doesn't end properly." << std::endl;
    return false;
  }

  mesh_data.vertices.clear();
  mesh_data.indices.clear();
  mesh_data.normals.clear();

  mesh_data.vertices.reserve(numVertices * 3);
  for (size_t i = 0; i < numVertices; ++i) {
    std::getline(plyFile, line);
    std::istringstream lineStream(line);
    double x, y, z;
    lineStream >> x >> y >> z;

    mesh_data.vertices.push_back(x);
    mesh_data.vertices.push_back(y);
    mesh_data.vertices.push_back(z);
  }

  if (hasNormals) {
    mesh_data.normals.reserve(numVertices * 3);
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
    mesh_data.normals.assign(numVertices * 3, 0.0);
  }

  if (hasFaces) {
    mesh_data.indices.reserve(numFaces * 3);
    for (size_t i = 0; i < numFaces; ++i) {
      std::getline(plyFile, line);
      std::istringstream lineStream(line);
      size_t vertexCountInFace;
      lineStream >> vertexCountInFace;

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

void CloudMeshPGR(MeshData &ioMesh, const std::string &pgr_folder_path,
                  const std::string &tmp_folder_path) {
  const int sample_point_num = 20000;
  const int wk = 7;
  const double wmax = 0.015;
  const double wmin = 0.0015;
  const double alpha = 1.05;
  const int d = 10;
  const int md = 1;
  const bool use_cpu = false;

  std::string valid_tmp_folder_path = tmp_folder_path;
  if (valid_tmp_folder_path.back() != '/') {
    valid_tmp_folder_path += '/';
  }

  const std::string save_pcd_file_path = valid_tmp_folder_path + "pcd.xyz";
  const std::string save_normalize_pcd_file_path =
      valid_tmp_folder_path + "normalized_pcd.xyz";
  const std::string save_mesh_file_path =
      valid_tmp_folder_path + "pgr_recon.ply";
  const bool overwrite = true;

  savePointCloudToXYZ(ioMesh.vertices, save_pcd_file_path, overwrite);

  std::string command = "python run_pgr.py";
  command += " " + save_pcd_file_path;
  command += " " + valid_tmp_folder_path;
  command += " " + save_mesh_file_path;
  command += " -s " + std::to_string(sample_point_num);
  command += " -wk " + std::to_string(wk);
  command += " -wmax " + std::to_string(wmax);
  command += " -wmin " + std::to_string(wmin);
  command += " -a " + std::to_string(alpha);
  command += " -d " + std::to_string(d);
  command += " -md " + std::to_string(md);

  if (use_cpu) {
    command += " --cpu";
  }

  const int result_state = system(command.c_str());

  if (result_state != 0) {
    std::cerr << "Error executing command: " << command << std::endl;
    return;
  }

  loadPlyFile(save_mesh_file_path, ioMesh);

  return;
}
