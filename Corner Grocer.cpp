#define NOMINMAX         //This #define removes the Windows.h definition of max for proper use in line 39
#include <Python.h>      //Included headers are for Python objects/functionality, input/output, Windows library, and utilizing thread sleep
#include <iostream>
#include <Windows.h>
#include <thread>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <iterator>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;     //Utilizing standard namespace

void ReadItemList(vector<string>& itemTempList) {     //Function to open and take strings from the input file and set to a vector
	ifstream inFS;                                             
	string currItem;                                  //Variable to hold the string from the file         

	inFS.open("CS210_Project_Three_Input_File.txt");             //opened the .txt file to read.

	if (!inFS.is_open()) {                                                           //if statement to test if the file opened properly, if not output error statement.
		cout << "Could not open file CS210_Project_Three_Input_File.txt." << endl;  
	}

	while (!inFS.eof()) {       //while statement continues looping until entire .txt file has been read.
		inFS >> currItem;       //inFS takes each string and assigns it to the temp variable.

		if (!inFS.fail()) {                          //as long as input stream has not failed, it will take the temp variables
			itemTempList.push_back(currItem);        //and add them to the vector
		}
	}

	inFS.close();      //closes the  open .txt file

	return;
}

void DisplayMenu() {     //Outputs menu for user to see options
	system("CLS");
	cout << "***********************************************" << endl;
	cout << "1: Display Items Sold/Count" << endl;
	cout << "2: Display Count For Specific Item" << endl;
	cout << "3: Create Histogram of Item Sales/Count" << endl;
	cout << "4: Exit" << endl;
	cout << "Enter your selection as a number 1, 2, 3, or 4." << endl;
	cout << "***********************************************" << endl;
}

