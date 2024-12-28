#pragma once
#include "../blender/proc_assets.h"
#include "../blender/utildefines.h"

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
	bool chunkMissing = false;
};
struct FoliageGenResult {
	TArray<FTransform> instances;
	int sectionIdx = -1;
};
struct FoliageGenRequest {
	int2 chunkAbsPos;
	int sectionIdx = -1;
};

struct FoliageChunk {
	TArray<int> instanceIndices;

	/**This flag is used to prevent a chunk from being populated twice.
	We have this flag because checking whether instanceIndices is empty
	or not is not a good indicator. A chunk might be populated with zero instances
	(for example a an water-filled ocean chunk will have no trees in it.
	This flag will prevent the generator from trying and failing to spawn
	trees in such chunk at each frame). */
	bool isPopulated = false;
};

struct FoliageChunks {

	TArray<FoliageChunk> sections;

	TArray<short2> inverseLookup;
};