#ifndef PROGRAMMABLE_ASYNC
#define PROGRAMMABLE_ASYNC
#include <thread>
#include <queue>
#include <iostream>
#include <string>
#include <vector>
#include <atomic>
#include <utility>
#include <map>
#include <memory>
#include "Command.h"
#include "Colour.h"
#include "Colour_Combiner.cpp"
#include "ProgrammableLight.h"
#include <utility>
#include "LIGHT_COMMAND_TYPE.h"
#include "Light_Command_Queue.h"

template <class T, class... _Args>
class ProgrammableLight_Async : public ProgrammableLight
{
public:
	ProgrammableLight_Async(_Args &&... __args)
		: light(std::make_unique<T>(std::forward<_Args>(__args)...)),
		  flag(true),
		  thread(&ProgrammableLight_Async::loop, this)

	{
		thread.detach();
		std::cout << "Initiated loop" << std::endl;
	}

	void EmitColour(const Command command, const std::map<int, Colour_Combiner> CurrentGroups) override
	{
		queue.push(command, CurrentGroups, LIGHT_COMMAND_TYPE::EMIT_COLOUR);
	};

	void OnCurrentGroupsUpdate(const Command command, const std::map<int, Colour_Combiner> CurrentGroups) override
	{
		queue.push(command, CurrentGroups, LIGHT_COMMAND_TYPE::ON_CURRENT_GROUPS_UPDATE);
	};

	void SpecificCommand(const Command command, const std::map<int, Colour_Combiner> CurrentGroups) override
	{
		queue.push(command, CurrentGroups, LIGHT_COMMAND_TYPE::SPECIFIC_COMMAND);
	};

	void loop()
	{
		while (flag.load())
		{
			auto opt = queue.pop();
			if (opt.has_value())
			{
				auto item = opt.value();

				switch (item.light_command)
				{
				case LIGHT_COMMAND_TYPE::EMIT_COLOUR:
				{
					light->EmitColour(item.command, item.groups);
					break;
				}
				case LIGHT_COMMAND_TYPE::ON_CURRENT_GROUPS_UPDATE:
				{
					light->OnCurrentGroupsUpdate(item.command, item.groups);
					break;
				}
				case LIGHT_COMMAND_TYPE::SPECIFIC_COMMAND:
				{
					light->SpecificCommand(item.command, item.groups);
					break;
				}
				}
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(100));
			}
		}
		std::cout << "Exited loop for async programmable light" << std::endl;
	}

	void OnStart() override
	{
		light->OnStart();
		flag.store(true);
		thread = std::thread(&ProgrammableLight_Async::loop, this);
		thread.detach();
	}

	void OnEnd() override
	{
		light->OnEnd();
		queue.clear();
		flag.store(false);
	}

	~ProgrammableLight_Async()
	{
		flag.store(false); //Cancel thread
	}

private:
	std::unique_ptr<ProgrammableLight> light;
	Light_Command_Queue queue;
	std::atomic<bool> flag;
	std::thread thread;
};

#endif
