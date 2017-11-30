#include "QflReg.h"
#include "churches.h"
#include "buildings.h"
#include "attendees.h"
#include "roomassign.h"

static int32_t year = 2017;
static int room_assignmentTest();

int main()
{
	int ret;
	ret = room_assignmentTest();
	system("PAUSE");
	return ret;
}


int room_assignmentTest()
{
	std::string churchname = "E:/projects/qfl/doc/churchlist_20170523.csv";
	std::string brname = "E:/projects/qfl/doc/buildingAndRoom-1125_2017.csv";
	const std::string filename = "E:/projects/qfl/doc/report1496368471394_checkout_0601.csv";

	int32_t status = 0;
	RoomAssign ra;

	status = ra.readInputs(churchname.c_str(), brname.c_str(), filename.c_str(), 2017);
	status = ra.preprocessData1();

	// collect allocated room status
	status = ra.roomAllocationStats();

	// collect lodging people status
	status = ra.lodgePeopleStats();
	ra.trackStatus("Begin");

	// take reserved special need rooms
	status = ra.assignRooms2SpecialNeeds();
	ra.printRooms2SpecialNeeds();
	ra.trackStatus("SpecialNeeds");

	// take reserved choir rooms
	status = ra.assignRooms2Choir();
	ra.printRooms2Choir();
	ra.trackStatus("Choir");

	// take reserved young child coworkers
	status = ra.assignRooms2ChildcareWorkers();
	ra.printRooms2ChildcareWorkers();
	ra.trackStatus("Childcare");

	// take reserved speaker rooms
	status = ra.assignRooms2Speakers();
	ra.printRooms2Speakers();
	ra.trackStatus("Speakers");

	status = ra.assignRooms2Recordings();
	ra.printRooms2Recordings();
	ra.trackStatus("Recordings");

	// take reserved special need rooms 
	status = ra.assignRooms2Babies();
	ra.printRooms2Babies();
	ra.trackStatus("Babies");
	status = ra.assignRooms2Seniors();
	ra.printRooms2Seniors();
	ra.trackStatus("Seniors");

	// take reserved family rooms 
	status = ra.assignRooms2Families();
	ra.printRooms2Families();
	ra.trackStatus("Families");

	// take reserved individual male rooms
	status = ra.assignRooms2Males();
	ra.printRooms2Males();
	ra.trackStatus("Males");

	// take reserved individual female rooms
	status = ra.assignRooms2Females();
	ra.printRooms2Females();
	ra.trackStatus("Females");

	return status;
}