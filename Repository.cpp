#include "Repository.h"
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include<cstdio>
int RepoNode::lastRepoID = 0;
FileNode::FileNode(string name,string content)
{
	filename = name;
	next = nullptr;
	FileContent = content;
}
RepoNode::RepoNode(string reponame,string ownername,bool m,FileNode* readme)
{
	forkcount = 0;
	this->ownername = ownername;
	RepoName = reponame;
	mode = m;//true for public and false for private
	this->readme = readme;
	files = FileLL();
	branches = BranchLL();
	ifstream file("RepoID.txt");
	file >> this->lastRepoID;
	left = NULL;
	right = NULL;
	this->repoID = lastRepoID;
}
RepositoryTree::RepositoryTree()
{
	root = NULL;
} 
void RepositoryTree::insertRepoInBST(RepoNode*& root, RepoNode* newnode) {
	if (root == nullptr) {
		root = newnode;
	}
	else {
		if (newnode->repoID < root->repoID) {
			insertRepoInBST(root->left, newnode);
		}
		else {
			insertRepoInBST(root->right, newnode);
		}
	}
}
RepoNode* RepositoryTree::searchRepoByID(int id, RepoNode* root)
{
	if (root->repoID == id)
	{
		return root;
	}
	else if (id < root->repoID)
	{
		searchRepoByID(id, root->left);
	}
	else if (id > root->repoID)
	{
		searchRepoByID(id, root->right);
	}
	else {
		cout << "repoid " << id << " not found in BST tree"<<endl;
	}
}
int RepositoryTree::convertStringToInt(const string& str) {
	stringstream ss(str);
	int result;
	ss >> result;
	return result;
}
void RepositoryTree::WriteInRepoFile(int ID, string ownername, string reponame, bool m, string readFilename)
{
	ofstream file("Repository.csv", ios::app);
	file << ID << "," << ownername << "," << reponame << "," << (m=true ? "true" : "false") << "," << readFilename << endl;
	file.close();
}
void RepositoryTree::displayTree(RepoNode* root) {
	if (root != NULL) {
		displayTree(root->left);
		cout << root->repoID << ": " << root->RepoName << endl;
		cout << "Files:" << endl;
		root->files.printFileNames(); // Print file names associated with the repository
		displayTree(root->right);
	}
}

void RepositoryTree::readFromRpoFile(RepositoryTree& tree) {
	ifstream file("Repository.csv");
	if (!file.is_open()) {
		cout << "Unable to open Repository.csv" << endl;
		return;
	}
	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		string id, owner, repo, m, readme;
		getline(ss, id, ',');
		getline(ss, owner, ',');
		getline(ss, repo, ',');
		getline(ss, m, ',');
		getline(ss, readme);

		// Convert mode string to boolean
		bool mode = (m == "true");

		// Create README node
		FileNode* readmeNode = new FileNode(readme, "");

		// Create new repository node
		RepoNode* newNode = new RepoNode(repo, owner, mode, readmeNode);
		newNode->repoID = convertStringToInt(id); // Set repository ID

		// Insert new node into the tree
		tree.insertRepoInBST(tree.root, newNode);
	}
	file.close();
}

