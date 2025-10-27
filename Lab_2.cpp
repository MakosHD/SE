
#include <iostream>
#include <vector> // Vector is used to create arrays with data
#include <conio.h> // getch() - This function is used to obtain the key pressed by the user.
#include <functional>// This library allows to save actions in the menu structure.
#define NOMINMAX // windows.h define min and max macros which conflict with std::min and std::max
#include <windows.h> // to get size of console window
#include <limits> // numeric_limits
#include <string> // convert double into string

//https://medium.com/@vitorcosta.matias/print-coloured-texts-in-console-a0db6f589138

using namespace std;

struct menu_item {
    string name;
    function<void()> action;
};

struct menu {
    string name;
    vector<menu_item> items;
};

struct variables_for_calculations {
    int n;
    double lower_limit;
    double upper_limit;
    double step;
    vector<vector<double>> solutions;
    bool calculated = false;
    bool error = false;
    string error_message;
};

vector<variables_for_calculations> data_for_calculations; // Vector to store all sets of data for calculations

int current_index;
menu menu_current; // Current menu to display
menu menu_main;
menu menu_manage_data;
menu menu_manage_results;

//Declarations of actions to use it in menu items
variables_for_calculations action_input_data();
void action_edit_data();
void action_calculate();
void action_view_results();

void menu_initialization() {
    menu_main = { "Main menu",{
        {"Manage data", []() {menu_current = menu_manage_data; }},
        {"Calculate!", action_calculate},
        {"Manage results", []() {menu_current = menu_manage_results; current_index = 0; }}
        }
    };

    menu_manage_data = { "Data management", {
        //{"Add data from file",[]() {}},
        {"Input data from keyboard", []() {data_for_calculations.push_back(action_input_data()); }},
        {"Edit data", action_edit_data},
        { "Back to main menu",[]() {menu_current = menu_main; current_index = 0; }}
        }
    };
    menu_manage_results = { "Results management",{
        {"View results", action_view_results},
        //{"Export results", []() {}},
        {"Back to main menu",[]() {menu_current = menu_main; current_index = 0; }}
        }
    };
};

// Function to get width of console window
//https://stackoverflow.com/questions/23369503/get-size-of-terminal-window-rows-columns
int get_size_of_window() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}
// Global variable to store width of console window(to not call function several times in one period of time)
int columns;

// Function to draw separator line
void draw_separator() {
    for (int i = 0; i < columns; i++) {
        cout << "-";
    }
}

// Function to draw header with menu name
void draw_header(string name) {
    columns = get_size_of_window();
    string text_to_write = "Dragi menu v.0.1 || " + name;
    int len = text_to_write.length();
    int pos = (columns - len) / 2;
    for (int i = 0; i < pos; i++) {
        cout << " ";
    }
    //cout << text_to_write << endl;
    cout << "\033[104;15m" << text_to_write << "\033[m" << endl;
    draw_separator();
    cout << endl;
}
// Overloaded function to draw header with current menu name
void draw_header() {
    draw_header(menu_current.name);
};

// Function to draw message with name and text, and optional ascii colors
void draw_message(string name, string text, string ascii = "") {
    cout << "\n" << ascii << name << endl;
    columns = get_size_of_window();
    draw_separator();
    cout << "\033[m" << text << ascii << endl;
    draw_separator();
    cout << "\033[m\n" << endl;
}

// Overloaded functions to draw error, hint and warning messages
void draw_error(string text) {
    draw_message("ERROR!", text, "\033[41m");
}
void draw_hint(string text) {
    draw_message("\nHint:", text);
}
void draw_warning(string text) {
    draw_message("WARNING!", text, "\033[43m");
}


// Function to draw current menu with items
void draw_menu() {
    int size_of_menu;
	cout << "\033[2J\033[H"; // Clear console
    draw_header();
    size_of_menu = menu_current.items.size();
	if (current_index == -1) { // Wrap around to the last item
        current_index = size_of_menu - 1;
    }
	else if (current_index == size_of_menu) { // Wrap around to the first item
        current_index = 0;
    }
	for (int i = 0; i < size_of_menu; i++) { // Display menu items
		if (i == current_index) { // Highlight the current item
            cout << "\033[107;30m>" << menu_current.items[i].name << "!<\033[m" << endl;
        }
		else { // Normal display for other items
            cout << menu_current.items[i].name << endl;
        }
    };
}

struct key_action { // Structure to map key's code to actions
    int key;
    function<void()> action;
};

