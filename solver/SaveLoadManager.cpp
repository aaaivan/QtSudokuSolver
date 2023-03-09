#include "SaveLoadManager.h"
#include "SudokuGrid.h"
#include "VariantConstraints.h"
#include "SudokuCell.h"
#include "Region.h"
#include "RegionsManager.h"

const std::map<SaveLoadManager::ValueType, char> SaveLoadManager::identifiersMap =
{
	{SaveLoadManager::ValueType::GivenCell, 'g'},
	{SaveLoadManager::ValueType::HintedElimination, 'h'},
};

SaveLoadManager::SaveLoadManager(): mParsingInfoMap()
{
	// Rows parsing info
	ParsingInfo info =
	{
		'r',
		std::bind(&SaveLoadManager::SerializeRegion, this, 'r', std::placeholders::_1),
		std::bind(&SaveLoadManager::ParseRegion, this, RegionType::House_Row, std::placeholders::_1, std::placeholders::_2)
	};
	mParsingInfoMap.emplace(RegionType::House_Row, info);
	
	// Columns parsing info
	info =
	{
		'c',
		std::bind(&SaveLoadManager::SerializeRegion, this, 'c', std::placeholders::_1),
		std::bind(&SaveLoadManager::ParseRegion, this, RegionType::House_Column, std::placeholders::_1, std::placeholders::_2)
	};
	mParsingInfoMap.emplace(RegionType::House_Column, info);

	// Regions parsing info
	info =
	{
		'h',
		std::bind(&SaveLoadManager::SerializeRegion, this, 'h', std::placeholders::_1),
		std::bind(&SaveLoadManager::ParseRegion, this, RegionType::House_Region, std::placeholders::_1, std::placeholders::_2)
	};
	mParsingInfoMap.emplace(RegionType::House_Region, info);

	// Killer cages parsing info
	info =
	{
		'k',
		std::bind(&SaveLoadManager::SerializeKillerCage, this, 'k', std::placeholders::_1),
		std::bind(&SaveLoadManager::ParseKillerCage, this, std::placeholders::_1, std::placeholders::_2)
	};
	mParsingInfoMap.emplace(RegionType::KillerCage, info);
}

bool SaveLoadManager::ParseGrid(std::ifstream& theFile, std::vector<std::vector<std::string>>& outGrid, unsigned short size) const
{
	if (!theFile.is_open())
		return false;

	std::string line;
	std::string token;
	// loop through the grid line by line
	for (unsigned short row = 0; row < size; ++row)
	{
		outGrid.emplace_back();
		std::getline(theFile, line);
		std::istringstream ss(line);
		unsigned short col = 0;
		// loop through the cells in the row
		while (ss >> token)
		{
			outGrid.at(outGrid.size() - 1).push_back(token);
			++col;
		}
		if (col != size)
			return false;
	}
	return true;
}

bool SaveLoadManager::ParseCell(std::string token, unsigned short row, unsigned short col, SudokuGrid* grid) const
{
	std::string::iterator it = token.begin();
	std::set<unsigned short> options;
	std::set<unsigned short> hints;
	ValueType state = ValueType::None;

	// parse the viable options in the cell
	while (it != token.end())
	{
		char c = *it;
		if (c == identifiersMap.at(ValueType::GivenCell) && it == token.begin()) // given digit
		{
			state = ValueType::GivenCell;
		}
		else if (c == identifiersMap.at(ValueType::HintedElimination)) // hinted elimination
		{
			state = ValueType::HintedElimination;
		}
		else if (c >= '1' && c <= '9') // option
		{
			unsigned short value = c - '0';
			switch (state)
			{
			case ValueType::GivenCell:
				grid->AddGivenCell(row, col, value);
				return std::distance(it, token.end()) == 1;
			case ValueType::HintedElimination:
				hints.insert(value);
				options.insert(value);
				break;
			case ValueType::None:
				options.insert(value);
				break;
			default:
				break;
			}
			state = ValueType::None;
		}
		else
		{
			return false;
		}
		++it;
	}
	grid->SetCellOptions(row, col, options);
	grid->SetCellEliminationHints(row, col, hints);

	return state == ValueType::None;
}

bool SaveLoadManager::ParseConstraints(std::ifstream& theFile, SudokuGrid* grid) const
{
	std::string line;
	while (std::getline(theFile, line))
	{
		char id = line[0]; // region type identifier
		auto it = std::find_if(mParsingInfoMap.begin(), mParsingInfoMap.end(), [=](auto i) { return i.second.id == id; });

		if (it != mParsingInfoMap.end())
		{
			if (!it->second.deserialisationFunc(line.substr(2), grid)) return false;
		}
	}
	return true;
}

bool SaveLoadManager::ParseRegion(RegionType type, std::string line, SudokuGrid* grid) const
{
	std::vector<std::array<unsigned short, 2>> house;
	std::string token;
	std::istringstream ss(line);

	// parse the cells that make up the region
	unsigned short count = 0;
	while (ss >> token)
	{
		if (token.size() != 2)
			return false;

		unsigned short row = token[0] - '0';
		unsigned short col = token[1] - '0';
		if (row >= grid->SizeGet() || col >= grid->SizeGet())
			return false;

		house.push_back({ row, col });
		++count;
	}
	if (count != grid->SizeGet())
		return false;

	grid->DefineRegion(house, type);
	return true;
}