void RepositoryTree::createRepo(string ownername)
{
	string reponame;
	cout << "Enter repo name: ";
	getline(cin, reponame);

	cout << "Choose mode" << endl;
	cout << "Enter 1 for public " << endl;
	cout << "Enter 0 for private" << endl;
	bool m;
	cin >> m;

	cin.ignore(); // Clear the newline character

	string discription;
	cout << "Enter description: ";
	getline(cin, discription);

	// Create README file
	ofstream readmeFile(reponame + "_Readme.txt");
	if (readmeFile.is_open())
	{
		readmeFile << discription; // Write description to README file
		readmeFile.close();
	}
	else
	{
		cout << "Unable to create README file: " << reponame + "_Readme.txt" << endl;
		return; // Exit function if unable to create README file
	}

	FileNode* readme = new FileNode(reponame + "_Readme.txt", discription);
	RepoNode* newnode = new RepoNode(reponame, ownername, m, readme);
	newnode->lastRepoID++;
	newnode->repoID = newnode->lastRepoID;
	ofstream file("RepoID.txt");
	file << newnode->lastRepoID;
	file.close();
	newnode->readme->filename = reponame + "_Readme.txt";
	
		insertRepoInBST(root, newnode);
		WriteInRepoFile(newnode->repoID, ownername, reponame, m, reponame + "_Readme.txt");
}
void RepositoryTree::addFileRepo(const string& loggedInUser)
{
	// Prompt the user to enter the ID of the repository
	int repoID;
	cout << "Enter the ID of the repository where you want to add a file: ";
	cin >> repoID;
	cin.ignore(); // Clear the newline character from the buffer

	// Search for the repository by ID
	RepoNode* repo = searchRepoByID(repoID, root);

	// Check if the repository exists and belongs to the current user
	if (repo != nullptr && repo->ownername == loggedInUser)
	{
		// Prompt the user to enter the filename and content of the file to be added
		string file, content, type;
		cout << "Enter the filename: ";
		getline(cin, file);
		cout << "Enter the content of the file: ";
		getline(cin, content);
		cout << "Enter the type of file (e.g., txt, cpp): ";
		cin >> type;

		// Concatenate filename and type
		string filename = file + "." + type;

		// Add the file to the repository's file linked list
		repo->files.insert(filename, content);
		// Optionally, you may want to update the file in the CSV or any other file storage here
		writeInFiles(repoID, filename, repo->ownername);
		
		cout << "File added successfully to repository " << repo->RepoName << endl;
	}
	else
	{
		cout << "Repository not found or you do not have permission to add files to it." << endl;
	}
	
}
void RepositoryTree:: deleteFileRepo(string& loggedInUser)
{
	int id;
	cout << "Enter id of repo whose files you want to delete:";
	cin >> id;
	string filename;
	cin.ignore();
	cout << "Enter filename with type: ";
	getline(cin, filename);
	RepoNode* repo = searchRepoByID(id, root);
	repo->files.remove(filename);
	removeEntryFromFileCSV(id, filename);
}
void RepositoryTree::writeInFiles(int repoID, const string& filename,string ownername)
{
	ifstream read("files.csv");
	vector<string> lines;
	bool idFound = false;
	string line;
	while (getline(read, line))
	{
		stringstream ss(line);
		string id;
		getline(ss, id, ',');
		// If repoID matches, append filename 
		if (convertStringToInt(id) == repoID)
		{
			line += "," + filename;
			idFound = true;
		}
		lines.push_back(line);
	}
	read.close();
	// If repoID not found, add a new line for it
	if (!idFound)
	{
		lines.push_back(to_string(repoID) +","+ownername+"," + filename);
	}
	// Rewrite the entire file with modified lines
	ofstream file("files.csv", ios::trunc); // Open in truncation mode to clear existing content
	for (const auto& modifiedLine : lines)
	{
		file << modifiedLine << endl;
	}
	file.close();
}
void RepositoryTree::removeEntryFromFileCSV(int repoID, const string& filename) {
	ifstream inFile("files.csv");
	if (!inFile.is_open()) {
		cerr << "Error: Unable to open input file." << endl;
		return;
	}

	ofstream outFile("temp.csv");
	if (!outFile.is_open()) {
		cerr << "Error: Unable to create output file." << endl;
		inFile.close(); // Close the input file
		return;
	}

	string line;
	// Read each line from files.csv and update the line if needed
	while (getline(inFile, line)) {
		stringstream ss(line);
		string id, owner, fileList;
		getline(ss, id, ',');
		getline(ss, owner, ',');
		getline(ss, fileList);

		// Check if the repository ID matches
		if (convertStringToInt(id) == repoID) {
			// Create a stringstream to process the list of files
			stringstream fileListStream(fileList);
			string file;
			string updatedFileList;

			// Parse the list of files, excluding the specified file
			while (getline(fileListStream, file, ',')) {
				if (file != filename) {
					updatedFileList += file + ",";
				}
			}

			// Remove the trailing comma
			if (!updatedFileList.empty()) {
				updatedFileList.pop_back();
			}

			// Write the updated line to the output file
			outFile << id << "," << owner << "," << updatedFileList << endl;
		}
		else {
			// Write the original line to the output file
			outFile << line << endl;
		}
	}

	// Close files
	inFile.close();
	outFile.close();

	// Replace the original file with the modified file
	remove("files.csv");
	rename("temp.csv", "files.csv");
}

