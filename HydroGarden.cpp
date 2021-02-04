#include <iostream>
#include <unistd.h>
#include "UI.h"
#include "WebServer.h"
#include "Scheduler.h"
#include <pistache/http.h>
#include <pistache/description.h>
#include <pistache/endpoint.h>

using namespace std;

int main(int argc, char *argv[])
{
 	Port port(8080);
	Address address(Ipv4::any(), port);
	
	WebServer ws(address);
	
	Scheduler *scheduler = Scheduler::GetInstance();
	Settings::GetInstance()->Headless(true);
	if (!Settings::GetInstance()->Headless())
	{
		UserInterface ui;
		ui.CreateMainWindow();	
		while (1)
		{
			lv_tick_inc(5);
			lv_task_handler();
			usleep(5000);
		}
	}
	else
	{
		int a = 0;
		while (a < 50) 
		{
			usleep(1000);
			//a++;			// Remove for valgrind
		}
	}
	
	
	
}