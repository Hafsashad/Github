#ifndef USERACCOUNTS_H
#define USERACCOUNTS_H
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
using namespace std;
class Node {
public:
    string username;
    string password;
    string email;
    Node* next;
    Node(const string& username, const string& password, const string& email)
        : username(username), password(password), email(email), next(nullptr) {}
};

class Hashtable {
private:
    static const int size = 100;
    Node* arr[size];

public:
    Hashtable();
    int hashIndex(const string& key);
    void insert(const vector<vector<string>>& data);
    bool search(const vector<vector<string>>& data);
    void display();
    void writeToFile(const string& filename);
    void readFromFile(const string& filename);
    void insertForfiles(const vector<vector<string>>& data);
};

class UserAccounts {
private:
    Hashtable hashtable;
    std::unordered_map<std::string, std::unordered_set<std::string>> followGraph;

public:
    void registration(const string& username, const string& password, const string& email);
    bool login(const string& username, const string& password);
    void logout();
    void saveToDisk(const string& filename);
    void loadFromDisk(const string& filename);
    // Social feature methods
    void addUser(const std::string& username, const std::string& password, const std::string& email);
    void follow(const std::string& follower, const std::string& followee);
    void unfollow(const std::string& follower, const std::string& followee);
    void displayFollowing(const std::string& username);
};

#endif // USERACCOUNTS_H
