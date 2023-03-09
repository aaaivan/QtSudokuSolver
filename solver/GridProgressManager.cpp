#include "GridProgressManager.h"
#include "Progress.h"
#include "Region.h"
#include "RegionUpdatesManager.h"
#include "RegionsManager.h"

GridProgressManager::GridProgressManager(SudokuGrid* sudoku) :
	mSudokuGrid(sudoku),
	mProgressQueue(),
	mQueuedActions(),
	mTechniques(static_cast<size_t>(TechniqueType::MAX_TECHNIQUES)),
	mCurrentTechnique(static_cast<TechniqueType>(0)),
	mFinished(false),
	mPaused(false)
{
	mTechniques[static_cast<size_t>(TechniqueType::LockedCandidates)] =
		std::make_unique<LockedCandidatesTechnique>(sudoku, SolvingTechnique::ObserveValues);

	mTechniques[static_cast<size_t>(TechniqueType::NakedSubset)] =
		std::make_unique<NakedSubsetTechnique>(sudoku, SolvingTechnique::ObserveCells);

	mTechniques[static_cast<size_t>(TechniqueType::HiddenNakedSubset)] =
		std::make_unique<HiddenNakedSubsetTechnique>(sudoku, SolvingTechnique::ObserveValues);

	mTechniques[static_cast<size_t>(TechniqueType::Fish)] =
		std::make_unique<FishTechnique>(sudoku, SolvingTechnique::ObserveValues);
}

const SolvingTechnique* GridProgressManager::TechniqueGet(TechniqueType type)
{
	return mTechniques[static_cast<size_t>(type)].get();
}

bool GridProgressManager::HasFinished()
{
	return mFinished;
}

void GridProgressManager::RegisterProgress(std::shared_ptr<Progress>&& deduction)
{
	if (deduction->isHighPriotity())
	{
		mHighPriorityProgressQueue.push(deduction);
	}
	else
	{
		mProgressQueue.push(deduction);
	}
}

void GridProgressManager::RegisterFailure(TechniqueType type, Region* region, SudokuCell* cell /* = nullptr */, unsigned short value /* = 0 */)
{
	if (value)
	{
		region->UpdateManagerGet()->OnTechniqueFailed(type, value);
	}
	else if (cell)
	{
		region->UpdateManagerGet()->OnTechniqueFailed(type, cell);
	}
	else
	{
		region->UpdateManagerGet()->OnTechniqueFailed(type);
	}
#if PRINT_LOG_MESSAGES
	printf("Technique %u failed on region:", static_cast<size_t>(type));
	printf("%s, ", region->IdGet().c_str());
	printf("\n");
#endif // PRINT_LOG_MESSAGES
}

void  GridProgressManager::NextStep()
{
	if (!mHighPriorityProgressQueue.empty())
	{
		Reset();
		mHighPriorityProgressQueue.front()->ProcessProgress();
		mHighPriorityProgressQueue.pop();
	}
	else if (!mProgressQueue.empty())
	{
		Reset();
		mProgressQueue.front()->ProcessProgress();
		mProgressQueue.pop();
	}
	else if (!mQueuedActions.empty())
	{
		while (!mQueuedActions.empty())
		{
			mQueuedActions.front()();
			mQueuedActions.pop();
		}
	}
	else if (!mPaused && !mFinished)
	{
		NextTechnique();
	}
}

void GridProgressManager::Clear()
{
	{
		std::queue<std::shared_ptr<Progress>> empty;
		std::swap(mProgressQueue, empty);
	}
	{
		std::queue<std::shared_ptr<Progress>> empty;
		std::swap(mHighPriorityProgressQueue, empty);
	}
	Reset();
}

void GridProgressManager::Reset()
{
	mCurrentTechnique = static_cast<TechniqueType>(0);
	mFinished = false;
	for (const auto& t : mTechniques)
	{
		t->Reset();
	}
}

void GridProgressManager::NextTechnique()
{
	size_t index = static_cast<size_t>(mCurrentTechnique);
	mTechniques.at(index)->NextStep();
	if (mTechniques.at(index)->HasFinished())
	{
		++index;
		mCurrentTechnique = static_cast<TechniqueType>(index);
		if (index >= mTechniques.size())
		{
			mFinished = true;
		}
	}
}