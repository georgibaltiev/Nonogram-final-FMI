/**
*
* Solution to course project # 1
* Introduction to programming course
* Faculty of Mathematics and Informatics of Sofia University
* Winter semester 2022/2023
*
* @author Georgi Baltiev
* @idnumber 2MI0600168
* @compiler VC
*
* <main file>
*
*/


#include <iostream>
#include <fstream>
#include <cstdlib>

//used constants
const size_t MAX_NAME_LENGTH = 20;
const size_t MAX_NAME_LENGTH_WITH_EXTENSION = 24;
const size_t PUZZLE_NAME_LENGTH = 12;
const size_t MAX_READER_LENGTH = 256;

void clearConsole() {
	std::cout << "\033[;H";
	std::cout << "\033[J";
}

void printDashLine(size_t length)
{
	// this method prints a dashline for the nonogram grid.
	// GRID_BUFFER serves as a way to locate where '+' should be printed. 
	// This is for aesthetic purposes only
	const size_t NEW_LENGTH = 2 * length - 1;
	const size_t GRID_BUFFER = 4;
	for (int i = 0; i < NEW_LENGTH; i++)
	{
		std::cout << ((i % GRID_BUFFER == 0) ? '+' : '-');
	}
	std::cout << std::endl;
}

void printOutTable(unsigned** mainGrid, unsigned** rowClues, unsigned** colClues, size_t gridSize, size_t maximumRowClues, size_t maximumColClues)
{
	const size_t TABLE_SIZE = gridSize * 2 + 1;
	const size_t ROW_BUFFER = 3 * maximumRowClues;
	const size_t COL_BUFFER = maximumColClues;
	const size_t CLUES_COL_SIZE = gridSize;
	const size_t COL_CLUE_SCANNER_BUFFER = CLUES_COL_SIZE - maximumColClues;
	const size_t ROW_CLUE_SCANNER_BUFFER = CLUES_COL_SIZE - maximumRowClues;

	for (size_t line = 0; line < COL_BUFFER; line++)
	{
		for (size_t cornerBuffer = 0; cornerBuffer < ROW_BUFFER; cornerBuffer++)
		{
			std::cout << " ";
		}
		std::cout << "|";
		for (size_t colClueScanner = 0; colClueScanner < gridSize; colClueScanner++)
		{
			std::cout << ' ';
			if (colClues[colClueScanner][COL_CLUE_SCANNER_BUFFER + line] == 0)
				std::cout << ' ';
			else
				std::cout << colClues[colClueScanner][COL_CLUE_SCANNER_BUFFER + line];
			std::cout << " |";
		}
		std::cout << std::endl;
	}

	for (size_t linePrinter = 0; linePrinter < ROW_BUFFER; linePrinter++)
	{
		std::cout << "-";
	}

	const size_t BOARD_LENGTH = gridSize * 2 + 1;

	printDashLine(BOARD_LENGTH);

	// we have printed the top section of the puzzle - the corner buffer and the top clues

	for (size_t line = 0; line < gridSize; line++)
	{

		for (size_t rowClueScanner = ROW_CLUE_SCANNER_BUFFER; rowClueScanner < CLUES_COL_SIZE; rowClueScanner++)
		{
			std::cout << " ";
			if (rowClues[line][rowClueScanner] != 0)
				std::cout << rowClues[line][rowClueScanner];
			else
				std::cout << " ";
			std::cout << " ";
		}

		for (size_t boardScanner = 0; boardScanner < gridSize; boardScanner++)
		{
			std::cout << "| ";
			if (mainGrid[line][boardScanner] == 0) std::cout << ' ';
			else if (mainGrid[line][boardScanner] == 1) std::cout << '#';
			else std::cout << '/';
			std::cout << " ";
		}
		std::cout << "| " << line + 1;

		std::cout << std::endl;

		for (size_t linePrinter = 0; linePrinter < ROW_BUFFER; linePrinter++)
		{
			std::cout << "-";
		}

		printDashLine(BOARD_LENGTH);
	}
	for (size_t linePrinter = 0; linePrinter <= ROW_BUFFER; linePrinter++) {
		std::cout << " ";
	}

	for (size_t numberCounter = 0; numberCounter < gridSize; numberCounter++) {
		std::cout << " " << (numberCounter + 1) << "  ";
	}
	std::cout << std::endl;
}

