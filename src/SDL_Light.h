#ifndef SDL_LIGHT_H
#define SDL_LIGHT_H
#include "SDL.h"
#include "ProgrammableLight.h"
#include "ProxyMaker.h"
#include "Tick_Listener.h"

class SDL_Light : public ProgrammableLight, public Tick_Listener
{
public:
    SDL_Light();

    void EmitColour(const Command CommandItem, const std::map<int, Colour_Combiner> ExpectedOutput) override;

    void SpecificCommand(const Command command, const std::map<int, Colour_Combiner> CurrentGroups) override{};

    void OnCurrentGroupsUpdate(const Command CommandItem, const std::map<int, Colour_Combiner> CurrentGroups) override{};

    void On_Tick() override;

    void OnStart() override{}; //TODO

    void OnEnd() override{}; //TODO

    static SDL_Renderer *renderer;
    static SDL_Window *MainWindow;

private:
    std::map<std::set<int>, int, cmpBySetSize> proxies;
    std::map<int, Colour_Combiner> groups;
};

#endif // SDL_LIGHT_H