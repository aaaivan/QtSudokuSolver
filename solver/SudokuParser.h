#ifndef SUDOKU_PARSER_H
#define SUDOKU_PARSER_H

#include <memory>
#include <vector>
#include <string>

class SudokuGrid;

void ParseSudokuFile(std::string filename, SudokuGrid* gridPtr);

#endif // !SUDOKU_PARSER_H
