//
//  Project 1 - Gerrymandering
//  Author: Rovia Simmons
//  TODO LATER: use function decomposition, change ourvector to ourourvector
//

#include "ourvector.h"
#include <iomanip>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

using namespace std;

struct state_info{
    string name;
    ourvector<int> districts;  // 1st num, district. 2nd num, democrat. 3rd num, republican
    int voters = 0;
};

//Sorts the inputted vector using insertion sort
void sort(ourvector<state_info> &states) {
    int i,j;
    state_info pivot;

    for (i = 1; i < states.size(); i++) {
        pivot = states.at(i);
        j = i - 1;
      
        while (j >= 0 && states.at(j).name > pivot.name) {
            states.at(j + 1) = states.at(j);
            --j;
        }
        states.at(j + 1) = pivot; 
    }
}

//makes string lowercase
string lowercase(string str){
    string new_word = "";
    for(char c : str){
    new_word.push_back(tolower(c));
    }
    return new_word;
}

//Not case sensetive. Uses binary search and returns element index in ourvector. Returns -1 if not found.
int search(ourvector<state_info> &states, string search_key) {

  search_key = lowercase(search_key);
    int low = 0, high = states.size() - 1;
    int mid;
  
    while (high != low) {
        int mid = (high + low) / 2;
        if (lowercase(states.at(mid).name) < search_key) {
            low = mid + 1;
        }
        else {
            high = mid;
        }
    }
    if (lowercase(states.at(low).name) == search_key) {
      return low;
    }
    else if (lowercase(states.at(high).name) == search_key) {
      return high;
    }
  return -1;
}

//pushes data seperated by a ',' delimiter into a vector
void split_line(const string &line, ourvector<string> &items){
    string element;
    stringstream ss(line);
    while(getline(ss, element, ',')){
        items.push_back(element);
    }
}

//Loads districts data into a state info. Prints state name and districts.
state_info load_districts(const string &line){
    ourvector<string> items;
    ourvector<int> districts;
    split_line(line, items);

    state_info state;

    state.name = items.at(0);
    state.districts.push_back(1); //in case first district is "AL"

    for(int i = 2; i < items.size(); i++){
        state.districts.push_back(stoi(items.at(i)));
    }

    cout << "..." << state.name << "..." << state.districts.size() / 3 << " districts total\n";

    return state;
}

//Loads eligible voters data into struct data: voters.
//if there is states in file2 that doesn't exist in file1, ignore it
void load_voters(const string &line, ourvector<state_info> &states){
    ourvector<string> items;
    split_line(line, items);
    int index = search(states, items.at(0));
    if(items.size() > 0 && index != -1) {
        states.at(index).voters = stoi(items.at(1));
        cout << "..." << items.at(0) << "..." << items.at(1) << " eligible voters\n";
    }
}

//Loads and displays voting data to the screen. Returns a boolean showing if the files are loaded. 
//calls sort function for the vector to store eligible_voter info
bool load(string districts_file, string voters_file, ourvector<state_info> &states){
    ifstream file;
    string line;
    file.open(districts_file);
    if(!file){
        cout << "\nInvalid first file, try again.\n";
        return 0;
    }
    cout << "\nReading: " << districts_file << endl;
    while(getline(file, line))
    {
        states.push_back(load_districts(line));
    }
    file.close();
    sort(states);
    
    file.open(voters_file);
    if(!file){
        cout << "\nInvalid second file, try again.\n";
        return 0;
    }
    cout << "\nReading: " << voters_file << endl;
    while(getline(file, line))
    {
        load_voters(line, states);
    }
    file.close(); 

    return 1;
}

//Calculates total wasted votes for state and efficiency factor
void calc_wasted_votes(state_info &state, int &wasted_democrat, int &wasted_republican, double &efficiency_factor){
    wasted_democrat = 0, wasted_republican = 0;
    int total_votes = 0;

    for(int i = 1; i < state.districts.size(); i += 3) {
        int democrats = state.districts.at(i), republicans = state.districts.at(i + 1),
        district_overhalf = (democrats + republicans) / 2 + 1;

        total_votes += democrats + republicans;

        if(democrats > republicans) {
            wasted_democrat += democrats - district_overhalf;
            wasted_republican += republicans;
        }
        else {
            wasted_republican += republicans - district_overhalf;
            wasted_democrat += democrats;
        }
  }
  efficiency_factor = 100 * (abs(wasted_democrat - wasted_republican) / (double)total_votes);
}