//27 - esc
//13 - enter
//8 - backspace
// Function to get user input, change index of current item in menu or execute corresponding action
// if user preses wrong key, function will wait until user press correct key
void get_input(vector<key_action> key_actions) {
    while (true) {
		int ch = _getch(); //get code of pressed key
        if (ch == 224) {
            ch = _getch();
            if (ch == 72) { // Up
                current_index--;
                return;
            }
            else if (ch == 80) { // Down
                current_index++;
                return;
            }
        }
		for (key_action k : key_actions) { // Check if pressed key corresponds to any action
            if (k.key == ch) {
				k.action(); // Execute corresponding action
                return;
            }
        }
    }
}


int main() {
	menu_initialization(); // Initialize menus
	current_index = 0; //set current index of menu item
	menu_current = menu_main; // Set current menu to main menu
    while (true)
    {
		draw_menu(); // Draw current menu
		// draw hints to user --------------------------------------------
		if (data_for_calculations.size() == 0) { // show hint to the user if there's no data for calculations
            draw_hint("To start, please input data in 'Manage data'->'Input data from keyboard'");
        }
		else { // show another hints to the user if there's data for calculations
            bool is_there_any_solution = false; // Check if there are any results of calculations
            for (auto& set : data_for_calculations) 
                if (!set.solutions.empty() || set.error) {
                    is_there_any_solution = true;
                    break;
                }
			if (is_there_any_solution && (menu_current.name == "Main menu" || menu_current.name == "Manage results")) {// show hint how to calculate 
                draw_hint(std::string("The calculations have results! To view them go to ") + ((menu_current.name == "Main menu") ? "'Manage results'->'View reults'!" : "'View results'!"));
            }
			if (!is_there_any_solution && data_for_calculations[0].solutions.size() == 0 && (menu_current.name == "Main menu" || menu_current.name == "Data management")) { // show hint how to view results
                draw_hint(std::string("You have entered data for calculations! To perform calculations, go to ") + ((menu_current.name == "Main menu") ? "'Calculate'!" : " 'Main menu' -> 'Calculate'!"));
            }
        }
		// -----------------------------------------------------------------
		// Wait for user input and define actions for keys
        get_input(vector<key_action> {
            {13, []() {menu_current.items[current_index].action(); draw_menu(); }},
            { 27, []() {menu_current = menu_main; current_index = 0; } }
        });
    }
    return 0; //end of the program, If this ever happens
}

