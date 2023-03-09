#include "SudokuParser.h"
#include "SudokuGrid.h"
#include <iostream>
#include <fstream>
#include <sstream>

void ParseSudokuFile(std::string filename, SudokuGrid* gridPtr)
{
    std::vector<std::vector<unsigned short>> grid;

	std::ifstream theFile(filename);
    if (theFile.is_open())
    {
        std::string line;
        std::getline(theFile, line);
        int gridSize = std::stoi(line);
        grid.reserve(gridSize);

        for (auto& row : grid)
        {
            row.reserve(gridSize);
        }

        char digit = 0;

		for (size_t r = 0; r < gridSize; r++)
		{
			grid.push_back({});
			for (size_t c = 0; c < gridSize; c++)
			{
				theFile >> digit;
				grid.back().push_back(digit - '0');
			}
		}

        theFile.close();
    }
#if PRINT_LOG_MESSAGES
    else
    {
        std::string message = "IVP - Could not open the file.\n";
        printf(message.c_str());
    }
#endif // PRINT_LOG_MESSAGES
	for (unsigned short row = 0; row < grid.size(); ++row)
	{
		for (unsigned short col = 0; col < grid.at(row).size(); ++col)
		{
			gridPtr->AddGivenCell(row, col, grid.at(row).at(col));
		}
	}
}
