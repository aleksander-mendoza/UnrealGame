#pragma once

struct ChunkDist {
	int dist;
	int2 chunkPos;
};
struct MeshGenResult {
	proc_assets::Mesh mesh;
	int sectionIdx;
	bool createNew;
};
struct MeshGenRequest {
	int2 chunkPos;
	int resX, resY;
	int sectionIdx;
	bool createNew;
};