void printPlayingOptions() {
	std::cout << "Enter the symbol you would like to mark ('#' - for a clue or '/' - for a non-clue) or type 'e' to terminate your game: ";
}

void printLevelScoreAndAttempts(size_t level, size_t score, int attempts) {
	// This one-liner sits above the table and it shows the player their progress
	std::cout << "Level: " << level << " | Score: " << score  << " | Amount of attempts remaining: " << attempts  << std::endl;
}

void initArrayWithZeroes(unsigned** arr, size_t rowSize, size_t colSize)
// this method fills up a 2-dimensional array with zeros. we need it because sometimes we have to 'delete' our data
{
	for (size_t i = 0; i < rowSize; i++)
	{
		for (size_t j = 0; j < colSize; j++)
		{
			arr[i][j] = 0;
		}
	}
}

void expandPointerSpace(unsigned** arr, size_t rowSize, size_t colSize)
// this method initializes the second dimension of a two-dimensional array
{
	for (size_t i = 0; i < rowSize; i++)
	{
		arr[i] = new unsigned[colSize];
	}
}

size_t initializeLongestSequenceOfClues(unsigned** clues, size_t rowDimension)
{
	// ths method returns the longest row sequence of clues in a clue array. 
	// the return value is later used in order to calculate how many intervals should be printed in order to accomodate all of the clues that will get displayed
	// e.g.
	// 0 0 1 2		0 1 2 
	// 0 2 1 1  ->	2 1 1 
	// 0 0 0 1		0 0 1

	const size_t colDimension = rowDimension;
	size_t maxAmountOfClues = 0;
	for (size_t i = 0; i < rowDimension; i++)
	{
		size_t amountOfClues = 0;
		for (size_t j = 0; j < colDimension; j++)
		{
			if (clues[i][j] != 0)
				amountOfClues++;
		}
		if (amountOfClues > maxAmountOfClues)
			maxAmountOfClues = amountOfClues;
	}
	return maxAmountOfClues;
}

void fillClues(unsigned** grid, size_t size, unsigned** rowClues, unsigned** colClues)
// this method scans the puzzle and fills the clue holders with clues - rows and columns
{
	for (size_t i = 0; i < size; i++)
	{
		unsigned counter = 0;
		size_t rowCluesCounter = 0;
		for (size_t j = 0; j < size; j++)
		{
			if (grid[i][j] == 0)
			{
				rowClues[i][rowCluesCounter] = counter;
				counter = 0;
				rowCluesCounter++;
			}
			else
				counter++;
		}
		rowClues[i][rowCluesCounter] = counter;
	}

	for (size_t i = 0; i < size; i++)
	{
		unsigned counter = 0;
		size_t colCluesCounter = 0;
		for (size_t j = 0; j < size; j++)
		{
			if (grid[j][i] == 0)
			{
				colClues[i][colCluesCounter] = counter;
				counter = 0;
				colCluesCounter++;
			}
			else
				counter++;
		}
		colClues[i][colCluesCounter] = counter;
	}
}

void swap(unsigned& alpha, unsigned& beta)
{
	unsigned temp = alpha;
	alpha = beta;
	beta = temp;
}

void rightShiftClues(unsigned** clue, size_t rowSize, size_t colSize)
{
	// this method is used to stick the non-null elements to the right side of the clues matrices
	// e.g - here our clues are more dispersed due to the location of our elements:
	// 0 1 2	0 1 2
	// 2 0 0 -> 0 0 2
	// 0 1 0	0 0 1
	// the method is needed to prevent 'holes' and weird gaps when we print our clues
	for (size_t i = 0; i < rowSize; i++)
	{
		for (size_t j = 0; j < colSize; j++)
		{
			if (clue[i][j] == 0)
			{
				for (size_t k = j; k > 0; k--)
				{
					swap(clue[i][k], clue[i][k - 1]);
				}
			}
		}
	}
}