// Function to input data for calculations from keyboard
variables_for_calculations action_input_data() {
	variables_for_calculations new_data; //temporary variable to store new set of data
	cout << "\033[2J\033[H"; // Clear console
	draw_header("Input data"); // this function doen't have menu, so we draw header with name of action
    draw_message("Pay attention", "Hey, This system of equations has two intervals for n:\nn > 7 for the first expression and\nn > 0 for the second.\nConsider it!");
    cout << "Enter n(n>0): ";
	while (!(cin >> new_data.n) || new_data.n <= 0) { // Validate input for n
		cin.clear();// Clear error flag on cin
		cin.ignore(std::numeric_limits<streamsize>::max(), '\n');// Discard invalid input
		draw_error("Invalid input. Please enter n that greater than 0.");// Show error message
		cout << "Enter n(n>0): ";// Prompt user to enter n again
    }
	//next inputs are validated in similar way
    cout << "Enter lower limit: ";
    while (!(cin >> new_data.lower_limit)) {
        cin.clear();
        cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
        draw_error("Invalid input. Please enter number.");
        cout << "Enter lower limit: ";
    }

    cout << "Enter upper limit: ";
    while (!(cin >> new_data.upper_limit) || new_data.upper_limit <= new_data.lower_limit) {
        cin.clear();
        cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
        draw_error("Invalid input. Please enter upper limit that greater than lower limit (" + std::to_string(new_data.lower_limit) + ").");
        cout << "Enter upper limit: ";
    }
    cout << "Enter step: ";
    while (!(cin >> new_data.step) || new_data.step <= 0) {
        cin.clear();
        cin.ignore(std::numeric_limits<streamsize>::max(), '\n');
        draw_error("Invalid input. Please enter step that greater than 0.");
        cout << "Enter step: ";
    }
    return new_data;
}
// Function to edit existing data for calculations
void action_edit_data() {
	current_index = 0; //set current index to the first item in the menu
	bool exit_edit = false; // this is the ony way that came to my mind to exit from while loop
	while (!exit_edit) {//main loop of this function
		cout << "\033[2J\033[H";//clear console
        draw_header("Edit data");// this function doen't have menu, so we draw header with name of action
        if (data_for_calculations.size() == 0) { //check if there's any data to edit, if not - show message and exit
            cout << "There's no data here to edit yet" << endl;
            system("pause");
            return;
        }
		//here we don't use draw_menu() function, because we need to display data sets with their parameters(draw_menu function doesn't fit here)
        for (int index_of_calculation = 0; index_of_calculation < data_for_calculations.size(); index_of_calculation++) {
            if (index_of_calculation == current_index) {
				cout << "\033[107;30m"; //start to highlight current item
            }
            cout << "\tData set " << index_of_calculation + 1 << endl;
            cout << "\tn = " << data_for_calculations[index_of_calculation].n << endl;
            cout << "\tlower limit = " << data_for_calculations[index_of_calculation].lower_limit << endl;
            cout << "\tupper limit = " << data_for_calculations[index_of_calculation].upper_limit << endl;
			cout << "\tstep = " << data_for_calculations[index_of_calculation].step << "\033[m" << endl;//end of highlighting and information about current item
            draw_separator();
            cout << endl;
        }
		if (current_index == data_for_calculations.size()) { cout << "\033[107;30m"; };//highlight "Back to main menu" item
        cout << "Back to main menu\n\033[m";
        draw_hint("Use Up/Down arrows to navigate, 'Enter' to edit,'Backspace' to delete");
        get_input(vector<key_action> {//wait for user input and define actions for keys
            //https://stackoverflow.com/questions/875103/how-do-i-erase-an-element-from-stdvector-by-index
			{8, []() {if (current_index != data_for_calculations.size()) { data_for_calculations.erase(data_for_calculations.begin() + current_index); }}},//backspace to delete current item
			{ 13, [&exit_edit]() {if (current_index == data_for_calculations.size()) { exit_edit = true; current_index = 0; } else { data_for_calculations[current_index] = action_input_data(); }} },//enter to edit current item or exit if "Back to main menu" is selected
			{ 27, [&exit_edit]() {exit_edit = true; current_index = 0; } }//esc to exit
        });
		// Wrap around current_index if it goes out of bounds
        if (current_index == data_for_calculations.size() + 1) {
            current_index = 0;
        }
        else if (current_index == -1) {
            current_index = data_for_calculations.size();
        }
    }
}

