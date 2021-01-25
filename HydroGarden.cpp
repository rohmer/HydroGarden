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
	
	UserInterface ui;
	ui.CreateMainWindow();
	while (1) {
		lv_tick_inc(5);
		lv_task_handler();
		usleep(5000);
	}
}