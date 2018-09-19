#include <vector>
#include <stack>
#include <queue>
#include <deque>
#include <iostream>
#include <getopt.h>
#include <sstream>
#include <list>


using namespace std;

struct State {

	char color;
	size_t x;
	size_t y;
	State();
	State(char color, size_t x, size_t y);
	~State();

};

State::State(void): color('^'), x(0), y(0){}
State::State(char C, size_t X, size_t Y): color(C), x(X), y(Y)  {}
State::~State(void){}

void printHelp(char *argv[]) {
	cout << "Usage: " << argv[0] << " [-m resize|reserve|nosize]|-h \n";
	cout << "This program determines whether a given puzzle level can be completed, \n";
	cout << "and if so, with what path. \n";
} // printHelp()

vector<string> readFlags(int argc, char * argv[]) {
	bool outputSpecified = false;
	bool flagSpecified = false;
	vector<string> output(2);
	string extoutput;

	// These are used with getopt_long()
	opterr = true; // Give us help with errors
	int choice;
	int option_index = 0;
	option long_options[] = {
		{ 
			"help", no_argument, nullptr, 'h'
		}, { 
			"queue", no_argument, nullptr, 'q' 
		}, { 
			"stack", no_argument, nullptr, 's' 
		}, { 
			"output", required_argument, nullptr, 'o' 
		}, { 
			nullptr, 0,                 nullptr, '\0' 
		}
	};
	
	while ((choice = getopt_long(argc, argv, "hqso:", long_options, &option_index)) != -1) {
		switch (choice) {
		case 'h':
			printHelp(argv);
			cout << endl;
			exit(0);

		case 'q':
			if (flagSpecified == true) { 
				cerr << "Error: Can only include stack or list, not both";
				cout << flush;
				exit(1);
			}
			output[0] = "q";
			flagSpecified = true;
			break;

		case 's':
			if (flagSpecified == true) { 
				cerr << "Error: Can only do stack or list, not both";
				cout << flush;
				exit(1);
			}
			output[0] = "s";
			flagSpecified = true;
			break;

		case 'o':
			extoutput = optarg;
			if (extoutput == "list") {
				output[1] = "list"; 
			}
			else if (extoutput == "map") {
				output[1] = "map";
			}
			if (extoutput != "map" && extoutput != "list") {
			cerr << "Error: invalid extra output";
			cout << flush;
			exit(1);
			} // if
			outputSpecified = true;
			break;

		default:
			cerr << "Error: invalid option";
			cout << flush;
			exit(1);
		} // switch
	} // while

	if (!outputSpecified) {
		output[1] = "map";
	} // if
	if (!flagSpecified) {
		cerr << "Error: no flag specified";
		cout << flush;
		exit(1);
	}

	return output;
} // readFlags()

void printMap(list<State> &statePath, vector<vector<vector<char>>> &REACHABLE_COLLECTION, vector<char> validColors, vector<string> &map) {
	size_t numColors = validColors.size();
	bool reachable = false;
	//size_t colorIndex = numColors - 1;
	if (statePath.empty()) {
		cout << "No solution.\nReachable:\n";
		for (size_t i = 0; i < map.size(); i++) {
			for (size_t j = 0; j < map[0].size(); j++) {
				for (size_t k = 0; k < numColors; k++) {
					if (REACHABLE_COLLECTION[k][j][i]) {
						reachable = true;
						break;
					}
				}
				if (!reachable) {
					map[i][j] = '#';
				}
				reachable = false;
			}
			cout << map[i] << "\n";

		}
	}
	else {
		size_t length = statePath.size();
		State prev = statePath.front();
		State current = statePath.front();
		char color = current.color;
		size_t colorIndex;
		if (color == '^') {
			colorIndex = 0;
		}
		else {
			colorIndex = color - 'a' + 1;
		}
		for (size_t i = 0; i < length; i++) {
			current = statePath.front();
			color = current.color;
			if (color == '^') {
				colorIndex = 0;
			}
			else {
				colorIndex = color - 'a' + 1;
			}
			if (i == 0) {
				REACHABLE_COLLECTION[colorIndex][current.x][current.y] = '@';
			}
			else if (i == length - 1) {
				REACHABLE_COLLECTION[colorIndex][current.x][current.y] = '?';
			}
			else if(current.color != prev.color){
				REACHABLE_COLLECTION[colorIndex][current.x][current.y] = '@';
				color = prev.color;
				if (color == '^') {
					colorIndex = 0;
				}
				else {
					colorIndex = color - 'a' + 1;
				}
				REACHABLE_COLLECTION[colorIndex][current.x][current.y] = '%';
			}
			else {
				REACHABLE_COLLECTION[colorIndex][current.x][current.y] = '+';
			}
			prev = State(current.color, current.x, current.y);
			statePath.pop_front();
		}
		for (size_t i = 0; i < REACHABLE_COLLECTION.size(); i++) {
			cout << "// color " << validColors[i] << "\n";
			for (size_t j = 0; j < REACHABLE_COLLECTION[0][0].size(); j++) {
				for (size_t k = 0; k < REACHABLE_COLLECTION[0].size(); k++) {
					char position = REACHABLE_COLLECTION[i][k][j];
					char tile = map[j][k];
					if ((map[j][k] == '@') || map[j][k] == validColors[i]){ 
						tile = '.';
					}
					else if (isupper(tile) && tolower(tile) == validColors[i]) { 
						tile = '.';
					}
					else if (tile >= 48 && tile <= 53) {
						if (isupper(map[j][k]) && tolower(map[j][k]) == validColors[i]) { 
							tile = '.';
						}
						else {
							tile = map[j][k];
						}
					}
					if (position != '@' && position != '+' && position != '%' && position != '?') {
						cout << tile;
					}
					else {
						cout << position;
					}
					
				}
				cout << "\n";
			}
		}
	}
}