void printIntroduction() {
	std::cout << "Welcome to Nonogram! \nPlease pick an option! \n1. Log in \n2. Create a new account \nI choose: ";
}

void printInvalidMenuChoice() {
	std::cout << "Incorrect option! please try again!: ";
}

void printIncorrectInput() {
	std::cout << "Incorrect input! Please try again: ";
}

bool userInputDataIsValid(char* alpha) {
	for (size_t index = 0; index < 20; index++) {
		if (alpha[index] == '\0') return true;
	}
	return false;
}

void enterAClue(unsigned** arr, size_t rowIndex, size_t colIndex) {
	arr[rowIndex][colIndex] = 1;
}

void enterANonClue(unsigned** arr, size_t rowIndex, size_t colIndex) {
	arr[rowIndex][colIndex] = 2;
}

void clearClue(unsigned** arr, size_t rowIndex, size_t colIndex) {
	arr[rowIndex][colIndex] = 0;
}

void initializeFileName(char* username, char* fileName) {
	// this used in order to add '.txt' to our username to render it valid for the ifstream object
	size_t index = 0;
	while (username[index] != 0) {
		fileName[index] = username[index];
		index++;
	}
	fileName[index] = '.';
	fileName[index + 1] = 't';
	fileName[index + 2] = 'x';
	fileName[index + 3] = 't';
	fileName[index + 4] = '\0';
}

bool equals(char* alpha, char* beta) {
	size_t index = 0;
	while (alpha[index] != 0 && beta[index] != 0) {
		if (alpha[index] != beta[index]) return false;
		index++;
	}
	return alpha[index] == beta[index];
}

void convertAMatrixIntoAString(char* string, unsigned** matrix, size_t rowSize, size_t colSize) {
	for (size_t i = 0; i < rowSize; i++) {
		for (size_t j = 0; j < colSize; j++) {
			string[i * rowSize + j] = matrix[i][j] + '0';
		}
	}
	string[rowSize * colSize] = '\0';
}

void annullString(char* string, size_t length) {
	// this method gets used when we save our file and the player has lost all of their progress on the current level
	for (size_t i = 0; i < length; i++) {
		string[i] = '0';
	}
}

void pickAPuzzle(size_t level, char* fileName) {
	// our puzzles are under the location of ../fk_l.txt where k is the level and l is a random number (1-6)
	srand(time(0));
	unsigned random = (unsigned) rand() % 6 + 1;
	fileName[0] = '.';
	fileName[1] = '.';
	fileName[2] = '/';
	fileName[3] = 'f';
	fileName[4] = (level + '0');
	fileName[5] = '_';
	fileName[6] = (random + '0');
	fileName[7] = '.';
	fileName[8] = 't';
	fileName[9] = 'x';
	fileName[10] = 't';
	fileName[11] = '\0';
}

void convertAStringIntoAPuzzleGrid(char* string, unsigned** grid, size_t dimension) {
	//this is being used to read the progress string located in each of the accounts and also to extract our puzzle data
	for (size_t i = 0; i < dimension; i++) {
		for (size_t j = 0; j < dimension; j++) {
			grid[i][j] = (unsigned)(string[(i * dimension) + j] - '0');
		}
	}
}

bool theGridsMatch(unsigned** playerGrid, unsigned** puzzleGrid, size_t gridDimension) {
	for (size_t i = 0; i < gridDimension; i++) {
		for (size_t j = 0; j < gridDimension; j++) {
			if (puzzleGrid[i][j] == 1) {
				if (playerGrid[i][j] == 1) continue;
				else return false;
			}
			else {
				if (playerGrid[i][j] == 1) return false;
				else continue;
			}
		}
	}
	return true;
}

