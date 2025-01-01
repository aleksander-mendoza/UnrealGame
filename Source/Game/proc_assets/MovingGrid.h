#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "MovingGrid.generated.h"


USTRUCT(BlueprintType)
struct GAME_API FMovingGrid
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float chunkSize = 1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int radius = 0;

	int diameter = 0;
	TArray<int> unusedSections;
	TArray<int> chunkToSection;
	TArray<SectionStatus> sections;
	TArray<ChunkDist> chunkDistances;
	int2 absChunkOffset;
	bool playerCrossedChunkBoundary;
	bool couldHaveMoreChunksToLoad = false;

	inline const int getRadius() {
		return radius;
	}
	inline const int getDiameter() {
		return diameter;
	}
	inline const int getArea() {
		return diameter * diameter;
	}
	inline void setChunkSize(float cs) {
		this->chunkSize = cs;
	}
	inline void setCenterPos(const double3 playerPosition) {
		absChunkOffset = getChunkAbsPosFromWorldPos(playerPosition);
	}
	inline void reset(const double3 playerPosition) {
		setCenterPos(playerPosition);
		reset();
	}
	inline void reset() {
		setGridSize(radius);
	}
	inline void setGridSize(int distance) {
		this->radius = distance;
		diameter = distance * 2 + 1;
		const int area = diameter * diameter;
		sections.Empty();
		sections.Reserve(area);
		chunkToSection.Empty();
		chunkToSection.Reserve(area);
		unusedSections.Empty();
		unusedSections.Reserve(area);
		chunkDistances.Empty();
		chunkDistances.Reserve(area);
		for (int i = 0; i < area; i++) {
			chunkToSection.Add(-1);
			unusedSections.Add(i);
			sections.AddDefaulted();
		}

		for (int y = 0, i = 0; y < diameter; y++) {
			for (int x = 0; x < diameter; x++, i++) {
				const int2 posFromCenter(x - radius, y - radius);
				const float distL2 = math::sqrt(math::dot(posFromCenter, posFromCenter));
				//const int distL1 = math::sum(math::abs(posFromCenter));
				const int distLinf = math::max(math::abs(posFromCenter.X), math::abs(posFromCenter.Y));
				chunkDistances.Add(ChunkDist{ .distL2 = distL2, .distLinf = distLinf,.chunkIdx = i,.chunkPos = posFromCenter });
			}
		}
		chunkDistances.Sort([](const ChunkDist& ip1, const ChunkDist& ip2) {
			return  ip1.distL2 < ip2.distL2;
			});
		check(sections.Num() == area);
		check(unusedSections.Num() == area);
		check(chunkToSection.Num() == area);
		check(chunkDistances.Num() == area);
		check(chunkDistances[0].distL2 <= chunkDistances.Last().distL2);
	}

	inline const int getChunkIdx(int2 chunkPos) const {
		if (chunkPos.Y < 0 || chunkPos.Y >= diameter || chunkPos.X < 0 || chunkPos.X >= diameter)return -1;
		int chunkIdx = chunkPos.X + diameter * chunkPos.Y;
		return chunkIdx;
	}
	inline const int2 getChunkPos(int chunkIdx) const {
		int chunkX = chunkIdx % diameter;
		int chunkY = chunkIdx / diameter;
		return int2(chunkX, chunkY);
	}
	inline const int getSectionIdx(int chunkIdx) const {
		if (chunkIdx < 0) return -1;
		return this->chunkToSection[chunkIdx];
	}
	inline const int getSectionIdx(int2 chunkPos) const {
		return getSectionIdx(getChunkIdx(chunkPos));
	}
	inline const int setSectionIdx(int chunkIdx, int sectionIdx) {
		if (chunkIdx == -1)return -1;
		int prev = this->chunkToSection[chunkIdx];
		this->chunkToSection[chunkIdx] = sectionIdx;
		return prev;
	}
	inline const int setSectionIdx(int2 chunkRelPos, int sectionIdx) {
		return setSectionIdx(getChunkIdx(chunkRelPos), sectionIdx);
	}
	inline void dropChunk(int chunkIdx) {
		const int sectionIdx = this->chunkToSection[chunkIdx];
		if (sectionIdx > -1) {
			unusedSections.Add(sectionIdx);
			this->chunkToSection[chunkIdx] = -1;
			if (sections[sectionIdx].hasContent) { //TODO: make this an atomic operation
				sections[sectionIdx].wentOutOfBounds = true;
			}
			sections[sectionIdx].newlyAdded = false;
		}
	}
	inline void addChunk(int chunkIdx) {
		if (this->chunkToSection[chunkIdx] == -1) {
			const int sectionIdx = this->chunkToSection[chunkIdx] = unusedSections.Pop();
			sections[sectionIdx].newlyAdded = true;
		}
	}
	inline const int2 getChunkAbsPosFromWorldPos(double posX, double posY) const {
		return int2(math::floor(posX / chunkSize), math::floor(posY / chunkSize));
	}
	inline const int2 getChunkAbsPosFromWorldPos(FVector pos) const {
		return getChunkAbsPosFromWorldPos(pos.X, pos.Y);
	}

	inline const int2 getAbsOffsetToBottomLeftmostChunk() {
		return int2(this->absChunkOffset.X - radius, this->absChunkOffset.Y - radius);
	}
	inline const int2 absToRelPos(int2 chunkAbsPos) {
		return chunkAbsPos - getAbsOffsetToBottomLeftmostChunk();
	}
	inline const int2 relToAbsPos(int2 chunkRelPos) {
		return chunkRelPos + getAbsOffsetToBottomLeftmostChunk();
	}


	void shiftSurroundingChunks(int2 shift) {
		if (diameter <= 1)return;
		shift.X = math::sgn(shift.X);
		shift.Y = math::sgn(shift.Y);
		int offset = shift.X + shift.Y * diameter;
		int dX = 1, dY = 1, sX = 0, sY = 0, eX = diameter, eY = diameter;
		if (shift.X != 0) {
			if (shift.X > 0) {
				dX = -1;
				sX = diameter - 1;
				eX = 0;
			}
			else {
				sX = 0;
				eX = diameter - 1;
			}

			for (int y = 0; y < diameter; y++) {
				dropChunk(sX + y * diameter);
			}
		}
		if (shift.Y != 0) {
			if (shift.Y > 0) {
				dY = -1;
				sY = diameter - 1;
				eY = 0;
			}
			else {
				sY = 0;
				eY = diameter - 1;
			}

			for (int x = 0; x < diameter; x++) {
				dropChunk(x + sY * diameter);
			}
		}

		for (int x = sX; x != eX; x += dX) {
			for (int y = sY; y != eY; y += dY) {
				const int toIdx = x + y * diameter;
				const int fromIdx = toIdx - offset;
				chunkToSection[toIdx] = chunkToSection[fromIdx];
			}
		}
		if (shift.X != 0) {
			for (int y = 0; y < diameter; y++) {
				chunkToSection[eX + y * diameter] = -1;
			}
		}
		if (shift.Y != 0) {
			for (int x = 0; x < diameter; x++) {
				chunkToSection[x + eY * diameter] = -1;
			}
		}

	}
	template<bool l2>
	inline void addChunksWithinRadius(float r) {
		for (int i = 0; i < chunkDistances.Num(); i++) {
			const ChunkDist& chunk = chunkDistances[i];
			if ((l2 ? chunk.distL2 : chunk.distLinf) <= r) {
				addChunk(chunk.chunkIdx);
				
			}
			else if (l2) break; // chunkDistances are sorted using l2 metric so we know we can terminate early
		}
	}
	inline int getNearestNewlyAddedSection() const{
		for (int i = 0; i < chunkDistances.Num(); i++) {
			const ChunkDist& chunk = chunkDistances[i];
			const int sectionIdx = getSectionIdx(chunk.chunkIdx);
			if (sections[sectionIdx].newlyAdded) {
				return sectionIdx;
			}
		}
		return -1;
	}

	FString toDebugStr() const
	{
		FString s;
		for (int y = diameter - 1; y >= 0; y--) {
			for (int x = 0; x < diameter; x++) {
				const int sectionIdx = getSectionIdx(int2(x, y));
				if (sectionIdx > -1) {
					const SectionStatus& sts = sections[sectionIdx];
					s += FString::Printf(TEXT("%3d"), sectionIdx);
					s += sts.newlyAdded ? '+' : '_';
					s += sts.hasContent ? '*' : '_';
					if (sts.wentOutOfBounds) s += '?';
				}
				else {
					s += " -1  ";
				}
				s += ' ';
			}
			s += '\n';
		}
		return s;
	}
	inline bool update(const double3 playerPos, const float spawnRadius) {
		const int2 pos = getChunkAbsPosFromWorldPos(playerPos);
		if (pos != absChunkOffset) {
			const int2 shift = absChunkOffset - pos;
			shiftSurroundingChunks(shift);
			absChunkOffset = pos;
			addChunksWithinRadius<false>(spawnRadius);
			playerCrossedChunkBoundary = true;
			return true;
		}
		return false;
	}
};