void RepositoryTree::LoadFromFile(RepositoryTree& tree) {
	ifstream file("files.csv");
	if (!file.is_open()) {
		cout << "Unable to open files.csv" << endl;
		return;
	}

	string line;
	while (getline(file, line)) {
		stringstream ss(line);
		string repoID_str, owner, filename;

		// Extract repository ID and owner name
		getline(ss, repoID_str, ',');
		getline(ss, owner, ',');

		// Convert repository ID from string to integer
		int repoID = stoi(repoID_str);

		// Search for the repository node by ID
		RepoNode* repo = searchRepoByID(repoID, tree.root);

		// If the repository node is found, insert files into its linked list
		if (repo != nullptr) {
			while (getline(ss, filename, ',')) {
				// Insert the file into the repository's file linked list
				repo->files.insert(filename, ""); // Assume content is empty for now
			}
		}
		else {
			cout << "Repository with ID " << repoID << " not found." << endl;
		}
	}

	file.close();
}
void RepositoryTree::makeBranch(int id, string loggedInUser)
{
	RepoNode* repo = searchRepoByID(id, root);
	BranchNode* branch = new BranchNode(loggedInUser, loggedInUser + "Branch", repo->mode, repo->readme);
	BranchNode* curr = repo->branches.head;
	while (curr != NULL)
	{
		cout << "Current branch owner: " << curr->branchowner << ", Logged-in user: " << loggedInUser << endl;
		if (curr->branchowner == loggedInUser)
		{
			cout << "You already have a branch." << endl;
			bool c = iscommit();
			if (c)
			{
				addcommit(branch);
			}
			return;
		}
		curr = curr->next;
	}

	repo->branches.insert(loggedInUser, loggedInUser + "Branch", repo->mode, repo->readme);
	cout << "You have successfully forked this repository with ID " << id << endl;
	bool ans = iscommit();
	if (ans)
	{
		addcommit(branch);
	}
	writeInBranches(id, branch);
}

void RepositoryTree::addcommit(BranchNode* branch)
{
	cin.ignore(); // Clear newline character from buffer
	string filename;
	cout << "Enter filename: ";
	getline(cin, filename);
	string type;
	cout << "Enter type of file: ";
	getline(cin, type);
	string content;
	cout << "Enter content of file: ";
	getline(cin, content);
	ofstream f(filename + "." + type);
	f << content;
	f.close();
	branch->branchfiles.insert(filename, content);
	cout << "Successfully committed changes." << endl;
	// Write to "branchfiles.csv"
}

