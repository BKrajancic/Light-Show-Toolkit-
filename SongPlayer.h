#include "CommandDataTypes.cpp"
#include "ProgrammableLight.h"
#include <string>
#include <vector> 
#include <utility> 
#include <map>
#include <iostream>
#include <chrono>
#include <thread>
class SongPlayer
{
    public: 
    void RunCommand(Command item);
    void RunFunction(std::string FunctionToPlay , CommandOperation Operation = CommandOperation::set);
    void WaitMilliseconds(float milliseconds);
    SongPlayer();
    std::vector<ColourListiner*> ListeningLights;
    
    std::map<std::string, std::vector<Command>> MainFile;
    

    std::vector<ProgrammableLight*> GroupChangeEventListiners;
    void AddFunctionToSupportFile(std::string, std::vector<Command>);
    void AddParsedFileToSupportFile(std::map<std::string, std::vector<Command>> ParsedFile);
    private: 
    std::map<std::string, std::vector<Command>> SupportFile;
};