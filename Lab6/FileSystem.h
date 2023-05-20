#pragma once

#include <string>
#include <map>
#include <vector>

using namespace std;

string GetCurrentDate();

class FileSystemComponent {
public:
	virtual ~FileSystemComponent() {}
	virtual void display(const string& path = "") = 0;
	
	virtual string GetName()const = 0;
	virtual unsigned int GetSize()const = 0;
	virtual string GetCreationDate() const = 0;
	virtual string GetModificationDate() const = 0;
	virtual FileSystemComponent* GetCopy() const = 0;

	virtual void SetName(const string&) = 0;

	virtual bool isDirectory() = 0;
	virtual bool isFile() = 0;
};

class File : public FileSystemComponent {
private:
	string Name;
	string CreationDate;
	unsigned int Size;
	string ModificationDate;
public:
	File(const string& fileName, unsigned int size = 0);
	void display(const string& path) override;

	string GetName() const override;
	unsigned int GetSize() const override;
	string GetCreationDate() const override;
	string GetModificationDate() const override;
	File* GetCopy() const override;

	void SetSize(unsigned int newSize);
	void SetName(const string& newName) override;

	bool isDirectory() override;
	bool isFile() override;
};

class Directory : public FileSystemComponent {
private:
	string Name;
	string CreationDate;
	unsigned int FilesSize;
	string ModificationDate;
public:
	vector<FileSystemComponent*> components;

	Directory(const string& dirName);
	void display(const string& path = "") override;
	void addComponent(FileSystemComponent* component);
	
	string GetName() const override;
	unsigned int GetSize() const override;
	string GetCreationDate() const override;
	string GetModificationDate() const override;
	unsigned int GetComponentsCount() const;
	unsigned int GetTotalComponentsCount() const;
	Directory* GetCopy() const override;

	void SetSize(unsigned int Size);
	void SetName(const string& newName) override;

	bool isDirectory() override;
	bool isFile() override;
	
	void UpdateSize(Directory* CurrentDir = nullptr);
};

class FileSystem {
private:
	Directory* root;
	map<string, FileSystemComponent*> Paths;
public:
	FileSystem();
	~FileSystem();
	void DisplayStructure();
	void DisplayStructure(Directory* Directory);

	File* CreateFile(Directory* TargetedDir, string Name, unsigned int Size = 0);
	Directory* CreateDirectory(Directory* TargetedDir, string Name);

	void Add(FileSystemComponent* component);
	void AddPath(FileSystemComponent* component, const string& path = "");
	
	void UpdatePaths();
	void UpdateSizes();
	
	void Modify(File* TargetedFile, unsigned int Size);
	void DeepRemove(FileSystemComponent* TargetedComponent, Directory* CurrentDir);
	void Remove(FileSystemComponent* TargetedComponent);
	void Rename(FileSystemComponent* TargetedComponent, const string& newName);
	void Copy(FileSystemComponent* TargetedComponent, Directory* newDir, string newName = "");
	void Move(FileSystemComponent* TargetedComponent, Directory* TargetedDir);

	Directory* GetRoot();
	FileSystemComponent* Get(string Name, Directory* Dir);

	FileSystemComponent* FindByPath(string path);
	vector<FileSystemComponent*> FindByMask(Directory* TargetedDirectory, string Mask);
	vector<FileSystemComponent*> FindBySize(Directory* TargetedDirectory, unsigned int MinSize, unsigned int MaxSize);
};