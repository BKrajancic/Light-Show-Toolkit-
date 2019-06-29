#include <string>
#include <vector>
#include <utility>
#include <map>
#include "CommandFactory.h"
#include <iostream>

std::map<std::string, std::vector<Command>> CommandFactory::CreateFunctionHolder(std::map<std::string, std::vector<std::string>> IntermediateFile) const
{

    std::map<std::string, std::vector<Command>> FunctionsToPlay;

    for (auto it = IntermediateFile.begin(); it != IntermediateFile.end(); ++it)
    {
        auto CleanedCommands = CleanupCommands(it->second); //TODO do this at a more appropriate time

        std::vector<Command> Commands;
        for (const auto Entry : CleanedCommands)
        {
            Commands.push_back(ParseCommand(Entry));
        }

        FunctionsToPlay.insert(std::make_pair(RemoveTrailingWhiteSpace(it->first), Commands));
    }
    return FunctionsToPlay;
}

Command CommandFactory::ParseCommand(std::string CommandInput) const
{
    std::pair<CommandType, std::string> ReturningObject;
    std::string GroupIdentifier = "Group";
    std::string value = "";
    CommandType CurrentCommandType;
    CommandOperation CurrentOperation = CommandOperation::set;
    int TimesToExecute = 1;

    if (CommandInput[0] == '+')
    {
        CurrentOperation = CommandOperation::add;
        CommandInput = CommandInput.erase(0, 1);
    }
    if (CommandInput[0] == '-')
    {
        CurrentOperation = CommandOperation::Remove;
        CommandInput = CommandInput.erase(0, 1);
    }

    if ((CommandInput[0] == '[') && (CommandInput.back() == ']'))
    {
        value = CommandInput.erase(0, 1);
        value = value.substr(0, value.size() - 1);
        CurrentCommandType = CommandType::SpecificCommand;
    }
    else if (CommandInput[0] == '#')
    {
        value = CommandInput.erase(0, 1);
        CurrentCommandType = CommandType::ColourChange_RGB;
    }
    else if (CommandInput[0] == '$')
    {
        value = CommandInput.erase(0, 1);
        CurrentCommandType = CommandType::ColourChange_HSV;
    }
    else if (CommandInput.compare(0, GroupIdentifier.size(), GroupIdentifier) == 0)
    {
        value = CommandInput.erase(0, GroupIdentifier.size());
        CurrentCommandType = CommandType::Group;
    }
    else if (isdigit(CommandInput[0]))
    {
        bool ParsedAmountOfTimesToExecute = false;
        value = "";
        for (char c : CommandInput)
        {
            if (ParsedAmountOfTimesToExecute)
            {
                value += c;
            }
            else if (isalpha(c))
            {
                TimesToExecute = std::stoi(value);
                value = c;
                ParsedAmountOfTimesToExecute = true;
            }
            else
            {
                value += c;
            }
        }
        if (ParsedAmountOfTimesToExecute)
        {
            CurrentCommandType = CommandType::FunctionName;
        }
        else
        {
            CurrentCommandType = CommandType::Wait;
        }
    }
    else
    {
        value = CommandInput;
        CurrentCommandType = CommandType::FunctionName;
    }
    return Command(CurrentCommandType, value, CurrentOperation, TimesToExecute);
}

std::vector<std::string> CommandFactory::CleanupCommands(const std::vector<std::string> &StringVector) const
{
    std::vector<std::string> CommandsOnLine;

    for (const std::string &Entry : StringVector)
    {

        std::string CleanedCommand;
        CleanedCommand = RemoveTrailingWhiteSpace(Entry);

        CommandsOnLine.push_back(CleanedCommand);
    }
    return CommandsOnLine;
}

std::string CommandFactory::RemoveTrailingWhiteSpace(const std::string &StringToFix) const
{
    std::vector<std::string> CommandsOnLine;

    bool SplittingStart = true;

    std::string StringWithoutTrailing;
    std::string TempString;

    for (const char c : StringToFix)
    {
        if (IsWhiteSpace(c) && SplittingStart)
        {
            continue;
        }
        else if (IsWhiteSpace(c))
        {
            TempString = TempString + c; //Not added until a non-whitespace character is read, which prevents end of line trailing
        }
        else
        {
            StringWithoutTrailing = StringWithoutTrailing + TempString + c;
            TempString = "";
            SplittingStart = false;
        }
    }

    return StringWithoutTrailing;
}

bool CommandFactory::IsWhiteSpace(const char c) const
{
    if (c == ' ')
    {
        return true;
    }
    else if (c == '\t')
    {
        return true;
    }
    else
    {
        return false;
    }
}
