#ifndef SONGPLAYERDEF
#define SONGPLAYERDEF

#include "CommandDataTypes.cpp"
#include "ProgrammableLight.h"
#include "FileParserHandler.h"

#include <string>
#include <vector> 
#include <utility> 
#include <map>
#include <iostream>
#include <chrono>
#include <thread>
#include "GroupManager.h"
class SongPlayer
{
    public: 
	SongPlayer(GroupManager* manager);
	GroupManager* Manager;
    void RunCommand(Command item);

    FileParserHandler Parser;
    
    void WaitMilliseconds(int milliseconds);
    
    std::map<std::string, std::vector<Command>> MainFile;
    
    std::vector<ProgrammableLight*> GroupChangeEventListiners;
    
    void AddFunctionToSupportFile(std::string, std::vector<Command>);
    void AddParsedFileToSupportFile(std::map<std::string, std::vector<Command>> ParsedFile);
    void StartPlaying(std::string SongToPlay, std::string FunctionToPlay = "Play");
    
    bool PlaySong(std::string SongToPlay);
    void StopSong();

    void LoadMainFile(std::string FileName);
    void AddSupportFile(std::string FileName); 

    private: 
    std::map<std::string, std::vector<Command>> SupportFile;
    void RunFunction(std::string FunctionToPlay , CommandOperation Operation = CommandOperation::set);
};

#endif