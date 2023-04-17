#include "IO/SaveLoadManager.h"

const std::map<SaveLoadManager::ValueType, std::string> SaveLoadManager::sCandidateIdentifiersMap =
{
    {SaveLoadManager::ValueType::Option, "o"},
    {SaveLoadManager::ValueType::GivenCell, "g"},
    {SaveLoadManager::ValueType::HintedElimination, "h"},
};

const std::map<SaveLoadManager::RegionType, std::string> SaveLoadManager::sConstraintIdentifiersMap =
{
    {SaveLoadManager::RegionType::Region, "r"},
    {SaveLoadManager::RegionType::Killer, "k"},
    {SaveLoadManager::RegionType::PositiveDiagonal, "pd"},
    {SaveLoadManager::RegionType::NegativeDiagonal, "nd"},
};

const std::string SaveLoadManager::sSectionEndString = "-";
const std::string SaveLoadManager::sGridContentsString = "contents";
const std::string SaveLoadManager::sConstraintsString = "constraints";

SaveLoadManager::SaveLoadManager()
{
}

bool SaveLoadManager::ParseGrid(std::ifstream& theFile, PuzzleData& puzzleData) const
{
    if (!theFile.is_open())
        return false;

    std::string line;
    std::string token;

    while (std::getline(theFile, line))
    {
        if(line == (sSectionEndString + sGridContentsString))
        {
            break;
        }

        std::istringstream ss(line);
        ss >> token;
        if(token == sCandidateIdentifiersMap.at(ValueType::GivenCell))
        {
            if(!ParseGiven(ss, puzzleData)) return false;
        }
        else if (token == sCandidateIdentifiersMap.at(ValueType::HintedElimination))
        {
            if(!ParseHints(ss, puzzleData)) return false;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool SaveLoadManager::ParseGiven(std::istringstream& ss, PuzzleData& puzzleData) const
{
    std::string token;
    int id;
    int value;

    try
    {
        ss >> token;
        id = std::stoi(token);
        ss >> token;
        value = std::stoi(token);
    }
    catch (...)
    {
        return false;
    }

    puzzleData.mGivens[id] = value;
    return true;
}

bool SaveLoadManager::ParseHints(std::istringstream& ss, PuzzleData& puzzleData) const
{
    std::string token;
    int id;
    int value;

    try
    {
        ss >> token;
        id = std::stoi(token);
        puzzleData.mHints[id] = std::set<unsigned short>();
        while(ss >> token)
        {
            value = std::stoi(token);
            puzzleData.mHints[id].insert(value);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool SaveLoadManager::ParseConstraints(std::ifstream& theFile, PuzzleData& puzzleData) const
{
    std::string line;
    std::string token;

    while (std::getline(theFile, line))
    {
        if(line == (sSectionEndString + sConstraintsString))
        {
            break;
        }

        std::istringstream ss(line);
        ss >> token;

        if (token == sConstraintIdentifiersMap.at(RegionType::Region))
        {
            if(!ParseRegion(ss, puzzleData)) return false;
        }
        else if (token == sConstraintIdentifiersMap.at(RegionType::Killer))
        {
            if(!ParseKillerCage(ss, puzzleData)) return false;
        }
        else if (token == sConstraintIdentifiersMap.at(RegionType::PositiveDiagonal))
        {
            puzzleData.mPositiveDiagonal = true;
        }
        else if (token == sConstraintIdentifiersMap.at(RegionType::NegativeDiagonal))
        {
            puzzleData.mNegativeDiagonal = true;
        }
        else
        {
            return false;
        }
    }
    return true;
}

bool SaveLoadManager::ParseRegion(std::istringstream &ss, PuzzleData &puzzleData) const
{
    std::string token;
    int id;
    int coord;

    try
    {
        ss >> token;
        id = std::stoi(token);
        puzzleData.mRegions[id] = std::set<CellCoord>();

        while(ss >> token)
        {
            coord = std::stoi(token);
            puzzleData.mRegions[id].insert(coord);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

bool SaveLoadManager::ParseKillerCage(std::istringstream &ss, PuzzleData &puzzleData) const
{
    std::string token;
    int id;
    int total;
    int coord;

    try
    {
        ss >> token;
        id = std::stoi(token);
        ss >> token;
        total = std::stoi(token);
        puzzleData.mKillerCages[id] = std::pair<unsigned int, std::set<CellCoord>>(total, {});
        while(ss >> token)
        {
            coord = std::stoi(token);
            puzzleData.mKillerCages[id].second.insert(coord);
        }
    }
    catch (...)
    {
        return false;
    }

    return true;
}

std::string SaveLoadManager::SerializeGrid(const PuzzleData& puzzleData) const
{
    std::string s;
    s += sGridContentsString;
    s += '\n';

    for(const auto& g : puzzleData.mGivens)
    {
        s += SerializeGiven(g.first, g.second);
    }
    for(const auto& h : puzzleData.mHints)
    {
        s += SerializeHints(h.first, h.second);
    }
    s += sSectionEndString;
    s += sGridContentsString;
    s += '\n';
    return s;
}

std::string SaveLoadManager::SerializeGiven(CellCoord cell, unsigned short value) const
{
    std::string s;
    s += sCandidateIdentifiersMap.at(ValueType::GivenCell);
    s += ' ';
    s += std::to_string(cell);
    s += ' ';
    s += std::to_string(value);
    s += '\n';

    return s;
}

std::string SaveLoadManager::SerializeHints(CellCoord cell, std::set<unsigned short> hints) const
{
    std::string s;
    s += sCandidateIdentifiersMap.at(ValueType::HintedElimination);
    s += ' ';
    s += std::to_string(cell);
    for(const auto& h : hints)
    {
        s += ' ';
        s += std::to_string(h);
    }
    s += '\n';

    return s;
}

std::string SaveLoadManager::SerializeConstraints(const PuzzleData& puzzleData) const
{
    std::string s;
    s += sConstraintsString;
    s += '\n';

    // regions
    for (unsigned short i = 0; i < puzzleData.mRegions.size(); ++i)
    {
        const auto& r = puzzleData.mRegions[i];
        if(r.empty()) continue;
        s += SerializeRegion(i, r);
        s += '\n';
    }

    // diagonals
    if(puzzleData.mNegativeDiagonal)
    {
        s += sConstraintIdentifiersMap.at(RegionType::NegativeDiagonal);
        s += '\n';
    }
    if(puzzleData.mPositiveDiagonal)
    {
        s += sConstraintIdentifiersMap.at(RegionType::PositiveDiagonal);
        s += '\n';
    }

    // killers
    for (const auto& k : puzzleData.mKillerCages)
    {
        s += SerializeKillerCage(k.first, k.second.first, k.second.second);
        s += '\n';
    }

    s += sSectionEndString;
    s += sConstraintsString;
    s += '\n';

    return s;
}

std::string SaveLoadManager::SerializeRegion(unsigned short id, const CellsInRegion& region) const
{
    std::string s;

    s += sConstraintIdentifiersMap.at(RegionType::Region);
    s += ' ';
    s += std::to_string(id);

    for (const auto& cellId : region)
    {
        s += ' ';
        s += std::to_string(cellId);
    }
    return s;
}

std::string SaveLoadManager::SerializeKillerCage(unsigned int id, unsigned int sum, const CellsInRegion& cells) const
{
    std::string s;

    s += sConstraintIdentifiersMap.at(RegionType::Killer);
    s += ' ';
    s += std::to_string(id);
    s += ' ';
    s += std::to_string(sum);

    for (const auto& cellId : cells)
    {
        s += ' ';
        s += std::to_string(cellId);
    }
    return s;
}

const SaveLoadManager* SaveLoadManager::Get()
{
    static SaveLoadManager instance;
    return &instance;
}

bool SaveLoadManager::LoadSudoku(std::string filepath, PuzzleData& puzzleData) const
{
    std::ifstream theFile(filepath);
    if (theFile.is_open())
    {
        std::string line;
        try
        {
            std::getline(theFile, line);
            int gridSize = std::stoi(line);
            puzzleData = PuzzleData(gridSize);
        }
        catch (...)
        {
            return false;
        }

        // parse content of grid
        while (std::getline(theFile, line))
        {
            if(line == (sGridContentsString))
            {
                break;
            }
        }
        std::vector<std::vector<std::string>> parsedGrid;
        if (!ParseGrid(theFile, puzzleData)) return false;


        // parse constraints
        while (std::getline(theFile, line))
        {
            if(line == (sConstraintsString))
            {
                break;
            }
        }
        if (!ParseConstraints(theFile, puzzleData)) return false;
    }
    else
    {
        // TODO display error
        return false;
    }
    return true;
}

bool SaveLoadManager::SaveSudoku(std::string filepath, const PuzzleData& puzzleData) const
{
    std::ofstream theFile(filepath, std::ios::trunc);
    if (theFile.is_open())
    {
        // write the size of the grid
        theFile << std::to_string(puzzleData.mSize) << std::endl;

        // write the options left in each cell of the grid
        theFile << SerializeGrid(puzzleData);

        // write the regions/other constraints
        theFile << SerializeConstraints(puzzleData);
    }
    else
    {
        // TODO display error
        return false;
    }
    return true;
}
