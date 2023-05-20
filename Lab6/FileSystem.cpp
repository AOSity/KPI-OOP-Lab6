#include "FileSystem.h"

string GetCurrentDate()
{
	chrono::system_clock::time_point now = chrono::system_clock::now();
	time_t currentTime = chrono::system_clock::to_time_t(now);
	tm timeInfo;
	localtime_s(&timeInfo, &currentTime);
	char timeString[20];
	strftime(timeString, sizeof(timeString), "%d.%m.%Y %H:%M:%S", &timeInfo);
	return timeString;
}

/* File */
File::File(const string& fileName, unsigned int size) : Name(fileName), Size(size)
{
	CreationDate = ModificationDate = GetCurrentDate();
	//cout << "File " << Name << " created at " << CreationDate << endl;
}
void File::display(const string& path)
{
	cout << path + "/" + Name << endl;
}
string File::GetName() const { return Name; }
unsigned int File::GetSize() const { return Size; }
string File::GetCreationDate() const { return CreationDate; }
string File::GetModificationDate() const { return ModificationDate; }
File* File::GetCopy() const
{
	File* Copy = new File(Name, Size);
	Copy->CreationDate = CreationDate;
	Copy->ModificationDate = ModificationDate;
	return Copy;
}
void File::SetSize(unsigned int newSize) { Size = newSize; ModificationDate = GetCurrentDate(); }
bool File::isDirectory() { return false; }
bool File::isFile() { return true; }
void File::SetName(const string& newName)
{
	Name = newName;
}
/* Directory */
Directory::Directory(const string& dirName) : Name(dirName)
{
	CreationDate = GetCurrentDate();
	//cout << "Directory " << Name << " created at " << CreationDate << endl;
}
void Directory::addComponent(FileSystemComponent* component) 
{
	components.push_back(component);
	string ModificationDate = component->GetCreationDate();
}
void Directory::display(const string& path) 
{
	cout << path + "/" + Name << endl;
	for (auto component : components) {
		component->display(path + "/" + Name);
	}
}
void Directory::SetSize(unsigned int Size) { FilesSize = Size; ModificationDate = GetCurrentDate(); }
string Directory::GetName() const { return Name; }
unsigned int Directory::GetSize() const { return FilesSize; }
string Directory::GetCreationDate() const { return CreationDate; }
string Directory::GetModificationDate() const { return ModificationDate; }
unsigned int Directory::GetComponentsCount() const { return components.size(); }
unsigned int Directory::GetTotalComponentsCount() const
{
	unsigned int Count = 0;
	for (FileSystemComponent* component : components)
	{
		if (component->isDirectory())
		{
			Count += ((Directory*)component)->GetTotalComponentsCount();
		}
	}
	Count += components.size();
	return Count;
}
Directory* Directory::GetCopy() const
{
	Directory* Copy = new Directory(Name);
	Copy->CreationDate = CreationDate;
	Copy->ModificationDate = ModificationDate;
	Copy->FilesSize = FilesSize;
	for (FileSystemComponent* comp : components)
	{
		Copy->addComponent(comp->GetCopy());
	}
	return Copy;
}
bool Directory::isDirectory() { return true; }
bool Directory::isFile() { return false; }
void Directory::SetName(const string& newName)
{
	Name = newName;
}
void Directory::UpdateSize(Directory* CurrentDir)
{
	FilesSize = 0;
	for (FileSystemComponent* Component : components)
	{
		if (Component->isDirectory())
		{
			((Directory*)Component)->UpdateSize();
		}
		FilesSize += Component->GetSize();
	}
}
/* FileSystem */
FileSystem::FileSystem() { root = new Directory("root"); Paths["/root"] = root; }
FileSystem::~FileSystem() { delete root; }
void FileSystem::DisplayStructure()
{
	root->display();
}
void FileSystem::DisplayStructure(Directory* Directory)
{
	Directory->display();
}
void FileSystem::AddPath(FileSystemComponent* component, const string& path)
{
	string componentPath = path + "/" + component->GetName();

	Paths[componentPath] = component;

	if (typeid(*component) == typeid(Directory))
	{
		for (FileSystemComponent* comp : ((Directory*)component)->components)
		{
			AddPath(comp, componentPath);
		}
	}
}
void FileSystem::Add(FileSystemComponent* component) 
{
	root->addComponent(component);
	AddPath(component, "/root");
}
FileSystemComponent* FileSystem::FindByPath(string path)
{
	if (Paths[path] != 0)
	{
		return Paths[path];
	}
	else
	{
		throw ("Wrong path : " + path);
		return nullptr;
	}
}
FileSystemComponent* FileSystem::Get(string Name, Directory* Dir)
{
	for (FileSystemComponent* component : Dir->components)
	{
		if (component->GetName() == Name)
		{
			return component;
		}
	}

	throw ("Object with name " + Name + " not found in " + Dir->GetName());
	return nullptr;

}
void FileSystem::UpdatePaths()
{
	Paths.clear();
	AddPath(root, "");
}
Directory* FileSystem::CreateDirectory(Directory* TargetedDir, string Name)
{
	Directory* newDir = new Directory(Name);
	TargetedDir->addComponent(newDir);
	UpdatePaths();
	return newDir;
}
void FileSystem::UpdateSizes()
{
	root->UpdateSize();
}
File* FileSystem::CreateFile(Directory* TargetedDir, string Name, unsigned int Size)
{
	File* newFile = new File(Name, Size);
	TargetedDir->addComponent(newFile);
	TargetedDir->SetSize(TargetedDir->GetSize() + Size);
	UpdatePaths();
	return newFile;
}
void FileSystem::Modify(File* TargetedFile, unsigned int Size)
{
	TargetedFile->SetSize(Size);
}
void FileSystem::DeepRemove(FileSystemComponent* TargetedComponent, Directory* CurrentDir)
{
	for (auto iter = CurrentDir->components.begin(); iter != CurrentDir->components.end(); iter++)
	{
		if ((*iter)->isDirectory())
		{
			if (TargetedComponent != *iter)
			{
				DeepRemove(TargetedComponent, (Directory*)*iter);
			}
			else
			{
				CurrentDir->components.erase(iter);
				return;
			}
		}
		else
		{
			if (TargetedComponent == *iter)
			{
				CurrentDir->components.erase(iter);
				return;
			}
		}
	}
}
void FileSystem::Remove(FileSystemComponent* TargetedComponent)
{
	DeepRemove(TargetedComponent, root);
	UpdatePaths();
	UpdateSizes();
}
void FileSystem::Rename(FileSystemComponent* TargetedComponent, const string& newName)
{
	TargetedComponent->SetName(newName);
	UpdatePaths();
}
void FileSystem::Copy(FileSystemComponent* TargetedComponent, Directory* newDir, string newName)
{
	FileSystemComponent* Copy = TargetedComponent->GetCopy();
	if (newName.size() != 0)
	{
		Copy->SetName(newName);
	}
	newDir->addComponent(Copy);
	UpdatePaths();
	UpdateSizes();
}
void FileSystem::Move(FileSystemComponent* TargetedComponent, Directory* TargetedDir)
{
	// move semantics left the chat
	TargetedDir->addComponent(TargetedComponent->GetCopy());
	Remove(TargetedComponent);
	UpdateSizes();
}
vector<FileSystemComponent*> FileSystem::FindByMask(Directory* TargetedDirectory, string Mask)
{
	string ConvertedMask;
	for (char character : Mask)
	{
		if (character == '?')
		{
			ConvertedMask.push_back('\.');
		}
		else if (character == '*')
		{
			ConvertedMask.push_back('\.');
			ConvertedMask.push_back('*');
		}
		else
		{
			ConvertedMask.push_back(character);
		}
	}
	regex Pattern(ConvertedMask);
	vector<FileSystemComponent*> Matches;

	for (FileSystemComponent* Component : TargetedDirectory->components)
	{
		if (regex_match(Component->GetName(), Pattern))
		{
			Matches.push_back(Component);
		}
		if (Component->isDirectory())
		{
			vector<FileSystemComponent*> newMatches = FindByMask((Directory*)Component, Mask);
			for (FileSystemComponent* match : newMatches)
			{
				Matches.push_back(match);
			}
		}
	}
	return Matches;
}
vector<FileSystemComponent*> FileSystem::FindBySize(Directory* TargetedDirectory, unsigned int MinSize, unsigned int MaxSize)
{
	vector<FileSystemComponent*> Matches;

	for (FileSystemComponent* Component : TargetedDirectory->components)
	{
		unsigned int ComponentSize = Component->GetSize();
		if (ComponentSize >= MinSize && ComponentSize <= MaxSize)
		{
			Matches.push_back(Component);
		}
		if (Component->isDirectory())
		{
			vector<FileSystemComponent*> newMatches = FindBySize((Directory*)Component, MinSize, MaxSize);
			for (FileSystemComponent* match : newMatches)
			{
				Matches.push_back(match);
			}
		}
	}
	return Matches;
}
Directory* FileSystem::GetRoot() {
	return root;
}