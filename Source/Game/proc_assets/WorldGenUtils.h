#pragma once

struct ChunkDist {
	int dist;
	int2 chunkPos;
};
struct MeshGenResult {
	proc_assets::Mesh mesh;
	int sectionIdx=-1;
};
struct MeshGenRequest {
	int2 chunkAbsPos;
	int resX=-1, resY=-1;
	int sectionIdx=-1;
};