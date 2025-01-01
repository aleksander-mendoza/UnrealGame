#pragma once
#include "../blender/proc_assets.h"
#include "../blender/utildefines.h"

struct ChunkDist {
	/**distance under l2 norm*/
	float distL2;
	/**distance under l-infinity norm*/
	int distLinf;
	int chunkIdx;
	int2 chunkPos;
	
};
struct MeshGenRequest {
	int2 chunkAbsPos;
	int resX=-1, resY=-1;
	int sectionIdx=-1;
	int chunkDistance=9999;
	bool chunkMissing = false;
};
struct EntityGenResult {
	TArray<FTransform> instances;
	int sectionIdx = -1;
};
struct EntityGenRequest {
	int2 chunkAbsPos;
	int sectionIdx = -1;
};

struct EntityChunk {
	TArray<FPrimitiveInstanceId> instanceIndices;
	TArray<FTransform> instanceTransforms;
	bool isLoaded = false;
	bool isDirty = false;
};

struct EntityChunks {

	TArray<EntityChunk> sections;

};

struct SectionStatus {
	bool wentOutOfBounds = false;
	bool newlyAdded = false;
	bool hasContent = false;
};

enum GenStatus {
	IDLE,
	MESH_GEN_RESULT_IS_READY,
	FOLIAGE_UPDATE_IS_NEEDED,
};