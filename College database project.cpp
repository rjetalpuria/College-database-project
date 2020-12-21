#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <iomanip>
#include "sqlite3.h"
using namespace std;

sqlite3* openDB(string uName);
void add(sqlite3* db);
void view(sqlite3* db);
void remove(sqlite3* db);
void update(sqlite3* db);
int print_callback(void* unused, int numCols, char** data, char** colNames);
int view_callback(void* unused, int numCols, char** data, char** colNames);
int add_callback(void* db, int numCols, char** data, char** colNames);
int remove_callback(void* db, int numCols, char** data, char** colNames);
int update_callback(void* db, int numCols, char** data, char** colNames);


struct Database {
    sqlite3* db = nullptr;
    string table;
};

int main()
{
    cout << "WELCOME TO THE UNIVERSITY DATABASE SYSTEM:" << endl;
    cout << "ENTER THE NAME OF THE UNIVERSITY:";
    string uName;
    getline(cin, uName);
    uName.append(".db");
    //create/open that university's database
    sqlite3* db = openDB(uName);
    bool repeat = true;
    while (repeat) {
        cout << "0. Quit" << endl;
        cout << "1. Add to a table" << endl;
        cout << "2. View a table" << endl;
        cout << "3. Remove from a table" << endl;
        cout << "4. Update info of table" << endl;
        cout << ">>> ";
        int input;
        cin >> input;
        cin.ignore();
        switch (input) {
        case 0:
            repeat = false;
            break;
        case 1:
            add(db);
            break;
        case 2:
            view(db);
            break;
        case 3:
            remove(db);
            break;
        case 4:
            update(db);
            break;
        } //switch
    }//while
}//main

sqlite3* openDB(string uName)
{
    sqlite3* db;
    int exit;
    exit = sqlite3_open(uName.c_str(), &db);
    if (exit) {
        cerr << sqlite3_errmsg(db) << endl;
    }
    else {
        cout << "Successfully opened/created database!" << endl;
        string table1 = "CREATE TABLE Courses (Name TEXT, Instructor TEXT, Time TEXT, Location TEXT);";
        string table2 = "CREATE TABLE Students (ID INTEGER PRIMARY KEY, Name TEXT, Major TEXT, Units_Completed INTERGER);";
        string table3 = "CREATE TABLE Instructors (Name TEXT, Department TEXT, Years_of_experience INTEGER, Pay INTEGER);";
        sqlite3_exec(db, table1.c_str(), NULL, NULL, NULL);
        sqlite3_exec(db, table2.c_str(), NULL, NULL, NULL);
        sqlite3_exec(db, table3.c_str(), NULL, NULL, NULL);
    }
    cout << endl;
    return db;
}

void add(sqlite3* db)
{
    cout << "Initaiting process..." << endl;
    cout << endl;
    cout << "Tables:" << endl;
    string sql = "SELECT Name FROM sqlite_master WHERE TYPE = 'table';";
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    cout << endl;
    cout << "Enter name of table to add to: ";
    string table;
    getline(cin, table);
    cout << endl;

    Database myDB;
    myDB.db = db;
    myDB.table = table;

    sql = "SELECT sql FROM sqlite_master WHERE TYPE = 'table' AND Name = '" + table + "';";
    sqlite3_exec(db, sql.c_str(), add_callback, &myDB, NULL);
    return;
}

void view(sqlite3* db) {
    cout << "Initaiting process..." << endl;
    cout << endl;
    cout << "Tables:" << endl;
    string sql = "SELECT Name FROM sqlite_master WHERE TYPE = 'table';";
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    cout << endl;
    cout << "Enter name of table to view: ";
    string table;
    getline(cin, table);
    cout << endl;
    while (table.back() == ' ')
        table.pop_back();
    sql = "SELECT sql FROM sqlite_master WHERE TYPE = 'table' AND Name = '" + table + "';";
    sqlite3_exec(db, sql.c_str(), view_callback, NULL, NULL);

    sql = "SELECT * FROM " + table;
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    cout << endl;
}

void remove(sqlite3* db)
{
    cout << "Initaiting process..." << endl;
    cout << endl;
    cout << "Tables:" << endl;
    string sql = "SELECT Name FROM sqlite_master WHERE TYPE = 'table';";
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    cout << endl;
    cout << "Enter name of table to remove from: ";
    string table;
    getline(cin, table);
    cout << endl;
    //string sql = "DELETE FROM Courses WHERE Name =  '" + cName + "';";

    Database myDB;
    myDB.db = db;
    myDB.table = table;
 
    sql = "SELECT sql FROM sqlite_master WHERE TYPE = 'table' AND Name = '" + table + "';";
    sqlite3_exec(db, sql.c_str(), remove_callback, &myDB, NULL);
  
    
    return;
}

void update(sqlite3* db)
{
    cout << "Initaiting process..." << endl;
    cout << endl;
    cout << "Tables:" << endl;
    string sql = "SELECT Name FROM sqlite_master WHERE TYPE = 'table';";
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    cout << endl;
    cout << "Enter name of table to update: ";
    string table;
    getline(cin, table);
    cout << endl;

    //sql = "UPDATE table_name SET some_column = some_value WHERE some_column = some_value;"

    Database myDB;
    myDB.db = db;
    myDB.table = table;
    sql = "SELECT * FROM " + table;
    sqlite3_exec(db, sql.c_str(), print_callback, NULL, NULL);
    sql = "SELECT sql FROM sqlite_master WHERE TYPE = 'table' AND Name = '" + table + "';";
    sqlite3_exec(db, sql.c_str(), update_callback, &myDB, NULL);
}

