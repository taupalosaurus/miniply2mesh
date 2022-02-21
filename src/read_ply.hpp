#pragma once

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#define TINYPLY_IMPLEMENTATION
#include "tinyply.h"

/**
 * @brief Minimalist functions and procedures to read a ply file
 * @author Romain Pacanowski @ INRIA DOT FR
 */

struct Vec2f
{
  float x;
  float y;
};

struct Vec3f
{
  float x; 
  float y;
  float z;
};

struct Vec3d
{
  int x; 
  int y;
  int z;
};

struct Face
{
  unsigned int i_v1; // index of first vertex
  unsigned int i_v2; // index of second vertex
  unsigned int i_v3; // index of third vertex
};


/**
 * @brief Read a ply file from a given path
 * 
 * Fills the different vector if the .ply file contains 
 * the relevant information.
 * 
 * Throw Runtime exception if the file cannot be open
 * 
 * @param filepath 
 * @param vertices 
 * @param normals 
 * @param uvs 
 * @param faces 
 * @param color_per_vertex 
 */
inline void read_ply(std::string        const & filepath,
                     std::vector<Vec3f>  & out_vertices,
                     std::vector<Vec3f>  & out_normals,
                     std::vector<Vec3f>  & out_uvs,
                     std::vector<Face>   & out_faces,
                     std::vector<Vec3d>  & out_color_per_vertex)
{

  using namespace tinyply;
  std::ifstream ss(filepath, std::ios::binary);
  if (ss.fail())
    throw std::runtime_error("failed to open " + filepath);

  PlyFile file;
  file.parse_header(ss);

  std::cout << "........................................................................\n";
  std::cout << " HEADER of PLYFILE: " << std::endl;
  std::cout << "........................................................................\n";
  for (auto c : file.get_comments())
    std::cout << "Comment: " << c << std::endl;
  for (auto e : file.get_elements())
  {
    std::cout << "element - " << e.name << " (" << e.size << ")" << std::endl;
    for (auto p : e.properties)
      std::cout << "\tproperty - " << p.name << " (" << tinyply::PropertyTable[p.propertyType].str << ")" << std::endl;
  }
  std::cout << "........................................................................" << std::endl;

  // Tinyply treats parsed data as untyped byte buffers. See below for examples.
  std::shared_ptr<PlyData> vertices, faces, normals, color_per_vertex, uvs;

  try
  {
    vertices = file.request_properties_from_element("vertex", {"x", "y", "z"});
  }
  catch ( std::exception &e)
  {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  // Providing a list size hint (the last argument) is a 2x performance improvement. If you have
  // arbitrary ply files, it is best to leave this 0.
  try
  {
    faces = file.request_properties_from_element("face", {"vertex_indices"}, 3);
  }
  catch (const std::exception &e)
  {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  try
  {
    normals = file.request_properties_from_element("vertex", {"nx", "ny", "nz"});
  }
  catch (const std::exception &e)
  {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  try
  {
    color_per_vertex = file.request_properties_from_element("vertex", {"red", "green", "blue"});
  }
  catch (const std::exception &e)
  {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  try
  {
    uvs = file.request_properties_from_element("vertex", {"u", "v"});
  }
  catch (const std::exception &e)
  {
    std::cerr << "tinyply exception: " << e.what() << std::endl;
  }

  
  file.read(ss);

  // if (vertices)
  //   std::cout << "\tRead " << vertices->count << " total vertices" << std::endl;
  // if (faces)
  //   std::cout << "\tRead " << faces->count << " total faces (triangles)" << std::endl;
  // if (normals)
  //   std::cout << "\tRead " << normals->count << " total normals" << std::endl;
  // if (color_per_vertex)
  //   std::cout << "\tRead " << color_per_vertex->count << " total colors" << std::endl;
  // if (uvs)
  //   std::cout << "\tRead " << uvs->count << " total UVs" << std::endl;
  

  // Now copy data to passed arguments
  if (vertices)
  {
    const size_t numVerticesBytes = vertices->buffer.size_bytes();
    out_vertices.resize(vertices->count);
    std::memcpy(out_vertices.data(), vertices->buffer.get(), numVerticesBytes);
  }
  if (faces)
  {
    const size_t numIndicesBytes = faces->buffer.size_bytes();
    out_faces.resize(faces->count);
    std::memcpy(out_faces.data(), faces->buffer.get(), numIndicesBytes);
  }
  if (normals)
  {
    const size_t numNormalsBytes = normals->buffer.size_bytes();
    out_normals.resize(normals->count);
    std::memcpy(out_normals.data(), normals->buffer.get(), numNormalsBytes);
  }
  if (color_per_vertex)
  {
    const size_t numColorsBytes = color_per_vertex->buffer.size_bytes();
    out_color_per_vertex.resize(color_per_vertex->count);
    std::memcpy(out_color_per_vertex.data(), color_per_vertex->buffer.get(), numColorsBytes);
  }
  if (uvs)
  {
    const size_t numUvsBytes = uvs->buffer.size_bytes();
    out_uvs.resize(uvs->count);
    std::memcpy(out_uvs.data(), uvs->buffer.get(), numUvsBytes);
  }

}