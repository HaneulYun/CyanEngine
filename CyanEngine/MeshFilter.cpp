#include "pch.h"
#include "MeshFilter.h"

MeshFilter::MeshFilter()
{
}

MeshFilter::~MeshFilter()
{
	if (mesh)
		delete mesh;
}
