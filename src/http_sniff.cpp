#include <iostream>
using namespace std;

#include "http_sniff_session.h"
#include "config.h"
#include "lib-linux/utility.h"

int main(void)
{
    snet::SessionFactoryImp<HTTPSniffSession> sm;
    snet::SessionManager manager(10);

    // listen http port
    if (!manager.StartupServer(&sm, 80))
    {
		return 1;
    }

	while (true)
	{
		timeval timeout = { 1, 0 };
		manager.Poll(timeout);
        Utility::Sleep(10);
	}

    return 0;
}