int print_callback(void* unused, int numCols, char** data, char** colNames)
{   
    string colNam = *colNames;
    for (int i = 0; i < numCols; ++i) {
        cout << data[i];
        if (numCols > 1 && i < numCols-1)
            cout << " | ";
    }
    cout << endl;
    return 0;
}

int view_callback(void* unused, int numCols, char** data, char** colNames)
{
    string dataStr = *data;
    dataStr = dataStr.substr(dataStr.find('(') + 1); // "Name TEXT, ID INTEGER)"
    dataStr.pop_back();
    dataStr.push_back(','); // "Name TEXT, ID INTEGER,"

    while (!dataStr.empty()) {
        string temp;
        stringstream(dataStr) >> temp; // "Name"
        cout << temp;
        dataStr = dataStr.substr(dataStr.find(',') + 1);
        if(!dataStr.empty())
            cout << " | ";
    }
    cout << endl;
    return 0;
}

int add_callback(void* db, int numCols, char** data, char** colNames)
{
 
    vector<string> inputName;
    vector<string> inputType;
    vector<string> inputValue;
    int index = 0;
    string dataStr = *data;
    dataStr = dataStr.substr(dataStr.find('(') + 1); // "Name TEXT, ID INTEGER)"
    dataStr.pop_back();
    dataStr.push_back(','); // "Name TEXT, ID INTEGER,"

    while (!dataStr.empty()) {
        if (dataStr[0] == ' ') {
            dataStr = dataStr.substr(dataStr.find(' ') + 1);
        }
        string temp;
        stringstream(dataStr) >> temp; // "Name"
        inputName.push_back(temp);
        dataStr = dataStr.substr(dataStr.find(' ') + 1); // "TEXT, ID INTEGER,"
        stringstream(dataStr) >> temp;
        if(temp.back() == ',')
            temp.pop_back(); //removes ','
        inputType.push_back(temp);
        
        cout << "Enter " << inputName[index] << " of type " << inputType[index] << ": ";
        getline(cin, temp);
        inputValue.push_back(temp);

        dataStr = dataStr.substr(dataStr.find(',') + 1);
        index++;
    }

    string colName = "(";
    for (string s : inputName) {
        colName.append(s);
        colName.push_back(',');
    }
    colName.pop_back();
    colName.push_back(')');

    string colVal = "(";
    for (int i = 0; i < inputValue.size(); ++i) {
        if (inputType[i] == "TEXT") {
            colVal.push_back('\'');
        }
        colVal.append(inputValue[i]);
        if (inputType[i] == "TEXT") {
            colVal.push_back('\'');
        }
        colVal.push_back(',');
    }
    colVal.pop_back();
    colVal.push_back(')');
    
    Database myDB = *(Database*)db;
    string sql = "INSERT INTO " + myDB.table + colName +  \
        " VALUES" + colVal + ";";
    sqlite3_exec(myDB.db, sql.c_str(), NULL, NULL, NULL);
    return 0;
}

int remove_callback(void* db, int numCols, char** data, char** colNames)
{
    //string sql = "DELETE FROM Courses WHERE Name =  '" + cName + "';";
    string dataStr = *data;
    dataStr = dataStr.substr(dataStr.find('(') + 1); // "Name TEXT, ID INTEGER)"
        
    string name;
    stringstream(dataStr) >> name;
    dataStr = dataStr.substr(dataStr.find(name) + name.length() + 1);
    string type;
    stringstream(dataStr) >> type;
    type.pop_back();
    cout << "Enter the " << name << " to delete: ";
    string value;
    getline(cin, value);
    
    if (type == "TEXT") {
        string temp = "'";
        temp.append(value);
        temp.push_back('\'');
        value = temp;
    }
    Database myDB = *(Database*)db;
    string sql = "DELETE FROM " + myDB.table + " WHERE " + name + "= " + value + ";";
    sqlite3_exec(myDB.db, sql.c_str(), NULL, NULL, NULL);

    return 0;
}

int update_callback(void* db, int numCols, char** data, char** colNames)
{
    string dataStr = *data;
    dataStr = dataStr.substr(dataStr.find('(') + 1); // "Name TEXT, ID INTEGER)"
    
    dataStr.pop_back();
    dataStr.push_back(',');
    string key, keyVal, keyType;
    stringstream(dataStr) >> key;
    
    cout << "Enter name of column to update: ";
    string column, colType;
    cin >> column;

    while (!dataStr.empty()) {
        string temp;
        stringstream(dataStr) >> temp;

        if (temp == key) {
            dataStr = dataStr.substr(dataStr.find(key) + key.length()+1);
            stringstream(dataStr) >> keyType;
            if(keyType.back() == ',')
                keyType.pop_back();
        }

        if (temp == column) {
            dataStr = dataStr.substr(dataStr.find(column) + column.length() + 1);
            stringstream(dataStr) >> colType;
            colType.pop_back();
        }

        dataStr = dataStr.substr(dataStr.find(',') + 1);
        
    }
    cout << "Enter a new value of type " << colType << ": ";
    string newVal;
    cin.ignore();
    getline(cin, newVal);
    cout << "Enter the " << key << " of the row to update: ";
    getline(cin, keyVal);

    if (colType == "TEXT") {
        string temp = "'";
        temp.append(newVal);
        temp.push_back('\'');
        newVal = temp;
    }

    if (keyType == "TEXT") {
        string temp = "'";
        temp.append(keyVal);
        temp.push_back('\'');
        keyVal = temp;
    }

    Database myDB = *(Database*)db;
    string sql = "UPDATE " + myDB.table + " SET " + column + " = " + newVal + " WHERE " + key + " = " + keyVal + ";";
    
    sqlite3_exec(myDB.db, sql.c_str(), NULL, NULL, NULL);

    return 0;
}
