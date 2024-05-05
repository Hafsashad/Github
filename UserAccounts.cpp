#include "UserAccounts.h"
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
using namespace std;
Hashtable::Hashtable() {
    for (int i = 0; i < size; i++) {
        arr[i] = nullptr;
    }
}

int Hashtable::hashIndex(const string& key) {
    int sum = 0;
    for (char c : key) {
        sum += c;
    }
    return sum % size;
}

void Hashtable::insert(const vector<vector<string>>& data) {
    for (const auto& row : data) {
        if (row.size() >= 3) { // Ensure row has at least 3 elements (username, password, email)
            const string& username = row[0];
            const string& password = row[1];
            const string& email = row[2];
            int index = hashIndex(username);
            Node* newNode = new Node(username, password, email);

            if (!arr[index]) {
                arr[index] = newNode;
            }
            else {
                Node* temp = arr[index];
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        }
    }
    writeToFile("user_accounts.csv"); // Call writeToFile function after inserting data
}
void Hashtable::insertForfiles(const vector<vector<string>>& data) {
    for (const auto& row : data) {
        if (row.size() >= 3) { // Ensure row has at least 3 elements (username, password, email)
            const string& username = row[0];
            const string& password = row[1];
            const string& email = row[2];
            int index = hashIndex(username);
            Node* newNode = new Node(username, password, email);

            if (!arr[index]) {
                arr[index] = newNode;
            }
            else {
                Node* temp = arr[index];
                while (temp->next) {
                    temp = temp->next;
                }
                temp->next = newNode;
            }
        }
    }
}
bool Hashtable::search(const vector<vector<string>>& data) {
    for (const auto& row : data) {
        if (row.size() >= 2) { // Ensure row has at least 2 elements (username, password)
            const string& username = row[0];
            const string& password = row[1];
            int index = hashIndex(username);

            readFromFile("user_accounts.csv"); // Call readFromFile function before searching

            Node* current = arr[index];
            while (current) {
                if (current->username == username && current->password == password) {
                    return true;
                }
                current = current->next;
            }
        }
    }
    return false;
}

void Hashtable::display() {
    for (int i = 0; i < size; i++) {
        cout << i << ": ";
        Node* current = arr[i];
        while (current) {
            cout << "Username: " << current->username << ", Email: " << current->email << " ";
            current = current->next;
        }
        cout << endl;
    }
}

void Hashtable::writeToFile(const string& filename) {
    ofstream outFile(filename);
    if (outFile.is_open()) {
        for (int i = 0; i < size; i++) {
            Node* current = arr[i];
            while (current) {
                outFile << current->username << "," << current->password << "," << current->email << endl;
                current = current->next;
            }
        }
        outFile.close();
    }
    else {
        cerr << "Unable to open file: " << filename << endl;
    }
}

void Hashtable::readFromFile(const string& filename) {
    ifstream inFile(filename);
    if (!inFile.is_open()) {
        cerr << "Unable to open file: " << filename << endl;
        return;
    }
    string line;
    while (getline(inFile, line)) {
        stringstream ss(line);
        string username, password, email;
        getline(ss, username, ',');
        getline(ss, password, ',');
        getline(ss, email, ',');
        vector<vector<string>> data = { {username, password, email} };//we use 2D vector bcz CSV is a excel sheet
        insert(data);
    }
    inFile.close();
}

void UserAccounts::registration(const string& username, const string& password, const string& email) {
   
    vector<vector<string>> data = { {username, password, email} };
    if (hashtable.search(data))
    {
        cout << "Already exist this account" << endl;
        return;
    }
    hashtable.insert(data);
}

bool UserAccounts::login(const string& username, const string& password) {
    vector<vector<string>> data = { {username, password} };
    if (hashtable.search(data)) {
        cout << "Login successful!" << endl;
        return true;
    }
    else {
        cout << "Invalid username and password" << endl;
        return false;
    }
}

void UserAccounts::logout() {
    cout << "Logout successfully" << endl;
}

void UserAccounts::saveToDisk(const string& filename) {
    hashtable.writeToFile(filename + ".csv");
}

void UserAccounts::loadFromDisk(const string& filename) {
    hashtable.readFromFile(filename + ".csv");
}
//social features
void UserAccounts::follow(const std::string& follower, const std::string& followee) {
    followGraph[follower].insert(followee);
}
void UserAccounts::addUser(const std::string& username, const std::string& password, const std::string& email) {
    hashtable.insert({ {username, password, email} });
}

void UserAccounts::unfollow(const std::string& follower, const std::string& followee) {
    if (followGraph.find(follower) != followGraph.end()) {
        followGraph[follower].erase(followee);
    }
}

void UserAccounts::displayFollowing(const std::string& username) {
    if (followGraph.find(username) != followGraph.end()) {
        std::cout << "Users followed by " << username << ":\n";
        for (const auto& followee : followGraph[username]) {
            std::cout << "Username: " << followee << std::endl;
        }
    }
    else {
        std::cout << "User not found or not following anyone.\n";
    }
}
