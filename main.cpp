#include <iostream>
#include <string>
#include "UserAccounts.h"
#include "Repository.h"
using namespace std;

int main()
{
    int c;
    cout << "Enter your choice: " << endl;
    cout << "1-Registration" << endl;
    cout << "2-Login" << endl;
    cin >> c;
    cin.ignore(); // Ignore the newline character in the buffer
    UserAccounts userAccounts;
    RepositoryTree tree;
    tree.readFromRpoFile(tree);
    tree.LoadFromFile(tree);
    //load from branches.csv
    //load from brachfiles.csv
    if (c == 1) {
        string username, password, email;
        cout << "Enter username: ";
        getline(cin, username);
        cout << "Enter password: ";
        getline(cin, password);
        cout << "Enter email: ";
        getline(cin, email);
        userAccounts.registration(username, password, email);
        cout << "Registration successful!" << endl;
    }
    else if (c == 2)
    {
      
        string username, password;
        cout << "Enter username: ";
        getline(cin, username);
        cout << "Enter password: ";
        getline(cin, password);
        
        if (userAccounts.login(username, password))
        {
            cout << "Login successful!" << endl;

            //User logged in, now allow repository actions
           // RepositoryTree repo;// Create repository with owner's username
            int choice;
                cout << "-------------------Menu----------------------------" << endl;
                cout << "1-Create Repository" << endl;
                cout << "2-Add File to Repository" << endl;
                cout << "3-Delete file from repository" << endl;
                cout << "4-Fork" << endl;
                cout << "5-Follow" << endl;
                cout << "6-Unfollow" << endl;
                cout << "7-Display Following list" << endl;
                cin >> choice;
                cin.ignore();
                switch (choice)
                {
                    case 1: {
                        tree.createRepo(username);
                        break;
                    }
                    case 2: {
                        tree.displayTree(tree.root);
                        tree.addFileRepo(username);
                        break;
                    }
                    case 3:
                    {
                        tree.displayTree(tree.root);
                        tree.deleteFileRepo(username);
                        break;
                    }
                    case 4:
                    {
                        tree.displayTree(tree.root);
                        int repoIDFork;
                        cout << "Enter the ID of the repository you want to fork: ";
                        cin >> repoIDFork;

                        // Check if the repoIDFork exists among the repositories owned by the current user
                        RepoNode* userRepo = tree.searchRepoByID(repoIDFork, tree.root);
                        if (userRepo != nullptr && userRepo->ownername == username)
                        {
                            cout << "You cannot fork your own repository." << endl;
                        }
                        else if (userRepo != nullptr)
                        {
                            // User can fork this repository
                            tree.makeBranch(repoIDFork, username);

                            // Check if the repoIDFork exists in forks.csv
                            ifstream forksFile("forks.csv");
                            bool repoExists = false;
                            vector<string> lines; // To store lines for writing back to file
                            string line;
                            while (getline(forksFile, line))
                            {
                                stringstream ss(line);
                                string id;
                                getline(ss, id, ',');
                                if (stoi(id) == repoIDFork)
                                {
                                    repoExists = true;
                                    // Increment the fork count for this repoIDFork
                                    userRepo->forkcount++;
                                    // Update the fork count in the file
                                    stringstream updatedLine;
                                    updatedLine << repoIDFork << "," << userRepo->forkcount;
                                    lines.push_back(updatedLine.str());
                                }
                                else
                                {
                                    lines.push_back(line);
                                }
                            }
                            forksFile.close();

                            // If repoIDFork doesn't exist in forks.csv, add it
                            if (!repoExists)
                            {
                                // Increment the fork count for this repoIDFork
                                userRepo->forkcount++;

                                // Add the repoIDFork and its fork count into lines
                                stringstream newLine;
                                newLine << repoIDFork << "," << userRepo->forkcount;
                                lines.push_back(newLine.str());
                            }

                            // Write the lines back to forks.csv
                            ofstream forksFileWrite("forks.csv");
                            if (forksFileWrite.is_open())
                            {
                                for (const string& updated : lines)
                                {
                                    forksFileWrite << updated << endl;
                                }
                                forksFileWrite.close();
                            }
                            else
                            {
                                cerr << "Error: Unable to open forks.csv for writing." << endl;
                            }
                        }


                        else
                        {
                            cout << "Repository not found." << endl;
                        }
                        break;
                    }
                    case 5: {
                        string followee;
                        cout << "Enter the username of the user you want to follow: ";
                        cin >> followee;
                        userAccounts.follow(username, followee);
                        break;
                    }
                    case 6: {
                        string unfollowee;
                        cout << "Enter the username of the user you want to unfollow: ";
                        cin >> unfollowee;
                        userAccounts.unfollow(username, unfollowee);
                        break;
                    }
                    case 7: {
                        userAccounts.displayFollowing(username);
                        break;
                    }
                    case 8:

                        return 0;
                        break;
                }
          
        }
        else {
            cout << "Invalid choice" << endl;
        }


        return 0;
    }
}
