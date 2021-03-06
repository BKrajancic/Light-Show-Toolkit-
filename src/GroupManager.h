#ifndef GROUPMANAGER_01
#define GROUPMANAGER_01
#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <memory>
#include <map>
#include "Command.h"
#include "Colour.h"
#include "ProgrammableLight.h"
#include "Colour_Combiner.cpp"
#include "Tick_Listener.h"
#include <set>
#include "ProgrammableLight_Async.h"
#include "ConsoleLight.h"

#ifdef MILIGHT_ENABLED
#include "Milight.h"
#endif

#ifdef __arm__
#define RPI
#define RaspberryPi
#include "BinaryLight.h"
#include "BL_433.h"
#include "RF24_Sender.h"
#endif
namespace LIGHT_TYPES
{
enum LIGHT_TYPES
{
  MILIGHT_L,
  RF24_SENDER,
  BINARY_LIGHT,
  CONSOLE
};
}

class GroupManager : public Tick_Listener
{
public:
  GroupManager()
  {
#ifdef __arm__
#warning Injecting GPIO based lights into program, sudo will be needed to run
    AddLight<BinaryLight<BL_433>>(14, 0);
    AddLight<RF24_Sender>(MILIGHT_VERSION::V5);
#endif

    AddLight<ConsoleLight>("First", 1);
    //AddLight<SDL_Light>();
    //AddTickListener(sdl_window);
    //AddTickListener<SDL_Light>(); //Used to be shared with the light listener.
  };

  void SetGroups(const int Group, const Command CommandItem);
  void AddToCurrentGroups(const int GroupToAdd);
  void UpdateColour(const Colour OutputColour, const Command item);
  void SpecificCommand(const Command command);
  void On_Tick();

  template <class T>
  void AddTickListener()
  {
    TickListeners.push_back(std::make_unique<T>());
  }

  template <class T, class... _Args>
  void AddLight(_Args &&... __args)
  {
    std::unique_ptr<ProgrammableLight> make = std::make_unique<ProgrammableLight_Async<T, _Args...>>(std::forward<_Args>(__args)...);

    ListeningLights.push_back(std::move(make));
  }

  void OnStart()
  {
    for (auto &light : ListeningLights)
    {
      light->OnStart();
    }
  }

  void OnEnd()
  {
    for (auto &light : ListeningLights)
    {
      light->OnEnd();
    }
  }

private:
  std::map<int, Colour_Combiner> AllGroups;
  std::set<int> CurrentlySelectedGroups;
  Colour_Combiner CurrentSelectedColour;

  std::vector<std::unique_ptr<ProgrammableLight>> ListeningLights;
  std::vector<std::unique_ptr<Tick_Listener>> TickListeners;
};
#endif
