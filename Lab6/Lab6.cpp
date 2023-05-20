#include <iostream>
#include <vector>

#include <iomanip>
#include <ctime>
#include <chrono>
#include <thread>

#include "FileSystem.h"

int main() {
	FileSystem fs;
	Directory* root = fs.GetRoot();
	Directory* DirPtr = nullptr;
	File* FilePtr = nullptr;

	// Actions before first display

	DirPtr = fs.CreateDirectory(root, "Studies");
	fs.CreateDirectory(DirPtr, "Lab7");
	DirPtr = fs.CreateDirectory(DirPtr, "Lab6");
	fs.CreateFile(DirPtr, "Lab6.sln", 2048);
	fs.CreateFile(DirPtr, "main.cpp", 1024);
	DirPtr = (Directory*)fs.FindByPath("/root/Studies");
	fs.CreateFile(DirPtr, "TODO.txt", 128);
	DirPtr = (Directory*)fs.FindByPath("/root/Studies/Lab6");
	fs.Copy(DirPtr, root, "Lab6-Copy");

	FilePtr = fs.CreateFile(root, "test.cpp", 64);
	cout << FilePtr->GetName() << " Creation date: " << FilePtr->GetCreationDate() << endl;
	cout << "Wait 1 second" << endl;
	this_thread::sleep_for(chrono::seconds(1));
	FilePtr = fs.CreateFile(root, "notes.txt", 16);
	cout << FilePtr->GetName() << " Creation date: " << FilePtr->GetCreationDate() << endl;

	cout << endl << "First fs structure display:" << endl; 
	fs.DisplayStructure(root); // First display
	cout << endl;

	cout << FilePtr->GetName() << " isDirectory: " << FilePtr->isDirectory() << endl;
	cout << DirPtr->GetName() << " isDirectory: " << DirPtr->isDirectory() << endl;

	try
	{
		cout << fs.FindByPath("/root/nonexistent")->GetName() << endl;
	}
	catch (string msg)
	{
		cout << msg << endl;
	}
	try
	{
		cout << fs.Get("test.cpp", root)->GetName() << endl;
		cout << fs.Get("nonexistent.unknown", root)->GetName() << endl;
	}
	catch (string msg)
	{
		cout << msg << endl;
	}

	FilePtr = (File*)fs.Get("test.cpp", root);
	cout << "CurrentDate: " << GetCurrentDate() << endl;
	cout << "Wait 1 second" << endl;
	this_thread::sleep_for(chrono::seconds(1));
	fs.Modify(FilePtr, 256);
	cout << FilePtr->GetName() << " modified. ModificationDate: " << FilePtr->GetModificationDate() << endl;

	fs.Rename(fs.FindByPath("/root/Lab6-Copy"), "Lab8");
	fs.Rename(fs.FindByPath("/root/Lab8/Lab6.sln"), "Lab8.sln");

	fs.Remove(fs.FindByPath("/root/Studies/Lab6"));

	fs.Move(fs.FindByPath("/root/Lab8"), (Directory*)fs.FindByPath("/root/Studies"));

	cout << endl << "Second fs structure display:" << endl;
	fs.DisplayStructure(root); // Second display
	cout << endl;

	cout << "Structure of Studies Directory:" << endl;
	fs.DisplayStructure((Directory*)fs.FindByPath("/root/Studies"));
	cout << endl;

	/* Test of search by mask */
	string Mask; 
	//Mask = "*.cpp";
	Mask = "*es*";
	//Mask = "Lab?";
	//Mask = "CustomMask" // Can be changed. ? is one any char, * is any number of any chars.
	vector<FileSystemComponent*> result = fs.FindByMask(root, Mask);
	cout << "Matches for mask \"" << Mask << "\": " << endl;
	for (FileSystemComponent* component : result)
	{
		cout << component->GetName() << endl;
	}
	result.clear();

	/* Test of search by size range */
	unsigned int MinSize = 128, MaxSize = 2048; // Min\Max size can be changed.
	result = fs.FindBySize(root, MinSize, MaxSize);
	cout << endl << "All files with MinSize: " << MinSize << " and MaxSize: " << MaxSize << endl;
	for (FileSystemComponent* component : result)
	{
		if (component->isFile())
			cout << component->GetName() << " size: " << component->GetSize() << endl;
	}
	result.clear();

	return 0;
}