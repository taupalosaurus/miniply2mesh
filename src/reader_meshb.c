#include <libmeshb7.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char* argv[])
{
	int InpMsh, OutMsh;
	int i, nbv, nbt, ver, dim, flag, *tt;
	double *ct;

	if (argc < 2) {
		printf("ERROR  must pass filename as argument.\n");
		exit(1);
	}

	// Open and check the inputs mesh
	InpMsh = GmfOpenMesh(argv[1], GmfRead, &ver, &dim);
	if( !InpMsh || (ver != 1) || (dim != 3) ) {
		printf("ERROR:  InpMsh: %d filename: %s  ver: %d,  dim: %d\n", InpMsh, argv[1], ver, dim);
		exit(12);
	}

	// Allocate vertices and quads tables
	nbv = GmfStatKwd(InpMsh, GmfVertices);
	ct = malloc(nbv * 3 * sizeof(float));
	nbt = GmfStatKwd(InpMsh, GmfTriangles);
	tt = malloc(nbt * 3 * sizeof(int));

	// Read vertices and quads then close the input file
	GmfGotoKwd(InpMsh, GmfVertices);
	for(i=0;i<nbv;i++)
		GmfGetLin(InpMsh, GmfVertices, &ct[3*i], &ct[3*i+1], &ct[3*i+2], &flag);
	GmfGotoKwd(InpMsh, GmfTriangles);
	for(i=0;i<nbt;i++)
		GmfGetLin(InpMsh, GmfTriangles, &tt[3*i], &tt[3*i+1], &tt[3*i+2]);

	GmfCloseMesh(InpMsh);
}
