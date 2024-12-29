#pragma once

#include "CoreMinimal.h"
#include "WorldGenUtils.h"
#include "MovingGrid.generated.h"


USTRUCT(BlueprintType)
struct GAME_API FMovingGrid
{
	GENERATED_BODY()


	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	float chunkSize=1000;

	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	int radius = 0;
	
	int diameter=0;
	TArray<int> unusedSections;
	TArray<int> chunkToSection;
	TArray<SectionStatus> sections;
	TArray<ChunkDist> chunkDistances;

	inline const int getRadius() {
		return radius;
	}
	inline const int getDiameter() {
		return diameter;
	}
	inline const int getArea() {
		return diameter * diameter;
	}
	inline void setChunkSize(float chunkSize) {
		this->chunkSize = chunkSize;
	}
	inline void resetGridSize() {
		setGridSize(radius);
	}
	inline void setGridSize(int radius) {
		this->radius = radius;
		diameter = radius * 2 + 1;
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
			unusedSections.AddDefaulted();
		}

		for (int y = 0, i = 0; y < diameter; y++) {
			for (int x = 0; x < diameter; x++, i++) {
				const int2 posFromCenter(x - radius, y - radius);
				const float distL2 = math::sqrt(math::dot(posFromCenter, posFromCenter));
				const int distL1 = math::sum(math::abs(posFromCenter));
				chunkDistances.Add(ChunkDist{ .distL2 = distL2, .distL1 = distL1,.chunkIdx = i,.chunkPos = posFromCenter });
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

	inline const int getChunkIdx(int2 chunkPos) {
		if (chunkPos.Y < 0 || chunkPos.Y >= diameter || chunkPos.X < 0 || chunkPos.X >= diameter)return -1;
		int chunkIdx = chunkPos.X + diameter * chunkPos.Y;
		return chunkIdx;
	}
	inline const int2 getChunkPos(int chunkIdx) {
		int chunkX = chunkIdx % diameter;
		int chunkY = chunkIdx / diameter;
		return int2(chunkX, chunkY);
	}
	inline const int getSectionIdx(int chunkIdx) {
		if (chunkIdx < 0) return -1;
		return this->chunkToSection[chunkIdx];
	}
	inline const int getSectionIdx(int2 chunkPos) {
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
		dropSection(getSectionIdx(chunkIdx));
	}
	inline void dropSection(int sectionIdx) {
		if (sections[sectionIdx].hasContent) { //TODO: make this an atomic operation
			sections[sectionIdx].wentOutOfBounds = true;
		}
		else {
			sections[sectionIdx].newlyAdded = false;
		}
	}
	inline void addChunk(int chunkIdx) {
		dropSection(getSectionIdx(chunkIdx));
	}
	inline void addSection(int sectionIdx) {
		sections[sectionIdx].newlyAdded = true;
	}
	inline const int2 getChunkAbsPosFromWorldPos(double posX, double posY) {
		return int2(floor(posX / chunkSize), floor(posY / chunkSize));
	}
	inline const int2 getChunkAbsPosFromWorldPos(FVector pos) {
		return getChunkAbsPosFromWorldPos(pos.X, pos.Y);
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
	inline bool addChunksWithinRadius(float radius) {
		for (int i = 0; i < chunkDistances.Num(); i++) {
			const ChunkDist& chunk = chunkDistances[i];
			if (chunk.distL2 > radius)break;
			addChunk(chunk.chunkIdx);
		}
	}
	inline int getNearestNewlyAddedSection() {
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
				s += FString::Printf(TEXT("%3d"), getSectionIdx(int2(x, y)));
				s += ' ';
			}
			s += '\n';
		}
		return s;
	}
};