void printList(list<State> statePath, State last, vector<vector<vector<char>>> &REACHABLE_COLLECTION, vector<char> validColors, vector<string> &map) {
	if (statePath.empty()) {
		last = State('^', 0, 0);
		printMap(statePath, REACHABLE_COLLECTION, validColors, map);
	}
	else {
		size_t length = statePath.size();
		string output = "";
		for (size_t i = 0; i < length; i++) {
			State current = statePath.front();
			statePath.pop_front();
			ostringstream charx;
			charx << current.x;
			ostringstream chary;
			chary << current.y;
			char color = current.color;
			output += "("s + color + ", ("s + charx.str() + ", "s + chary.str() + "))\n"s;
		}
		cout << output;
	}

}


bool addState(State next, char prev, size_t colorIndex, vector<string> &map, deque<State> &state_collection, vector<vector<vector<char>>> &REACHABLE_COLLECTION) {
	if (!REACHABLE_COLLECTION[colorIndex][next.x][next.y]) {
		if (map[next.y][next.x] != '#') {
			if (map[next.y][next.x] == '?') {
				state_collection.push_back(next);
				REACHABLE_COLLECTION[colorIndex][next.x][next.y] = prev;
				return true;
			}
			else if (map[next.y][next.x] == '@' || map[next.y][next.x] == '.' || map[next.y][next.x] == '^' || (map[next.y][next.x] - 'a' >= 0 && map[next.y][next.x] <= 122) || (map[next.y][next.x] - 'A' >= 0 && map[next.y][next.x] <= 90 && tolower(map[next.y][next.x]) == next.color)) {
				state_collection.push_back(next);
				REACHABLE_COLLECTION[colorIndex][next.x][next.y] = prev;
			}
		}
	}
	return false;
}

void checkStates(string flag, State &start, State &end, list<State> &statePath, deque<State> &state_collection, vector<vector<vector<char>>> &REACHABLE_COLLECTION, vector<string> &map) {
	REACHABLE_COLLECTION[0][start.x][start.y] = '0'; 
	state_collection[0] = start;
	bool endReached = false;
	State last;
	//vector<char> path_to_finish;
	State S;
	State Button;
	while (!state_collection.empty()) {
		if (flag == "q") {
			S = state_collection.front();
			state_collection.pop_front();
		}
		else {
			S = state_collection.back();
			state_collection.pop_back();
		}
		
		if(map[S.y][S.x] != S.color && ((map[S.y][S.x] - 'a' >= 0 && map[S.y][S.x] <= 122) || map[S.y][S.x] == '^')){
			Button = State(map[S.y][S.x], S.x, S.y);
			size_t colorIndex;
			if (map[S.y][S.x] == '^') {
				colorIndex = 0;
			}
			else {
				colorIndex = map[S.y][S.x] - 'a' + 1;
			}
			if (!REACHABLE_COLLECTION[colorIndex][Button.x][Button.y]) {
				state_collection.push_back(Button);
				if (Button.color == '^') {
					REACHABLE_COLLECTION[0][Button.x][Button.y] = S.color;
				}
				else {
					REACHABLE_COLLECTION[colorIndex][Button.x][Button.y] = S.color;
				}
			}
		}
		else {
			size_t colorIndex;
			if (S.color == '^') {
				colorIndex = 0;
			}
			else {
				colorIndex = S.color - 'a' + 1;
			}
			size_t northY = S.y - 1;
			size_t eastX = S.x + 1;
			size_t southY = S.y + 1;
			size_t westX = S.x - 1;
			char prev;
			State nextState;
			if (northY < map.size() && S.y > 0) {
				prev = '3'; //3 = south
				nextState = State(S.color, S.x, northY);
				endReached = addState(nextState, prev, colorIndex, map, state_collection, REACHABLE_COLLECTION);
				if (endReached) {
					end = nextState;
					break;
				}
			}
			if (eastX < map[0].size()) {
				prev = '4';
				nextState = State(S.color, eastX, S.y);
				endReached = addState(nextState, prev, colorIndex, map, state_collection, REACHABLE_COLLECTION);
				if (endReached) {
					end = nextState;
					break;
				}
			}
			if (southY < map.size()) {
				prev = '1';
				nextState = State(S.color, S.x, southY);
				endReached = addState(nextState, prev, colorIndex, map, state_collection, REACHABLE_COLLECTION);
				if (endReached) {
					end = nextState;
					break;
				}
			}
			if (S.x > 0) {
				prev = '2';
				nextState = State(S.color, westX, S.y);
				endReached = addState(nextState, prev, colorIndex, map, state_collection, REACHABLE_COLLECTION);
				if (endReached) {
					end = nextState;
					break;
				}
			}
		}
	}
	if (endReached) {
		statePath.push_back(end);
		char c = end.color;
		size_t colorIndex;
		if (c == '^') {
			colorIndex = 0; 
		}
		else {
			colorIndex = c - 'a' + 1;
		}
		size_t x = end.x;
		size_t y = end.y;
		while(c != '0'){
			if (REACHABLE_COLLECTION[colorIndex][x][y] == '1') {
				y--;
			}
			else if (REACHABLE_COLLECTION[colorIndex][x][y] == '2') {
				x++;
			}
			else if (REACHABLE_COLLECTION[colorIndex][x][y] == '3') {
				y++;
			}
			else if (REACHABLE_COLLECTION[colorIndex][x][y] == '4') {
				x--;
			}
			else if (REACHABLE_COLLECTION[colorIndex][x][y] == '0') {
				break;
			}
			else{
				c = REACHABLE_COLLECTION[colorIndex][x][y];
				if (c == '^') {
					colorIndex = 0;
				}
				else {
					colorIndex = c - 'a' + 1;
				}
			}
			State current = State(c, x, y);
			statePath.push_front(current);
		}
	}
}