bool RepositoryTree::iscommit()
{
	int c;
	cout << "Do you want to commit changes?" << endl;
	cout << "1 - Commit changes" << endl;
	cout << "2 - Do not commit changes" << endl;
	cin >> c;
	return (c == 1);
}
void RepositoryTree::writeInBranches(int id, BranchNode* branch)
{
	ifstream inFile("branches.csv");
	if (!inFile.is_open())
	{
		cerr << "Error: Unable to open branches.csv for reading." << endl;
		return;
	}

	vector<string> lines;
	string line;
	bool idFound = false;

	// Read each line from branches.csv and update the line if needed
	while (getline(inFile, line))
	{
		stringstream ss(line);
		string repoID;
		getline(ss, repoID, ',');

		// If repoID matches, append branch information
		if (convertStringToInt(repoID) == id)
		{
			line += "_" + branch->branchowner + "," + branch->branchName + "," +
				(branch->branchmode ? "public" : "private") + "," + branch->branchreadme->filename;
			idFound = true;
		}
		lines.push_back(line);
	}

	inFile.close();

	// If repoID not found, add a new line for it
	if (!idFound)
	{
		lines.push_back(to_string(id) + "," + branch->branchowner + "," +
			branch->branchName + "," + (branch->branchmode ? "public" : "private") +
			"," + branch->branchreadme->filename);
	}

	// Rewrite the entire file with modified lines
	ofstream outFile("branches.csv", ios::trunc); // Open in truncation mode to clear existing content
	for (const auto& modifiedLine : lines)
	{
		outFile << modifiedLine << endl;
	}
	outFile.close();
}
void RepositoryTree::LoadfromBraches(RepositoryTree& tree)
{
	ifstream inFile("branches.csv");
	if (!inFile.is_open())
	{
		cerr << "Error: Unable to open branches.csv for reading." << endl;
		return;
	}

	string line;
	while (getline(inFile, line))
	{
		stringstream ss(line);
		string repoID, branchOwner, branchName, branchMode, branchReadme;

		getline(ss, repoID, ',');
		getline(ss, branchOwner, '_'); // Branch owner is separated by '_'
		getline(ss, branchName, ',');
		getline(ss, branchMode, ',');
		getline(ss, branchReadme, ',');

		// Convert branchMode to boolean
		bool mode = (branchMode == "public") ? true : false;

		// Create a new BranchNode
		BranchNode* branch = new BranchNode(branchOwner, branchName, mode, new FileNode(branchReadme, ""));

		// Insert the branch into the tree's branches
		tree.root->branches.insert(branch->branchowner,branch->branchName,branch->branchmode,branch->branchreadme);
	}

	inFile.close();
}
//void RepositoryTree::writeInFiles(int repoID, const string& filename,string ownername)
//{
//	ifstream read("files.csv");
//	vector<string> lines;
//	bool idFound = false;
//	string line;
//	while (getline(read, line))
//	{
//		stringstream ss(line);
//		string id;
//		getline(ss, id, ',');
//		// If repoID matches, append filename 
//		if (convertStringToInt(id) == repoID)
//		{
//			line += "," + filename;
//			idFound = true;
//		}
//		lines.push_back(line);
//	}
//	read.close();
//	// If repoID not found, add a new line for it
//	if (!idFound)
//	{
//		lines.push_back(to_string(repoID) +","+ownername+"," + filename);
//	}
//	// Rewrite the entire file with modified lines
//	ofstream file("files.csv", ios::trunc); // Open in truncation mode to clear existing content
//	for (const auto& modifiedLine : lines)
//	{
//		file << modifiedLine << endl;
//	}
//	file.close();
//}
//void RepositoryTree::insertRepoInBST(RepoNode*& root, RepoNode* newnode)
//{
//	if (root == NULL)
//	{
//		root = newnode;
//	}
//	else if (newnode->repoID < root->repoID)
//	{
//		insertRepoInBST(root->left, newnode);
//	}
//	else if (newnode->repoID > root->repoID)
//	{
//		insertRepoInBST(root->right, newnode);
//	}
//}
//void RepositoryTree::readFromRpoFile(RepositoryTree& tree)
//{
//	ifstream file("Repository.csv");
//	if (!file.is_open())
//	{
//		cout << "Unable to open Repository.csv" << endl;
//		return;
//	}
//
//	string line;
//	while (getline(file, line))
//	{
//		stringstream ss(line);
//		string id, owner, repo, m, readme;
//		getline(ss, id, ',');
//		getline(ss, owner, ',');
//		getline(ss, repo, ',');
//		getline(ss, m, ',');
//		getline(ss, readme);
//
//		ifstream readmeFile(readme);
//		if (!readmeFile.is_open())
//		{
//			cout << "Unable to open readme file: " << readme << endl;
//			continue; // Skip this repository if readme file cannot be opened
//		}
//
//		string readmeContent;
//		string readmeLine;
//		while (getline(readmeFile, readmeLine))
//		{
//			readmeContent += readmeLine;
//		}
//
//		bool mode = (m == "true");
//		FileNode* readmeNode = new FileNode(readme, readmeContent);
//		int repoID = convertStringToInt(id);
//		RepoNode* newNode = new RepoNode(repo, owner, mode, readmeNode);
//		tree.insertRepoInBST(tree.root, newNode);
//	}
//	file.close();
//}
//
//void RepositoryTree::WriteInRepoFile(int ID, string ownername, string reponame, bool m, string readFilename)
//{
//	ofstream file("Repository.csv", ios::app);
//	file << ID << "," << ownername << "," << reponame << "," << (m ? "true" : "false") << "," << readFilename << endl;
//	file.close();
//}
//int RepositoryTree::convertStringToInt(const string& str) {
//	stringstream ss(str);
//	int result;
//	ss >> result;
//	return result;
//}
//RepoNode* RepositoryTree::searchRepoByID(int id, RepoNode* node)
//{
//	if (node == NULL)
//	{
//		return NULL;
//	}
//
//	if (id < node->repoID)
//	{
//		return searchRepoByID(id, node->left);
//	}
//	else if (id > node->repoID)
//	{
//		return searchRepoByID(id, node->right);
//	}
//	else
//	{
//		return node;
//	}
//}
//void RepositoryTree::displayTree()
//{
//	ifstream file("Repository.csv");
//	string line;
//	while (getline(file, line))
//	{
//		stringstream ss(line);
//		string id, name;
//		getline(ss, id, ',');
//		getline(ss, name, ',');
//		cout << id << "-" << name << endl;
//	}
//	file.close();
//}
//void RepositoryTree::createRepo(string ownername)
//{
//	string reponame;
//	cout << "Enter repo name: ";
//	getline(cin, reponame);
//
//	cout << "Choose mode" << endl;
//	cout << "Enter 1 for public " << endl;
//	cout << "Enter 0 for private" << endl;
//	bool m;
//	cin >> m;
//
//	cin.ignore(); // Clear the newline character
//
//	string discription;
//	cout << "Enter description: ";
//	getline(cin, discription);
//
//	// Create README file
//	ofstream readmeFile(reponame + "_Readme.txt");
//	if (readmeFile.is_open())
//	{
//		readmeFile << discription; // Write description to README file
//		readmeFile.close();
//	}
//	else
//	{
//		cout << "Unable to create README file: " << reponame + "_Readme.txt" << endl;
//		return; // Exit function if unable to create README file
//	}
//
//	FileNode* readme = new FileNode(reponame + "_Readme.txt", discription);
//	RepoNode* newnode = new RepoNode(reponame, ownername, m, readme);
//	newnode->lastRepoID++;
//	newnode->repoID = newnode->lastRepoID;
//	ofstream file("RepoID.txt");
//	file << newnode->lastRepoID;
//	file.close();
//	newnode->readme->filename = reponame + "_Readme.txt";
//	insertRepoInBST(root,newnode);
//	WriteInRepoFile(newnode->repoID, ownername, reponame, m, reponame + "_Readme.txt");
//}
////void RepositoryTree::LoadFromFile(RepositoryTree& tree) {
////	string line;
////	ifstream file("files.csv");
////
////	while (getline(file, line)) {
////		string id, owner, files;
////		stringstream ss(line);
////		getline(ss, id, ',');
////		getline(ss, owner, ',');
////		getline(ss, files);
////		int ID = convertStringToInt(id);
////		RepoNode* s = tree.searchRepoByID(ID, tree.root);
////
////		if (s != nullptr) {
////			stringstream fileSS(files);
////			string filename;
////
////			while (getline(fileSS, filename, ',')) {
////				ifstream fileContent(filename);
////				string content;
////
////				if (fileContent.is_open()) {
////					string fileLine;
////					while (getline(fileContent, fileLine)) {
////						content += fileLine;
////						content += "\n"; // Add newline to separate lines
////					}
////					fileContent.close();
////
////					// Add the file to the repository's linked list of files
////					tree.addFileRepo(owner);
////				}
////				else {
////					cout << "Unable to open file: " << filename << endl;
////					continue; // Move to the next file
////				}
////			}
////		}
////		else {
////			cout << "Repository with ID " << ID << " not found." << endl;
////		}
////	}
////
////	file.close();
////}
//
//void RepositoryTree::addFileRepo(const string& loggedInUser)
//{
//	int id;
//	cout << "Enter repoID to add a file: ";
//	cin >> id;
//	cin.ignore();
//	// Read repository information from the file and update the repository tree
//	//readFromRpoFile();hum is ko ab main mein pehla is ko load kraye ga phir agla kaam kra ga
//	RepoNode* s = searchRepoByID(id, root);
//	//if (s != nullptr) {
//		// Check if the logged-in user matches the owner of the repository
//		
//		string fileType;
//		cout << "Enter the type of file (e.g., txt, cpp, etc.): ";
//		cin >> fileType;
//		cin.ignore();
//
//		string filename;
//		cout << "Enter the path to the file you want to add: ";
//		getline(cin, filename);
//		filename += "." + fileType;
//
//		ofstream file(filename);
//		if (file.is_open()) {
//			string content;
//
//
//			cout << "Enter content to store in the file: ";
//			getline(cin, content);
//			file << content;
//			file.close();
//
//			/*FileNode* newFile = new FileNode(filename, content);
//			if (s->files == nullptr) {
//				s->files = newFile;
//			}
//			else {
//				FileNode* current = s->files;
//				while (current->next != nullptr) {
//					current = current->next;
//				}
//				current->next = newFile;
//			}*/
//			writeInFiles(id, filename, s->ownername);
//		}
//		else {
//			cout << "Unable to create file: " << filename << endl;
//		}
//	//}
//	/*else {
//		cout << "Repository with ID " << id << " not found." << endl;
//	}*/
//}
//
//void RepositoryTree::deleteFileRepo(string& loggedInUser)
//{
//	displayTree();
//	int id;
//	cout << "Enter repoID to delete a file: ";
//	cin >> id;
//	cin.ignore();
//	string file;
//	cout << "Enter filename to delete from RepoID: ";
//	getline(cin, file);
//	// Read repository information from the file and update the repository tree
//	//readFromRpoFile();main mein jb programrun ho ga data sb sa pehla tree mein load ho ga so is ki zaroratt ni ab yaha
//	// Search for the repository with the given ID
//	RepoNode* s = searchRepoByID(id, root);
//	if (s != nullptr) {
//		// Check if the logged-in user matches the owner of the repository
//		if (s->ownername != loggedInUser) {
//			cout << "You do not have permission to delete a file from this repository." << endl;
//			return;
//		}
//
//		// Traverse through the file nodes to find and delete the file
//		FileNode* curr = s->files;
//		FileNode* prev = nullptr; // Keep track of the previous node
//		while (curr != nullptr)
//		{
//			if (curr->filename == file)
//			{
//				// If the file is found, delete it
//				if (prev == nullptr) {
//					// If the file to be deleted is the first file in the list
//					s->files = curr->next;
//				}
//				else {
//					prev->next = curr->next;
//				}
//				delete curr;
//				cout << "File '" << file << "' deleted successfully from RepoID " << id << endl;
//
//				// Now, remove the entry from the files.csv file
//				removeEntryFromFileCSV(id, file);
//				return;
//			}
//			prev = curr;
//			curr = curr->next;
//		}
//
//		cout << "File '" << file << "' not found in RepoID " << id << endl;
//	}
//	else {
//		cout << "Repository with ID " << id << " not found for this user " << loggedInUser << endl;
//	}
//}
//void RepositoryTree::removeEntryFromFileCSV(int repoID, const string& filename)
//{
//	ifstream inFile("files.csv");
//	if (!inFile.is_open()) {
//		cerr << "Error: Unable to open input file." << endl;
//		return;
//	}
//
//	ofstream outFile("temp.csv");
//	if (!outFile.is_open()) {
//		cerr << "Error: Unable to create output file." << endl;
//		inFile.close(); // Close the input file
//		return;
//	}
//
//	string line;
//
//	// Read each line from files.csv and update the line if needed
//	while (getline(inFile, line))
//	{
//		stringstream ss(line);
//		string id, owner, fileList;
//		getline(ss, id, ',');
//		getline(ss, owner, ',');
//
//		// Read the file list
//		getline(ss, fileList);
//		stringstream fileListStream(fileList);
//		string file;
//		string updatedFileList;
//
//		bool fileFound = false;
//
//		// Parse the list of files, excluding the specified file
//		while (getline(fileListStream, file, ',')) {
//			if (file == filename) {
//				file = " ";
//				break;
//			}
//		}
//	}
//
//	// Close files
//	inFile.close();
//	outFile.close();
//}
////void RepositoryTree::createRepo(string ownername)
////{
////	string reponame;
////	cout << "Enter repo name: ";
////	getline(cin, reponame);
////
////	cout << "Choose mode" << endl;
////	cout << "Enter 1 for public " << endl;
////	cout << "Enter 0 for private" << endl;
////	bool m;
////	cin >> m;
////
////	cin.ignore(); // Clear the newline character
////
////	string discription;
////	cout << "Enter description: ";
////	getline(cin, discription);
////
////	// Create README file
////	ofstream readmeFile(reponame + "_Readme.txt");
////	if (readmeFile.is_open())
////	{
////		readmeFile << discription; // Write description to README file
////		readmeFile.close();
////	}
////	else
////	{
////		cout << "Unable to create README file: " << reponame + "_Readme.txt" << endl;
////		return; // Exit function if unable to create README file
////	}
////
////	FileNode* readme = new FileNode(reponame + "_Readme.txt",discription);
////	RepoNode* newnode = new RepoNode(reponame, ownername, m, readme);
////	newnode->lastRepoID++;
////	newnode->repoID = newnode->lastRepoID;
////
////	ofstream file("RepoID.txt");
////	file << newnode->lastRepoID;
////	file.close();
////
////	newnode->readme->filename = reponame + "_Readme.txt";
////
////	insertRepoInBST(newnode->repoID, ownername, reponame, m, readme);
////}
//
////void RepositoryTree::readFromRpoFile()
////{
////	ifstream file("Repository.csv");
////	if (!file.is_open())
////	{
////		cout << "Unable to open" << endl;
////		return;
////	}
////
////	string line;
////	while (getline(file, line))
////	{
////		stringstream ss(line);
////		string id, owner, repo, m, readme;
////		getline(ss, id, ',');
////		getline(ss, owner, ',');
////		getline(ss, repo, ',');
////		getline(ss, m, ',');
////		getline(ss, readme);
////		ifstream r(readme);
////		string readfile;
////		string lines;
////		while (getline(r, lines))
////		{
////			readfile+= lines;
////		}
////		FileNode* read = new FileNode(readme,readfile);
////		int repoid = convertStringToInt(id);
////		// Check if a node with the same repoID already exists
////		if (searchRepoByID(repoid, root) == NULL)
////		{
////			bool mode = (m == "true");
////			insertforfile(repoid, owner, repo, mode,read);
////		}	
////	}
////	file.close();
////}
//
//
//
//void RepositoryTree::insertforfile(int ID, string ownername, string repo, bool m, FileNode* readme)
//{
//	RepoNode* newnode = new RepoNode(repo, ownername, m, readme);
//	newnode->repoID = ID;
//	if (!root)
//	{
//		root = newnode;
//	}
//	else
//	{
//		RepoNode* curr = root;
//		while (curr != NULL)
//		{
//			if (ID < curr->repoID)
//			{
//				if (curr->left == NULL)
//				{
//					curr->left = newnode;
//					return;
//				}
//				else
//					curr = curr->left;
//			}
//			else if (ID > curr->repoID)
//			{
//				if (curr->right == NULL)
//				{
//					curr->right = newnode;
//					return;
//				}
//				else
//					curr = curr->right;
//			}
//			else
//			{
//				cout << "Duplicate value found in tree" << endl;
//			}
//		}
//	}
//}
//
//
//void RepositoryTree::writeInFiles(int repoID, const string& filename,string ownername)
//{
//	ifstream read("files.csv");
//	vector<string> lines;
//	bool idFound = false;
//	string line;
//	while (getline(read, line))
//	{
//		stringstream ss(line);
//		string id;
//		getline(ss, id, ',');
//		// If repoID matches, append filename 
//		if (convertStringToInt(id) == repoID)
//		{
//			line += "," + filename;
//			idFound = true;
//		}
//		lines.push_back(line);
//	}
//	read.close();
//	// If repoID not found, add a new line for it
//	if (!idFound)
//	{
//		lines.push_back(to_string(repoID) +","+ownername+"," + filename);
//	}
//	// Rewrite the entire file with modified lines
//	ofstream file("files.csv", ios::trunc); // Open in truncation mode to clear existing content
//	for (const auto& modifiedLine : lines)
//	{
//		file << modifiedLine << endl;
//	}
//	file.close();
//}
