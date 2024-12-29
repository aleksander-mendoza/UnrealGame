#pragma once

#include "CoreMinimal.h"
#include "../blender/proc_assets.h"
#include "../blender/utildefines.h"

struct ChunkDist {
	/**distance under l2 norm*/
	float distL2;
	int distL1;
	int chunkIdx;
	int2 chunkPos;
};
struct MeshGenResult {
	proc_assets::Mesh mesh;
	int sectionIdx=-1;
};
struct FoliageChunk {
	TArray<int> instanceIndices;
};

struct FoliageChunks {

	TArray<FoliageChunk> sections;

	TArray<short2> inverseLookup;
};

struct SectionStatus {
	bool wentOutOfBounds = false;
	bool newlyAdded = false;
	bool hasContent = false;
};