bool SaveLoadManager::ParseKillerCage(std::string line, SudokuGrid* grid) const
{
	std::vector<std::array<unsigned short, 2>> house;
	std::string token;
	std::istringstream ss(line);

	unsigned int sum = 0;
	try
	{
		// get cage sum
		ss >> token;
		unsigned int sum = std::stoi(token);
	}
	catch (...)
	{
		return false;
	}

	// parse the cells that make up the cage
	while (ss >> token)
	{
		if (token.size() != 2)
			return false;

		unsigned short row = token[0] - '0';
		unsigned short col = token[1] - '0';
		if (row >= grid->SizeGet() || col >= grid->SizeGet())
			return false;

		house.push_back({ row, col });
	}
	std::unique_ptr<VariantConstraint> killer = std::make_unique<KillerConstraint>(sum);
	grid->DefineRegion(house, RegionType::KillerCage, std::move(killer));
	return true;
}

std::string SaveLoadManager::SerializeGrid(const SudokuGrid* grid) const
{
	std::string s;
	for (unsigned short row = 0; row < grid->SizeGet(); ++row)
	{
		for (unsigned short col = 0; col < grid->SizeGet(); ++col)
		{
			s += SerializeCell(grid->CellGet(row, col));
			if (col < grid->SizeGet() - 1)
			{
				s += ' ';
			}
		}
		s += '\n';
	}
	return s;
}

std::string SaveLoadManager::SerializeCell(const SudokuCell* cell) const
{
	std::string s;
	if (cell->IsGiven())
	{
		s += identifiersMap.at(ValueType::GivenCell);
		s += std::to_string(cell->ValueGet());
	}
	else
	{
		const auto options = cell->OptionsGet();
		for (const auto& opt : options)
		{
			s += std::to_string(opt);
		}
		const auto hints = cell->HintedEliminationsGet();
		for (const auto& h : hints)
		{
			s += identifiersMap.at(ValueType::HintedElimination);
			s += std::to_string(h);
		}
	}
	return s;
}

std::string SaveLoadManager::SerializeConstraints(const SudokuGrid* grid) const
{
	std::string s;
	auto regions = grid->RegionsManagerGet()->StartingRegionsGet();

	for (size_t i = 0; i < regions.size(); ++i)
	{
		for (const auto& r : regions.at(i))
		{
			s += mParsingInfoMap.at(static_cast<RegionType>(i)).serialisationFunc(r.get());
			s += '\n';
		}
	}
	return s;
}

std::string SaveLoadManager::SerializeRegion(char id, const Region* region) const
{
	std::string s;

	s += id;
	for (const auto& c : region->CellsGet())
	{
		s += ' ';
		s += std::to_string(c->RowGet());
		s += std::to_string(c->ColGet());
	}
	return s;
}

std::string SaveLoadManager::SerializeKillerCage(char id, const Region* region) const
{
	std::string s;

	const KillerConstraint* killer = static_cast<const KillerConstraint*>(region->GetConstraintByType(RegionType::KillerCage));
	if (!killer) return s;

	s += id;
	s += ' ';
	s += std::to_string(killer->SumGet());

	for (const auto& c : region->CellsGet())
	{
		s += ' ';
		s += std::to_string(c->RowGet());
		s += std::to_string(c->ColGet());
	}
	return s;
}

const SaveLoadManager* SaveLoadManager::Get()
{
	static SaveLoadManager instance;
	return &instance;
}

std::unique_ptr<SudokuGrid> SaveLoadManager::LoadSudoku(std::string filepath) const
{
	std::unique_ptr<SudokuGrid> gridPtr;
	std::ifstream theFile(filepath);
	if (theFile.is_open())
	{
		std::string line;
		std::getline(theFile, line);
		int gridSize = std::stoi(line);
		gridPtr = std::make_unique<SudokuGrid>(static_cast<unsigned short>(gridSize));

		// parse content of grid
		std::vector<std::vector<std::string>> parsedGrid;
		if (!ParseGrid(theFile, parsedGrid, gridPtr->SizeGet())) return nullptr;

		// parse constraints
		if (!ParseConstraints(theFile, gridPtr.get())) return nullptr;

		// parse individual cells
		for (unsigned short row = 0; row < parsedGrid.size(); ++row)
		{
			std::vector<std::string> rowVector = parsedGrid.at(row);
			for (unsigned short col = 0; col < rowVector.size(); ++col)
			{
				if (!ParseCell(rowVector.at(col), row, col, gridPtr.get())) return nullptr;
			}
		}
	}
	else
	{
		// TODO display error
		return nullptr;
	}

	return gridPtr;
}

bool SaveLoadManager::SaveSudoku(std::string filepath, const SudokuGrid* grid) const
{
	std::ofstream theFile(filepath, std::ios::trunc);
	if (theFile.is_open())
	{
		// write the size of the grid
		theFile << std::to_string(grid->SizeGet()) << std::endl;

		// write the options left in each cell of the grid
		theFile << SerializeGrid(grid);

		// write the regions/other constraints
		theFile << SerializeConstraints(grid);
	}
	else
	{
		// TODO display error
		return false;
	}
	return true;
}