void saveUserInfo(std::ofstream& fileWriter, char* userFile, char* password, size_t level, size_t score, char* puzzleName, unsigned** playerGrid, size_t gridSize) {
	const size_t MATRIX_STRING_LENGTH = gridSize * gridSize + 1; // the length of the string corresponds to the amount of elements in the n*n grid plus one slot for '\0' - hence the formula	
	char* progressString = new char[MATRIX_STRING_LENGTH];
	convertAMatrixIntoAString(progressString, playerGrid, gridSize, gridSize);
	fileWriter.open(userFile);
	fileWriter.trunc;
	fileWriter << password << std::endl;
	fileWriter << level << std::endl;
	fileWriter << score << std::endl;
	fileWriter << puzzleName << std::endl;
	fileWriter << progressString << std::endl;
	fileWriter.close();
	delete[] progressString;
	delete[] userFile;
	delete[] password;
	delete[] puzzleName;
}

void validateDataInput(char* data, bool validInput) {
	// this is an extended version of the method that checks if we work with valid inputs (length-wise), because we work with a lot of inputs and we can avoid a lot of code
	do {
		std::cin >> data;
		if (!userInputDataIsValid(data)) {
			validInput = false;
			printIncorrectInput();
		}
		else validInput = true;
	} while (!validInput);
}

void closeMatricesAfterTheRoundEnds(unsigned** puzzle, unsigned** playerGrid, unsigned** rowClues, unsigned** colClues, size_t gridSize) /* this method is used to delete the data that was relevant for the round.We require this in order to 'expand' our matrices for the next round*/ {
	//this method is used in order to delete all of our player data at once 
	for (size_t i = 0; i < gridSize; i++) {
		delete[] puzzle[i];
		delete[] playerGrid[i];
		delete[] rowClues[i];
		delete[] colClues[i];
	}
	delete[] puzzle;
	delete[] playerGrid;
	delete[] rowClues;
	delete[] colClues;
}

void readAndInitializePuzzleFromFile(char* fileName, size_t level, unsigned** puzzle, unsigned** rowClues, unsigned** colClues, size_t& maxRowBuffer, size_t& maxColBuffer) {
	
	size_t gridSize = level + 4;

	expandPointerSpace(puzzle, gridSize, gridSize);
	initArrayWithZeroes(puzzle, gridSize, gridSize);

	expandPointerSpace(rowClues, gridSize, gridSize);
	initArrayWithZeroes(rowClues, gridSize, gridSize);

	expandPointerSpace(colClues, gridSize, gridSize);
	initArrayWithZeroes(colClues, gridSize, gridSize);

	std::ifstream reader;
	char* puzzleString = new char[gridSize * gridSize + 1]; // the size is set that way because it needs to accomodate size^2 elements and the terminating zero - hence the + 1
	reader.open(fileName);
	reader >> puzzleString;
	reader.close();
	convertAStringIntoAPuzzleGrid(puzzleString, puzzle, gridSize);

	fillClues(puzzle, gridSize, rowClues, colClues);

	rightShiftClues(rowClues, gridSize, gridSize);
	rightShiftClues(colClues, gridSize, gridSize);

	maxRowBuffer = initializeLongestSequenceOfClues(rowClues, gridSize);
	maxColBuffer = initializeLongestSequenceOfClues(colClues, gridSize);

	delete[] puzzleString;
}