void action_calculate() {
	cout << "\033[2J\033[H";// Clear console
	draw_header("Calculations");// this function doen't have menu, so we draw header with name of action
	if (data_for_calculations.size() == 0) { //check if there's any data to calculate, if not - show message and exit
        cout << "There's no data for calculations yet" << endl;
        system("pause");
        return;
    }
    int n, i;
    double lower_limit, upper_limit, step, x, answer, mlt;
	// Main loop to perform calculations for each set of data
    for (int index_of_calculation = 0; index_of_calculation < data_for_calculations.size(); index_of_calculation++) {
        n = data_for_calculations[index_of_calculation].n;
        lower_limit = data_for_calculations[index_of_calculation].lower_limit;
        upper_limit = data_for_calculations[index_of_calculation].upper_limit;
        step = data_for_calculations[index_of_calculation].step;
        x = lower_limit;
        vector<vector<double>> solutions;
        data_for_calculations[index_of_calculation].error = false;
        data_for_calculations[index_of_calculation].error_message = "";
        cout << "Data set " << index_of_calculation + 1 << ":" << endl;
		// Loop to calculate answers for x from lower_limit to upper_limit with given step
        while (x <= upper_limit) {
			try { // Try-catch block to handle exceptions during calculations
                cout << "\033[102;97mCalculation for\033[m x = " << x << endl;
                if (x < 7) {
                    cout << "\033[44mx < 7\033[m" << endl;
					if (n <= 7) {//this part of exuation requires n to be greater than 7, so we check it and throw exception if it's not
                        throw invalid_argument("For x = " + std::to_string(x) + ": n(" + std::to_string(n) + ") must be greater than 7 when x < 7");
                    }
                    mlt = 1;
                    cout << "    \033[107; 30mmlt = 1\033[m" << endl;
                    for (i = 3; i <= n - 4; i++) {
                        cout << "    \033[107;30mi = " << i << "\033[m" << endl;
                        cout << "    mlt = mlt * (x / (5 + i )) -> " << mlt << " * (" << x << " / (5 + " << i << ")) = ";
                        mlt = mlt * (x / (5.0 + i));
                        cout << mlt << endl;
                    }
                    answer = mlt;
                    cout << "\033[44mAnswer for x = " << x << " is\033[m " << answer << endl;
                }
				else { // if x >= 7
					if (n <= 0) {//but this part of exuation requires n to be greater than 0, so we check it and throw exception if it's not
                        throw invalid_argument("For x = " + std::to_string(x) + ": n(" + std::to_string(n) + ") must be greater than 0 when x >= 7");
                    }
					//ви справді це все читаєте/перевіряєте? якщо так то, ура? я не знаю що тут сказати. I LIKE TOOOOOOOOOOOOOOOOOOOOOTHYYYYYYYYYYYYYYYYYYYYYYYYYYYY! Black scary night fury!
                    cout << "  \033[44mx >=7\033[m\n    \033[107;30msum = 0\033[m\n    \033[107;30mj = 0\033[m" << endl;
                    double sum = 0;
                    for (int j = 0; j <= n; j++) {
                        mlt = 1;
                        for (i = 1; i <= (n + 1); i++) {
                            cout << "      \033[107;30mmlt = " << mlt << "\033[m\n" << "      \033[107;30mi = " << i << "\033[m\n" << "      \033[107;30mj = " << j << "\033[m" << endl;
                            cout << "      mlt = mlt * ((1 / (2 * x) + (3*i/(i-j))) -> " << mlt << "*((1 / (2 * " << x << ") + (3 *" << i << "/(" << i << "-" << j << "))) = ";
                            if (i == j) {
                                cout << "\033[43mSKIP: i = j, division by zero prevented\033[m" << endl;
                                continue;
                            }
                            mlt = mlt * ((1.0 / (2.0 * x)) + (3.0 * i / (i - j)));
                            cout << mlt << endl;
                        }
                        cout << "    sum = sum + mlt -> sum = " << sum << " + " << mlt << " = ";
                        sum = sum + mlt;
                        cout << sum << endl;
                    }
                    answer = sum;
                    cout << "\033[44mAnswer for x = " << x << " is\033[m " << answer << endl;
                };
				solutions.push_back({ x, answer });// Store the result
            }
			catch (const exception& e) {// Catch standard exceptions
                data_for_calculations[index_of_calculation].error = true;
                data_for_calculations[index_of_calculation].error_message += std::string(e.what()) + "\n";
                draw_error("Error in data set " + std::to_string(index_of_calculation + 1) + ": " + e.what());
            }
			catch (...) {// Catch any other exceptions
                data_for_calculations[index_of_calculation].error = true;
                data_for_calculations[index_of_calculation].error_message += "Unknown error\n";
                draw_error("Unknown error in data set " + std::to_string(index_of_calculation + 1));
            }
			x = x + step;// Increment x by step
        }
		data_for_calculations[index_of_calculation].solutions = solutions;// Store all results in the data structure
		data_for_calculations[index_of_calculation].calculated = true;// Mark as calculated
        draw_separator();
        cout << endl;
    }

    system("pause");
}
// Function to view results of calculations
void action_view_results() {
	cout << "\033[2J\033[H";// Clear console
    draw_header("View results");
	bool is_there_any_solution = false;// Check if there's any results of calculations
    for (auto& set : data_for_calculations)
        if (!set.solutions.empty() || set.error) {
            is_there_any_solution = true;
            break;
        }
	if (!is_there_any_solution) { //if there's no results - show message and exit
        cout << "There's no results for calculations here yet" << endl;
        system("pause");
        return;
    }
    for (int index_of_calculation = 0; index_of_calculation < data_for_calculations.size(); index_of_calculation++) {
        cout << "Data set " << index_of_calculation + 1 << endl;
        cout << "n = " << data_for_calculations[index_of_calculation].n << endl;
        cout << "lower limit = " << data_for_calculations[index_of_calculation].lower_limit << endl;
        cout << "upper limit = " << data_for_calculations[index_of_calculation].upper_limit << endl;
        cout << "step = " << data_for_calculations[index_of_calculation].step << endl;
        if (data_for_calculations[index_of_calculation].error) {
            draw_warning("There were errors in this data set:\n" + data_for_calculations[index_of_calculation].error_message);
        }
        cout << "Results:" << endl;
        cout << "x\tf(x)" << endl;
        for (int i = 0; i < data_for_calculations[index_of_calculation].solutions.size(); i++) {
            cout << data_for_calculations[index_of_calculation].solutions[i][0] << "\t" << data_for_calculations[index_of_calculation].solutions[i][1] << endl;
        }
        draw_separator();
        cout << endl;
    }
    system("pause");
}
//nothing to fix