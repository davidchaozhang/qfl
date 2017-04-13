#include <liveRead.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>


static void qfl_checkin_help(int retval)
{
	std::cout << "Quest For Life Camp Check in Program" << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "checkin -t: check in test (using test website)" << std::endl;
	std::cout << "checkin: check in (using official website)" << std::endl;
	std::cout << "Press Esc to exit the program" << std::endl;
}

int32_t main(int32_t argc, char** argv) {
	LiveRead lread;
	std::string checkin_code;
	std::string url;
	int this_argc;
	char *this_argv;

	// display check in message
	qfl_checkin_help(argc);

	this_argc = 1;
	this_argv = argv[1];
	if (argc == 1)
		url = lread.getOfficialURL();
	else {
		while ((this_argc < argc) && (this_argv[0] == '-')) {
			switch (this_argv[1]) {
			case 't':
				url = lread.getTestURL();
				break;
			default:
				break;
			} /* switch */

			this_argc++;
			this_argv = argv[this_argc];

		} /* while */
	}

	while (lread.zbar_video_detect() == 0)
	{
		std::string code = lread.getCode();
		if (lread.parse_code(code) != 0) {
			std::cout << code << " is invalid" << std::endl;
			return -1;
		}
		else {
			std::string checkin_code = "name=" + lread.getKeyword();
			lread.checkSalesforce(url, checkin_code);
		}
		_sleep(10);
	}
	std::cout << "Exiting..." << std::endl;
	return 0;
}