//shows gerrymandering stats for a given state
//if gerrymandered, show gerrymandered against, efficiency factor(%)
void stats(state_info &state){
    int wasted_democrat, wasted_republican;
    double efficiency_factor;
    calc_wasted_votes(state, wasted_democrat, wasted_republican, efficiency_factor);

    if(efficiency_factor >= 7.0 && state.districts.size() > 9){
        cout << "\nGerrymandered: Yes\n";
        if(wasted_democrat > wasted_republican) {
            cout << "Gerrymandered against: Democrats\n";
        }
        else {
            cout << "Gerrymandered against: Republicans\n";
        }
        cout << "Efficiency Factor: " << efficiency_factor << "%\n";
    }
    else{
        cout << "\nGerrymandered: No\n";
    }

    cout << "Wasted Democratic votes: " << wasted_democrat << endl;
    cout << "Wasted Republican votes: " << wasted_republican << endl;
    cout << "Eligible voters: " << state.voters << endl;
}

//plots the percent of democrats to percent of republicans using Ds and Rs, 100 characters per district.
void plot(state_info &state){
    for(int i = 1; i < state.districts.size(); i += 3) {
        cout << "\nDistrict: " << i / 3 + 1 << endl;
        int democrats = state.districts.at(i), republicans = state.districts.at(i + 1),
            total = democrats + republicans,
            democrats_plot = (democrats / (double)total) * 100,
            republicans_plot = 100 - democrats_plot;   
        
        for(int j = 0; j < democrats_plot; j++){
            cout << "D";
        }
        for(int j = 0; j < republicans_plot; j++){
            cout << "R";
        }

    }
}

//Prints out main screen data, such as is data loaded? and the selected state
string main_screen(bool data_loaded, int state_index, ourvector<state_info> &states){
    string command;

    cout << "\nData loaded? ";
    if(data_loaded) {
        cout << "Yes\n";
    }
    else {
        cout << "No\n";
    }

    if(state_index == -1){
        cout << "State: N/A\n";
    }
    else{
        cout << "State: " << states.at(state_index).name << endl;
    }

    cout << "\nEnter command: ";
    cin >> command;
    cout << "\n-----------------------------\n";

    return command;
}

//runs commands and checks if filename/state selected is valid
bool command_validity(string command, bool has_data, int state_index, int state_size) {
    if(command == "load") {
        if(has_data) {
                cout << "Already read data in, exit and start over.\n";
                return 0;
        }
    }
    else if(command == "search") {
        if(state_size == 0) {
            cout << "\nNo data loaded, please load data first.\n";
            return 0;
        }
    }
    else if(command == "stats" || command == "plot") {
        if(!has_data) {
            cout << "No data loaded, please load data first.";
            return 0;
        }
        else if(state_index == -1) {
            cout << "No state indicated, please search for state first.";  
            return 0;
        }
    }
    return 1;
}

//reads commands and changes data for states, index, and has_data for use in main.
void run_command(string command, ourvector<state_info> &states, int &state_index, bool &has_data) {
    if(command == "load") {
            string file1, file2;
            cin >> file1 >> file2;
                has_data = load(file1, file2, states);
        }
    else if(command == "search"){
        string state;
        getline(cin, state);
        if(state != "") state.erase(0,1);
        if((state_index = search(states, state)) < 0) {
            cout << "State does not exist, search again.\n";
        }
    }
    else if(command == "stats"){
        stats(states.at(state_index));
    }
    else if(command == "plot"){
        plot(states.at(state_index));
    }
}

int main() {
    ourvector<state_info> states;
    string command = "";
    int state_index = -1;
    bool has_data = false;

    cout << "Welcome to the Gerrymandering App!\n";

    while(command != "exit") {
        command = main_screen(has_data, state_index, states);
        
        if( !command_validity(command, has_data, state_index, states.size()) ) {
            cin.ignore(10000, '\n');
            continue;
        }
        run_command(command, states, state_index, has_data);
    }
    
    return 0;
}