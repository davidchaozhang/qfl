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
	const std::string filename_new = "E:/projects/qfl/doc/report_new_assignment.csv";
	const std::string filename_building_distr = "E:/projects/qfl/doc/report_church_buildings.csv";

	int32_t status = 0;
	RoomAssign ra;

	status = ra.readInputs(churchname.c_str(), brname.c_str(), filename.c_str(), 2017);
	status = ra.preprocessData1();

	// collect allocated room status
	status = ra.roomAllocationStats();

	// collect lodging people status
	status = ra.lodgePeopleStats();
	ra.trackStatus("0.Begin");

	// take reserved special need rooms
	status = ra.assignRooms2SpecialNeeds();
	ra.printRooms2SpecialNeeds();
	ra.trackStatus("1.SpecialNeeds");

	// take reserved choir rooms
	status = ra.assignRooms2Choir();
	ra.printRooms2Choir();
	ra.trackStatus("2.Choir");

	// take reserved young child coworkers
	status = ra.assignRooms2ChildcareWorkers();
	ra.printRooms2ChildcareWorkers();
	ra.trackStatus("3.Childcare");

	// take reserved speaker rooms
	status = ra.assignRooms2Speakers();
	ra.printRooms2Speakers();
	ra.trackStatus("4.Speakers");

	status = ra.assignRooms2Recordings();
	ra.printRooms2Recordings();
	ra.trackStatus("5.Recordings");

	// take reserved special need rooms 
	status = ra.assignRooms2Babies();
	ra.printRooms2Babies();
	ra.trackStatus("6.Babies");
	status = ra.assignRooms2Seniors();
	ra.printRooms2Seniors();
	ra.trackStatus("7.Seniors");

	// take reserved family rooms 
	status = ra.assignRooms2Families();
	ra.printRooms2Families();
	ra.trackStatus("8.Families");

	// take reserved individual male rooms
	status = ra.assignRooms2Males();
	ra.printRooms2Males();
	ra.trackStatus("9.Males");

	// take reserved individual female rooms
	status = ra.assignRooms2Females();
	ra.printRooms2Females();
	ra.trackStatus("10.Females");

	// print an updated document with the latest room assignment
	ra.printRoomAssignment(filename_new.c_str());

	// print updated document for attendees of each church distributed per building
	ra.printChurchDistributionPerBuilding(filename_building_distr.c_str());
	return status;
}