void inputValid(size_t rows, size_t columns, size_t numColors) {
	if (rows < 1) {
		cerr << "Error: invalid number of rows (must be at least 1)";
		cout << flush;
		exit(1);
	}
	else if (columns < 1) {
		cerr << "Error: invalid number of columns (must be at least 1)";
		cout << flush;
		exit(1);
	}
	else if (numColors > 26) {
		cerr << "Error: invalid number of colors (must be at least 0 and at most 26)";
		cout << flush;
		exit(1);
	}
}


int main(int argc, char *argv[]) {

	ios_base::sync_with_stdio(false);
	vector<string> args = readFlags(argc, argv);
	string flag = args[0];
	string output = args[1];
	if (output.size() == 0) {
		cerr << "Error: invalid output";
		cout << flush;
		exit(1);
	}
	size_t rows;
	size_t columns;
	size_t numColors;
	vector<string> map;
	cin >> numColors >> columns >> rows;
	inputValid(rows, columns, numColors);
	vector<char> validColors(numColors + 1);
	validColors[0] = '^';
	for (size_t i = 0; i < numColors; i++) {
		size_t color = 97 + i;
		validColors[i + 1] = (char)color;
	}
	numColors = validColors.size();
	string row;
	getline(cin, row);
	size_t playerCount = 0;
	size_t endCount = 0;
	State start;
	size_t i = 0;
	while (getline(cin, row) && row != "") {
		if (row[0] == '/' && row[1] == '/') {
			continue;
		}
		for (size_t j = 0; j < row.size(); j++) {
			if (playerCount > 1 || endCount > 1) {
				if (playerCount > 1) {
					cerr << "Error: player should only occur once";
				}
				if (endCount > 1) {
					cerr << "Error: ending should only occur once";
				}
				map.clear();
				cout << flush;
				exit(1);

			}
			else if (row[j] == '@') { 
				start.color = '^';
				start.x = j;
				start.y = i;
				playerCount++;
			}
			else if (row[j] == '?') {
				endCount++;
			}
			else {
				unsigned int colorIndex = tolower(row[j]) - 'a';
				if (row[j] != '.' && row[j] != '#' && row[j] != '^' && ((tolower(row[j]) - 'a' < 0 || tolower(row[j]) > 122) || colorIndex > numColors)) {
					cerr << "Error: invalid character\n";
					map.clear();
					cout << flush;
					exit(1);
				}
			}
		}
		i++;
		if (row.size() == columns) {
			map.push_back(row);
		}
		else {
			cerr << "Error: incorrect row length \n";
			exit(1);
		}
		
	}
	//size_t maxStates = rows * columns * numColors;
	vector<vector<vector<char>>> REACHABLE_COLLECTION(numColors, vector<vector<char>>(columns, vector<char>(rows)));
	deque<State> state_collection(1);
	State end;
	list<State> statePath;
	checkStates(flag, start, end, statePath, state_collection, REACHABLE_COLLECTION, map);
	if (output == "list") {
		printList(statePath, end, REACHABLE_COLLECTION, validColors, map);
	}
	else if (output == "map") {
		printMap(statePath, REACHABLE_COLLECTION, validColors, map);
	}
	else {
		cerr << "Error: invalid output";
		cout << flush;
		exit(1);
	}
	
	state_collection.clear();
	REACHABLE_COLLECTION.clear();
	validColors.clear();
	map.clear();
	cout << flush;
	
	return 0;
}