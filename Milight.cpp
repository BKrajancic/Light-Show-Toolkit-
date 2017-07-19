#include "CommandDataTypes.cpp"
#include "Milight.h"

#define WIN32_LEAN_AND_MEAN
#include <winsock2.h>

#include <ctime>
#include <iomanip>




    
void Milight::EmitColour(const Command CommandItem , const std::vector<std::pair<const int, Colour>*> ExpectedOutput) {
    CanUseByteForALLGROUPS CanSendAllGroupByte = CheckIfCanUseByteForALLGROUPS(ExpectedOutput);
    UDPPacketSender.InitialiseConnection("10.0.0.65" , 8899);

    const char AllGroupByte = 0x42;

    //COLOUR MUST ALWAYS BE SENT FIRST
    Colour FirstEntryColour = ExpectedOutput[0]->second;
    if (CanSendAllGroupByte == ForBoth) {
        
        //Send Group Byte
        UDPPacketSender.SendHexPackets(AllGroupByte);
        
        //Send Hue
        SendHue( FirstEntryColour, AllGroupByte);
        
        //Individually send brightness 
        SendBrightness( FirstEntryColour, AllGroupByte);
        
    } else if (CanSendAllGroupByte == ForHue) {
        //Send Group Byte
        UDPPacketSender.SendHexPackets(AllGroupByte);
        
        //Send Hue
        SendHue( FirstEntryColour, AllGroupByte);
        
        //Individually send brightness 
        for (std::pair<const int, Colour>* entry : ExpectedOutput ) {
            if ((entry->first < 5) && (entry->first > 0)) {
                char GroupHexByte = GetGroupHexByte(entry->first);
                UDPPacketSender.SendHexPackets(GroupHexByte);
                SendBrightness( (entry->second), GroupHexByte);
            }
        }
        
    }
    else if (CanSendAllGroupByte == ForBrightness){
        
        //Individually send Hue 
        for (std::pair<const int, Colour>* entry : ExpectedOutput ) {
            if ((entry->first < 5) && (entry->first > 0)) {
                char GroupHexByte = GetGroupHexByte(entry->first);
                UDPPacketSender.SendHexPackets(GroupHexByte);
                SendHue( (entry->second), GroupHexByte);
            }
        }
        //Send GroupByte
        UDPPacketSender.SendHexPackets(AllGroupByte);
        
        //Send Brightness
        SendBrightness(FirstEntryColour, AllGroupByte);
    }
}


bool UpdatedCurrentGroup = false;

CanUseByteForALLGROUPS Milight::CheckIfCanUseByteForALLGROUPS (const std::vector<std::pair<const int, Colour>*> Collection) {
    
    bool AllHuesAreSame = false;
    bool AllBrightnessAreSame = false;

    
    int NumberOfUniqueGroups = 0;
    CanUseByteForALLGROUPS ret = ForBoth;
    uint8_t Hue = 0;
    uint8_t Brightness = 0;
    
    bool ContainsGroup1 = false;
    bool ContainsGroup2 = false;
    bool ContainsGroup3 = false;
    bool ContainsGroup4 = false;
    
    for (std::pair<const int, Colour>* item : Collection) {
        if ((item->first == 1) && (ContainsGroup1 == false))  {
            ContainsGroup1 = true;
            NumberOfUniqueGroups++;
        }
        if ((item->first == 2) && (ContainsGroup2 == false))  {
            ContainsGroup2 = true;
            NumberOfUniqueGroups++;
        }
        if ((item->first == 3) && (ContainsGroup3 == false))  {
            ContainsGroup3 = true;
            NumberOfUniqueGroups++;
        }
        if ((item->first == 4) && (ContainsGroup4 == false))  {
            ContainsGroup4 = true;
            NumberOfUniqueGroups++;
        }
        if (NumberOfUniqueGroups == 1 ) {
            Hue = item->second.Hue;
            Brightness = item->second.Brightness;
        }
        if (NumberOfUniqueGroups > 1) {
            if (item->second.Hue != Hue) {
                ret = (CanUseByteForALLGROUPS) (ret & ~ForHue);  //Performs bitwise subtraction
            }
            if (item->second.Brightness != Brightness) {
                ret = (CanUseByteForALLGROUPS) (ret & ~ForBrightness); //Performs bitwise subtraction
                AllBrightnessAreSame = false;
            }
        }
    }
    if ( NumberOfUniqueGroups != 4) {
        ret = ForNeither;
    }

    return ret;
}
void Milight::OnCurrentGroupsUpdate(GroupManager& Manager) {
    
    CurrentGroupBytes.clear();
    //These bools allow us to ensure no doubles occur, and question if all are in
    //We could probably replace this with a list that ensures unique values
    //And then check for "Four" inputs at the end
    UpdatedCurrentGroup = false;

    bool ContainsGroup1 = false;
    bool ContainsGroup2 = false;
    bool ContainsGroup3 = false;
    bool ContainsGroup4 = false;
        

    for (std::pair<const int, Colour>* item : Manager.CurrentlySelectedGroups) {
        if ((item->first == 1) && (ContainsGroup1 == false))  {
            ContainsGroup1 = true;
            CurrentGroupBytes.push_back(GetGroupHexByte(item->first));
        }
        if ((item->first == 2) && (ContainsGroup2 == false))  {
            ContainsGroup2 = true;
            CurrentGroupBytes.push_back(GetGroupHexByte(item->first));
        }
        if ((item->first == 3) && (ContainsGroup3 == false))  {
            ContainsGroup3 = true;
            CurrentGroupBytes.push_back(GetGroupHexByte(item->first));
        }
        if ((item->first == 4) && (ContainsGroup4 == false))  {
            ContainsGroup4 = true;
            CurrentGroupBytes.push_back(GetGroupHexByte(item->first));
        }
    }
    
    if ( ((ContainsGroup1 && ContainsGroup2) && (ContainsGroup3 && ContainsGroup4)) == true) {
        CurrentGroupBytes.clear();
        CurrentGroupBytes.push_back(0x42);
    }
}


char  Milight::GetGroupHexByte(int GroupNumber) {
    //Group 1 ALL ON is 0x45, Group 2 ALL ON is 0x47, and so on

    char GroupHex = 0x43; //If group is 1, this will increment to 1 on execute
    for (int i = 0; i < GroupNumber ; i++) {
            GroupHex++;
            GroupHex++;
    }
    
    return GroupHex;
}

void Milight::SendHue(const Colour OutputColour, const char CurrentGroupByte)
{
    char bytes[3];
    int WhiteThreshhold = 10;
    

    if (OutputColour.Saturation < WhiteThreshhold) {
        UDPPacketSender.SendHexPackets(CurrentGroupByte + 128);
        std::cout << "SETTING WHITE" << std::endl;
    }
    else {
        bytes[0] = 0x40; 
        bytes[1] = 174 - OutputColour.Hue;
        bytes[2] = 0x55;
        UDPPacketSender.SendHexPackets(bytes);
        std::cout << "SETTING COLOUR" << std::endl;
    }
}

void Milight::SendBrightness(const Colour OutputColour, const char CurrentGroupByte)
{
    int BrightnessThreshhold = 10;
    if (OutputColour.Brightness < BrightnessThreshhold)
    {
        if (CurrentGroupByte == 0x42) {
            UDPPacketSender.SendHexPackets(0x41);
        }
        else {
            UDPPacketSender.SendHexPackets(CurrentGroupByte + 1);
        }
        std::cout << "TURNING OFF" << std::endl;
    } 
}

void Milight::SpecificCommand(const Command command){
}



 