void MenuSelect(int& menuNum) {      //Takes user input and validates as either 1, 2, 3, or 4 for CallProcedure
	DisplayMenu();

	menuNum = 0;      //Reset menuNum so user can continue to choose between 1, 2, 3, until 4 is pressed

	try {
		cin >> menuNum;     //Input desired function number

		if (menuNum < 1 || menuNum > 4 || menuNum == NULL) {     //Throws if menuNum is anything but 1, 2, 3, or 4
			throw runtime_error("Invalid input \n");
		}
	}
	catch (runtime_error excpt) {      //Catch for a non 1, 2, 3, or 4 input
		system("CLS");                 //Clear screen

		cout << excpt.what() << endl;  //Output invalid input to user

		std::this_thread::sleep_for(3000ms);   //Wait 3 seconds so user can see error message

		cin.clear();                                          //Clear cin buffer
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	return;
}

void CallProcedure(int& menuNum, vector<string> &itemTempList) {       //Takes the validated 1, 2, 3, or 4 menuNum value and performs the selected task based on input integer 
	string contChar;        //Variable to "press to continue"

	system("CLS");          //Clear screen

	try {                                                                 
		if (menuNum == 1) {
			Py_Initialize();              //Initialize the Python Interpreter

			PyObject* listObj = PyTuple_New(itemTempList.size());           //PyTuple declared with length of vector

			for (size_t i = 0; i < itemTempList.size(); i++) {              //For loop to take each string in vector, convert it to char*, then PyString, then append to PyTuple
				char* procname = new char[itemTempList[i].length() + 1];
				std::strcpy(procname, itemTempList[i].c_str());

				PyObject* val = Py_BuildValue("(z)", procname);

				if (!val) {
					Py_DECREF(listObj);
				}
				PyTuple_SET_ITEM(listObj, i, val);
			}

			PyObject* pName, * pModule, * pDict, * pFunc = nullptr, * pValue = nullptr, * presult = nullptr;

			pName = PyUnicode_FromString((char*)"List Functions");        // Build the name object

			pModule = PyImport_Import(pName);         // Load the module object

			pDict = PyModule_GetDict(pModule);        // pDict is a borrowed reference

			pFunc = PyDict_GetItemString(pDict, "ItemCounter");          //Sets pFunc to appropriate Function name

			if (PyCallable_Check(pFunc)) {                       //If function can be called, call function with vector as argument
				presult = PyObject_CallObject(pFunc, listObj);
			}

			std::this_thread::sleep_for(3000ms);        //Wait for 3 seconds so user can see the output

			cout << endl << "Press any key to continue . . .";

			cin >> contChar;        //Press a key to continue

		}
		if (menuNum == 2) {
			string userWord;     //Variable for user input

			cout << "*******************************************" << endl;
			cout << "* What Item Would You Like To Search For? *" << endl;
			cout << "*******************************************" << endl;
			cout << "*****************************************************************" << endl;
			cout << "* Please Captitalize the First Letter and Make the Word Plural! *" << endl;
			cout << "*****************************************************************" << endl;

			cin >> userWord;     //user input to userWord

			int t = count(itemTempList.begin(), itemTempList.end(), userWord);      //search vector for userWord

			if (t == 0) {                //If statement to throw if userWord is not in the vector
				throw runtime_error("This Item is not in the list! \n");
			}

			itemTempList.push_back(userWord);      //Append userWord to vector

			Py_Initialize();        //Initialize the Python Interpreter

			PyObject* listObj = PyTuple_New(itemTempList.size());     //PyTuple declared with length of appended vector

			for (size_t i = 0; i < itemTempList.size(); i++) {              //For loop to take each string in vector, convert it to char*, then PyString, then append to PyTuple
				char* procname = new char[itemTempList[i].length() + 1];
				std::strcpy(procname, itemTempList[i].c_str());

				PyObject* val = Py_BuildValue("(z)", procname);

				if (!val) {
					Py_DECREF(listObj);
				}
				PyTuple_SET_ITEM(listObj, i, val);
			}

			PyObject* pName, * pModule, * pDict, * pFunc = nullptr, * pValue = nullptr, * presult = nullptr;

			pName = PyUnicode_FromString((char*)"List Functions");        // Build the name object

			pModule = PyImport_Import(pName);         // Load the module object

			pDict = PyModule_GetDict(pModule);        // pDict is a borrowed reference

			pFunc = PyDict_GetItemString(pDict, "ItemFinder");       //Sets pFunc to appropriate Function name

			if (PyCallable_Check(pFunc)) {
				PyObject* presult = PyObject_CallObject(pFunc, listObj);       //Call Python function and set the returned value to PyObject

				string pyResult = std::string(PyUnicode_AsUTF8(presult));      //Convert returned Item count from Python to value usable in C++

				cout << endl << userWord << " were bought " << pyResult << " times today." << endl;     //Output user input word with returned count value
			}
			
			std::this_thread::sleep_for(3000ms);       //Wait for 3 seconds so user can see the output

			cout << endl << "Press any key to continue . . .";

			cin >> contChar;      //Press a key to continue
		}

		if (menuNum == 3) {
			ifstream inFS;        //Create variable to handle input stream
			string currItem;      //Variables for item/count
			int currNumber;

			PyObject* pName, * pModule, * pDict, * pFunc = nullptr, * pValue = nullptr, * presult = nullptr;

			Py_Initialize();           // Initialize the Python Interpreter

			pName = PyUnicode_FromString((char*)"List Functions");        // Build the name object

			pModule = PyImport_Import(pName);         // Load the module object

			pDict = PyModule_GetDict(pModule);        // pDict is a borrowed reference

			pFunc = PyDict_GetItemString(pDict, "FileOutput");    //Sets pFunc to appropriate Function name

			if (PyCallable_Check(pFunc)) {                   //Checks to see if Python function is callable and then call its with no argument
				presult = PyObject_CallObject(pFunc, NULL);
			}

			inFS.open("frequency.dat");    //Open file made from Python

			while (!inFS.eof()) {      //while statement continues looping until all items and count are read
				inFS >> currItem;      //Input to variables for item/count
				inFS >> currNumber;

				if (!inFS.fail()) {                                   //As long as input stream has not failed, it will take the new values
					cout << right << setw(11) << currItem << " | ";   //Outputs a histogram with Item and count being represented as '*'s
					for (int i = 1; i <= currNumber; i++) {
						cout << "*";
					}
				}
				cout << endl;
			}

			inFS.close();             //Close file

			std::this_thread::sleep_for(3000ms);        //Wait for 3 seconds so user can see the output

			cout <<  "Press any key to continue . . .";

			cin >> contChar;               //Press a key to continue
		}
		if (menuNum == 4) {                //Displays goodbye
			cout << "Thank You, Come Again!" << endl;

			std::this_thread::sleep_for(3000ms);      		//Wait for 3 seconds so user can see the output
		}
	}
	catch (runtime_error excpt) {      //Catch for an input not in list
		system("CLS");                 //Clear screen

		cout << excpt.what() << endl;  //Output Item not in list to user

		std::this_thread::sleep_for(3000ms);   //Wait 3 seconds so user can see error message

		cin.clear();                                          //Clear cin buffer
		cin.ignore(numeric_limits<streamsize>::max(), '\n');
	}
	return;
}

int main() {
	int menuNum = 0;         //Declare and initialize a variable for referencing and use in while loop/functions
	vector<string> itemTempList;     //Declare vector for referencing

	ReadItemList(itemTempList);      ////Function to open and take strings from the input file and set to vector

	while (menuNum != 4) {        //While loop until user inputs a '4'
		MenuSelect(menuNum);      //Takes user input and validates as either 1, 2, 3, or 4 for CallProcedure

		CallProcedure(menuNum, itemTempList);   //Takes the validated 1, 2, 3, or 4 menuNum value and performs the selected task based on input 
	}
	return 0;
}