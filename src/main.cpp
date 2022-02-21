#include <cstdlib>
#include <cstdio>
#include <cmath>

#include <iostream>
#include <exception>

extern "C" {
#include <libmeshb7.h>
}

#include "read_ply.hpp"




void export_gmf_mesh3d(const std::string & filepath, std::vector<Vec3f> & vertices, std::vector<Face> & faces)
{
    int             dim, tag, NFacets, NNodes;
    const Vec3f    *coords;
    const Face     *fac;
    char            fileName[128];
    long long       meshIndex;
    int             gmfVersion;

    NNodes = vertices.size();
    NFacets = faces.size();
    //printf("DEBUG  NNodes: %d   NFacets: %d\n", NNodes, NFacets);
    dim = 3;
    gmfVersion = 3;
    
    strcpy(fileName, filepath.c_str());
    strcat(fileName, ".meshb");

    if ( !(meshIndex = GmfOpenMesh(fileName, GmfWrite, gmfVersion, dim)) ) {
        fprintf(stderr,"####  ERROR  mesh file %s cannot be opened\n", fileName);
        exit(1);
    }
    printf("  %%%% %s opened\n",fileName);

    GmfSetKwd(meshIndex, GmfVertices, NNodes);
    for (int i=0; i<NNodes; i++) {
        coords = &vertices[i];
        tag = 0;
        GmfSetLin(meshIndex, GmfVertices, coords->x, coords->y, coords->z, tag);
        if (i == NNodes-1) {
            //printf("DEBUG  trying to write vertex: %1.4e %1.4e %1.4e %d to file.\n", coords->x, coords->y, coords->z, tag);
        }
    }

    GmfSetKwd(meshIndex, GmfTriangles, NFacets);
    for (int i=0; i<NFacets; i++) {
        fac = &faces[i];
        tag = 0;
        GmfSetLin(meshIndex, GmfTriangles, fac->i_v1+1, fac->i_v2+1, fac->i_v3+1, tag); // TODO CHECK i_v+1 ?
    }

    GmfCloseMesh(meshIndex);
}



void export_gmf_sol(const std::string & filepath, std::vector<Vec3f> & normals, std::vector<Vec3f> & uvs, 
                       std::vector<Vec3d> & color_per_vertex)
{
    int                dim, NFacets, NNodes;
    const Vec3f       *norm, *uv;
    const Vec3d       *cpv;
    const Face        *fac;
    char               fileName[128];
    long long          meshIndex;
    int                gmfVersion;
    int                solTypeTable[4] = {2, 2, 2, 2};
    double             buffer[12] = {0.};
    int                rgb[3];
    double             xyz[3], nrm;
    std::vector<float> chromaticity;

    NNodes = color_per_vertex.size();
    dim = 3;
    gmfVersion = 3;

    chromaticity.resize(NNodes*2);
    for (int i=0; i<NNodes; ++i) {
        rgb[0] = color_per_vertex[i].x;
        rgb[1] = color_per_vertex[i].y;
        rgb[2] = color_per_vertex[i].z;
        xyz[0] = 0.4887180*rgb[0] + 0.3106803*rgb[1] + 0.2006017*rgb[2];
        xyz[1] = 0.1762044*rgb[0] + 0.8129847*rgb[1] + 0.0108109*rgb[2];
        xyz[2] = 0.0000000*rgb[0] + 0.0102048*rgb[1] + 0.9897952*rgb[2];
        nrm = xyz[0]+xyz[1]+xyz[2];
        if (fabs(nrm) < 1.e-12) {
            chromaticity[2*i]   = 0;
            chromaticity[2*i+1] = 0;    
        }
        else {
            chromaticity[2*i]   = xyz[0]/(xyz[0]+xyz[1]+xyz[2]);
            chromaticity[2*i+1] = xyz[1]/(xyz[0]+xyz[1]+xyz[2]);
        }
    }
    
    strcpy(fileName, filepath.c_str());
    strcat(fileName, ".solb");

    if ( !(meshIndex = GmfOpenMesh(fileName, GmfWrite, gmfVersion, dim)) ) {
        fprintf(stderr,"####  ERROR  mesh file %s cannot be opened\n", fileName);
        exit(1);
    }
    printf("  %%%% %s opened\n",fileName);

    GmfSetKwd(meshIndex, GmfSolAtVertices, NNodes, 4, solTypeTable);
    for (int i=0; i<NNodes; i++) {
        //norm = &normals[i];
        //uv = &uvs[i];
        cpv = &color_per_vertex[i];
        //buffer[0] = norm->x; buffer[1] = norm->y; buffer[2] = norm->z;
        //buffer[3] = uv->x;   buffer[4] = uv->y;   buffer[5] = uv->z;
        buffer[6] = (double)cpv->x;  buffer[7] = (double)cpv->y;  buffer[8] = (double)cpv->z;
        buffer[9] = chromaticity[2*i];  buffer[10] = chromaticity[2*i+1];  buffer[11] = 0;
        if (true){//(isnan(buffer[9])) {
            printf("DEBUG  i: %d / %d; buffer: %1.1e %1.1e %1.1e %1.1e %1.1e %1.1e\n",
            i, NNodes, buffer[6], buffer[7], buffer[8], buffer[9], buffer[10], buffer[11]);
        }
        GmfSetLin(meshIndex, GmfSolAtVertices, buffer);
    }


    GmfCloseMesh(meshIndex);
}



int main(int argc, char**argv)
{
    if (argc != 2) {
        std::cout << "[ERROR] while parsing command line." << std::endl
                  << "        USAGE is: miny-plyloader your_mesh.ply" << std::endl;
    }

    std::string const filepath(argv[1]);

    std::vector<Vec3f>  vertices;
    std::vector<Vec3f>  normals;
    std::vector<Vec3f>  uvs;
    std::vector<Face>   faces;
    std::vector<Vec3d>  color_per_vertex;

    try {
        read_ply(filepath, vertices, normals, uvs, faces, color_per_vertex);

        std::cout << "\t Read " << vertices.size() << " vertices " << std::endl;
        std::cout << "\t Read " << normals.size() <<  " normals per vertex " << std::endl;
        std::cout << "\t Read " << uvs.size()     << " uvs per vertex " << std::endl;
        std::cout << "\t Read " << faces.size() << " faces " << std::endl;
        std::cout << "\t Read " << color_per_vertex.size() << "  color per vertex " << std::endl;
    }
    catch(const std::exception& e) {
        std::cerr << e.what() << '\n';
    }

    export_gmf_mesh3d(filepath, vertices, faces);
    export_gmf_sol(filepath, normals, uvs, color_per_vertex);

    return EXIT_SUCCESS;
}