int main()
{
	printIntroduction();

	char menuNavigator; // this variable is used in order to navigate the options
	
	const char LOGIN = '1';
	const char NEW_ACCOUNT = '2';

	std::cin >> menuNavigator;
	while (menuNavigator != LOGIN && menuNavigator != NEW_ACCOUNT) {
		printInvalidMenuChoice();
		std::cin >> menuNavigator;
	}

	char* username;
	char* userFile;
	char* password = new char[MAX_NAME_LENGTH];
	char* puzzleName;

	std::ifstream fileSearcherAndReader; // this is used in order to read user accounts or to check if they are real
	std::ofstream fileOpenerAndWriter; // this overwrites the information in the user accounts

	size_t level; // this is crucial to later determine the size of our level's grid
	size_t score; // this is being accumulated in each person's account in order to keep some sort of a highscore

	size_t currentPuzzleDimension = 0; 
	size_t maxRowBuffer, maxColBuffer; // those two variables are used for the printing method - their function is to help the program skip unnecessary empty columns

	unsigned** playerGrid = new unsigned*[currentPuzzleDimension];
	unsigned** puzzle = new unsigned*[currentPuzzleDimension]; 

	if (menuNavigator == LOGIN) {
		// this is the case where we try to login into an EXISTING account
		clearConsole();
		std::cout << "You have chosen 1: (Log in).\nPlease enter your \nUsername: ";
		bool succesfulLogin = true; // we use this bool to check if the entered name's length is valid or if it exists as an account

		do {
			username = new char[MAX_NAME_LENGTH];
			std::cin >> username;
			userFile = new char[MAX_NAME_LENGTH_WITH_EXTENSION];
			initializeFileName(username, userFile);
			fileSearcherAndReader.open(userFile);
			bool validInput = userInputDataIsValid(username);
			bool succesfulInput = !fileSearcherAndReader.fail();
			if (!validInput) {
				printIncorrectInput();
			}
			else if (!succesfulInput) {
				std::cout << "Such user does not exist! Please try again!\nUsername: ";
			}
			succesfulLogin = validInput && succesfulInput; // here we calculate the real value of this variable - it determines whether the do-while loops needs to be run over again
		} while (!succesfulLogin);

		char* passwordChecker;

		std::cout << "Password: ";

		passwordChecker = new char[MAX_NAME_LENGTH]; // this is needed in order to compare our input to the existing password
		fileSearcherAndReader.getline(passwordChecker, MAX_NAME_LENGTH); // this extracts the password from our existing file to grant access

		do {
			std::cin >> password;
			bool validInput = userInputDataIsValid(password);
			bool correctPassword = equals(password, passwordChecker);
			if (!validInput) {
				printIncorrectInput();
				continue;
			}
			else if (!correctPassword) {
				std::cout << "The password you have entered is not correct! Please try again \nPassword: ";
			}

			succesfulLogin = validInput && correctPassword;

		} while (!succesfulLogin);

		delete[] passwordChecker;

		char pickAnOperation;
		clearConsole();
		std::cout << "Login succesful! Choose an action: \n1. Start from where I've last left \n2. Start the puzzle from the beginning \n3. Start a new game (delete my score and progress) \nI choose: ";

		//these constants are used to make it easier to navigate through the code
		const char CONTINUE = '1';
		const char START_OVER_LEVEL = '2';
		const char START_ALL_OVER = '3';

		do {
			std::cin >> pickAnOperation;
			if (pickAnOperation != CONTINUE && pickAnOperation != START_OVER_LEVEL && pickAnOperation != START_ALL_OVER) printIncorrectInput();
		} while (!(pickAnOperation == CONTINUE || pickAnOperation == START_OVER_LEVEL || pickAnOperation == START_ALL_OVER));

		//this determines what level we will choose
		if (pickAnOperation != START_ALL_OVER) {
			fileSearcherAndReader >> level;
		}
		else {
			level = 1; // if we start all over, the level is set back to 1, and instead - it remains as it is
			fileSearcherAndReader.ignore(256, '\n');
		}

		//this determines what is our score
		if (pickAnOperation == CONTINUE) {
			fileSearcherAndReader >> score;
		}
		else if (pickAnOperation == START_OVER_LEVEL) {
			fileSearcherAndReader >> score;
			fileSearcherAndReader.ignore(256, '\n'); // in all honesty i have no idea why the file reader acted like a little bitch there but when it jumps 2 rows it works somehow
		}
		else {
			score = 0;
			fileSearcherAndReader.ignore(256, '\n');
		}

		currentPuzzleDimension = level + 4;

		puzzleName = new char[PUZZLE_NAME_LENGTH];
		//if we choose 3 - we have to pick a new puzzle and if we dont - we pick the same puzzle
		if (pickAnOperation == START_ALL_OVER) {
			pickAPuzzle(level, puzzleName);
			fileSearcherAndReader.ignore(256, '\n');
		}
		else fileSearcherAndReader >> puzzleName;

		size_t puzzleStringLength = (currentPuzzleDimension * currentPuzzleDimension);

		playerGrid = new unsigned* [currentPuzzleDimension];
		expandPointerSpace(playerGrid, currentPuzzleDimension, currentPuzzleDimension);
		initArrayWithZeroes(playerGrid, currentPuzzleDimension, currentPuzzleDimension);

		// This is the main difference between choice 1 and choice 2 - when we pick 1 we scan our file for the puzzle, but in 2 we just annull it
		if (pickAnOperation == CONTINUE) {
			char* playerProgressString = new char[puzzleStringLength + 1];
			fileSearcherAndReader >> playerProgressString;
			convertAStringIntoAPuzzleGrid(playerProgressString, playerGrid, currentPuzzleDimension);
			delete[] playerProgressString;
		}

		fileSearcherAndReader.close();
	}
	else {
		//this is the case where we CREATE a NEW account
		clearConsole();

		std::cout << "You have chosen 2: (Create a new account). \nPlease create your \nUsername: ";

		bool validInput = true;

		do {
			username = new char[MAX_NAME_LENGTH];
			validateDataInput(username, validInput);

			userFile = new char[MAX_NAME_LENGTH_WITH_EXTENSION];
			initializeFileName(username, userFile);

			fileSearcherAndReader.open(userFile);
			if (!fileSearcherAndReader.fail()) {
				std::cout << "That user already exists! \nPlease try another name: ";
			}
		} while (!fileSearcherAndReader.fail()); // This ensures that we will create a new UNIQUE account by checking for already existing names

		fileSearcherAndReader.close();

		std::cout << "Password: ";

		validateDataInput(password, validInput);

		level = 1;
		score = 0;
		currentPuzzleDimension = level + 4;

		puzzleName = new char[PUZZLE_NAME_LENGTH];
		playerGrid = new unsigned* [currentPuzzleDimension];

		pickAPuzzle(level, puzzleName);

		expandPointerSpace(playerGrid, currentPuzzleDimension, currentPuzzleDimension);
		initArrayWithZeroes(playerGrid, currentPuzzleDimension, currentPuzzleDimension);
	}

	char roundPicker;

	do {
		unsigned** puzzle = new unsigned* [currentPuzzleDimension];
		unsigned** rowClues = new unsigned* [currentPuzzleDimension];
		unsigned** colClues = new unsigned* [currentPuzzleDimension];

		readAndInitializePuzzleFromFile(puzzleName, level, puzzle, rowClues, colClues, maxRowBuffer, maxColBuffer);

		size_t numberOfAttempts = (7 - level) * 2; // this will determine how many tries we can have at each of the 5 levels

		clearConsole();
		printLevelScoreAndAttempts(level, score, numberOfAttempts);
		printOutTable(playerGrid, rowClues, colClues, currentPuzzleDimension, maxRowBuffer, maxColBuffer);

		do {
			char option;
			bool validInput = true;

			do {
				printPlayingOptions();
				std::cin >> option;
				if (option != 'e' && option != '/' && option != '#') validInput = false;
				else validInput = true;
			} while (!validInput);

			if (option == 'e') {
				saveUserInfo(fileOpenerAndWriter, userFile, password, level, score, puzzleName, playerGrid, currentPuzzleDimension);
				std::cout << "Progress saved. Thank you for playing!\n";
				return 0; // this is the case where we terminate our game and save our progress
			}
			else 
				{
				char xClue, yClue;
				size_t x, y;
				std::cout << "Please enter the coordinates that you want to place your clue on (in this order: (row, column)): ";
				do {
					validInput = true;
					std::cin >> xClue;
					std::cin >> yClue;
					x = xClue - '0';
					y = yClue - '0';
					if (x < 1 || y < 1 || x > currentPuzzleDimension || y > currentPuzzleDimension) {
						std::cout << "The position you've entered is not valid! please try again!";
						validInput = false;
					}
				} while (!validInput);

				if (option == '#') enterAClue(playerGrid, x - 1, y - 1);
				else enterANonClue(playerGrid, x - 1, y - 1);

				clearConsole();
				printLevelScoreAndAttempts(level, score, numberOfAttempts);
				printOutTable(playerGrid, rowClues, colClues, currentPuzzleDimension, maxRowBuffer, maxColBuffer);

				if (puzzle[x - 1][y - 1] == 0 && playerGrid[x - 1][y - 1] == 1) {
					// if we've put a clue on an actually empty space we get sanctioned
					std::cout << "Your clue was wrong! Please be more careful next time! " << std::endl;
					numberOfAttempts--;
					if (numberOfAttempts == 0) {
						std::cout << "You've ran out of attempts! Sorry about that. See you next time! " << std::endl;
						initArrayWithZeroes(playerGrid, currentPuzzleDimension, currentPuzzleDimension); // our puzzle gets reset back to it's starting point
						saveUserInfo(fileOpenerAndWriter, userFile, password, level, score, puzzleName, playerGrid, currentPuzzleDimension);
						closeMatricesAfterTheRoundEnds(puzzle, playerGrid, rowClues, colClues, currentPuzzleDimension);
						return 0;
					}
					else std::cout << "Your clue was wrong! Please be more careful next time! " << std::endl;
				}
			}
		} while (!theGridsMatch(playerGrid, puzzle, currentPuzzleDimension));

		//this if-else block checks the status of our game after each round is finished - when it becomes 5 it exits the game
		if (level == 5) { // here we terminate our player's game and reset their progress
			level = 1;
			score = 0;
			pickAPuzzle(level, puzzleName);
			std::cout << "You've finished the last level of the game! Thank you for playing and hope that you have enjoyed your time";
			saveUserInfo(fileOpenerAndWriter, userFile, password, level, score, puzzleName, playerGrid, currentPuzzleDimension);
			closeMatricesAfterTheRoundEnds(puzzle, playerGrid, rowClues, colClues, currentPuzzleDimension);
			return 0;
		}
		else {
			level++;
			currentPuzzleDimension++;
			pickAPuzzle(level, puzzleName);
			playerGrid = new unsigned* [currentPuzzleDimension];
			expandPointerSpace(playerGrid, currentPuzzleDimension, currentPuzzleDimension);
			initArrayWithZeroes(playerGrid, currentPuzzleDimension, currentPuzzleDimension);
		}

		std::cout << "You've guessed the puzzle! Would you want to continue (c) or to stop your game (e)?: ";

		do {
			std::cin >> roundPicker;
			if (roundPicker != 'e' && roundPicker != 'c') std::cout << "Incorrect input! Please try again and enter ONLY 'e' or 'c': ";
		} while (roundPicker != 'e' && roundPicker != 'c');

		if (roundPicker == 'e') {
			saveUserInfo(fileOpenerAndWriter, userFile, password, level, score, puzzleName, playerGrid, currentPuzzleDimension);
			closeMatricesAfterTheRoundEnds(puzzle, playerGrid, rowClues, colClues, currentPuzzleDimension);
			return 0;
		}

		score += 50; // immediately after we guess the puzzle we are gifted 50 points

		closeMatricesAfterTheRoundEnds(puzzle, playerGrid, rowClues, colClues, currentPuzzleDimension);

	} while (roundPicker == 'c');

	saveUserInfo(fileOpenerAndWriter, userFile, password, level, score, puzzleName, playerGrid, currentPuzzleDimension);

	return 0;
}
