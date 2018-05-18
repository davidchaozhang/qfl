#include "QflReg.h"
#include "churches.h"
#include "buildings.h"
#include "attendees.h"
#include "roomassign.h"

static int room_assignmentTest();
static int usage();
static int parsing(int argc, char**argv);
static int auto_roomassign_2018(int argc, char**argv);


std::string churchname = "D:/users/dzhang/QFL/roomassign/churchlist_20180514_milestone.csv";
std::string brname = "D:/users/dzhang/QFL/roomassign/buildingAndRoom-0514_2018.csv";
std::string filename = "D:/users/dzhang/QFL/reports/report0517_2018_milestone.csv";
std::string sortedname = "D:/users/dzhang/QFL/reports/sorted_report.csv";
std::string filename_new = "D:/users/dzhang/QFL/roomassign/report_new_assignment.csv";
std::string filename_building_distr = "D:/users/dzhang/QFL/roomassign/report_church_buildings.csv";
std::string cellgroups_name = "D:/users/dzhang/QFL/roomassign/cell_groups.csv";
std::string camp_all = "D:/users/dzhang/QFL/roomassign/camp_all.csv";

int main(int argc, char**argv)
{
	int ret = 0;
	parsing(argc, argv);
	ret = auto_roomassign_2018(argc, argv);
	system("PAUSE");
	return ret;
}

int usage()
{

	return 0;
}

int parsing(int argc, char**argv)
{
	char *c;
	while (--argc > 0 && (*++argv)[0] == '-') {
		for (c = argv[0] + 1; *c != '\0'; c++) {
			switch (*c) {
			case 'c': 
				churchname = (++argv)[0];
				argc--;
				break;
			case 'b': 
				brname = (++argv)[0];
				argc--;
				break;
			case 'r': 
				filename = (++argv)[0];
				argc--;
				break;
			case 'n': 
				filename_new = (++argv)[0];
				argc--;
				break;
			case 's':
				sortedname = (++argv)[0];
				argc--;
				break;
			case 'd': 
				filename_building_distr = (++argv)[0];
				argc--;
				break;
			default:
			case 'h':
				usage();
			}
		}
	}
	return 0;
}

int auto_roomassign_2018(int argc, char**argv)
{
	int32_t status = 0;
	int32_t year = 2018;
	RoomAssign ra;
	status = ra.readInputs(churchname.c_str(), brname.c_str(), filename.c_str(), year);
	status = ra.preprocessData1();

	//ra.printEU_for_cellgroup(cellgroups_name.c_str());

	// collect allocated room status
	status = ra.roomAllocationStats();

	// collect lodging people status
	status = ra.lodgePeopleStats(sortedname.c_str());
	ra.trackStatus("0.Begin");

	// take reserved special need rooms
	status = ra.assignRooms2SpecialNeeds();
	ra.printRooms2SpecialNeeds();
	ra.trackStatus("1.SpecialNeeds");

	// take reserved choir rooms
	status = ra.assignRooms2Choir();
	ra.printRooms2Choir();
	ra.trackStatus("2.Choir");

	// take reserved drama team rooms
	status = ra.assignRooms2DramaTeam();
	ra.printRooms2DramaTeam();
	ra.trackStatus("3.Drama");

	// take reserved young child coworkers
	status = ra.assignRooms2ChildcareWorkers();
	ra.printRooms2ChildcareWorkers();
	ra.trackStatus("4.Childcare");

	// take reserved speaker rooms
	status = ra.assignRooms2Speakers();
	ra.printRooms2Speakers();
	ra.trackStatus("5.Speakers");

	status = ra.assignRooms2Recordings();
	ra.printRooms2Recordings();
	ra.trackStatus("6.Recordings");

	// take reserved special need rooms 
	status = ra.assignRooms2Babies();
	ra.printRooms2Babies();
	ra.trackStatus("7.Babies");

	status = ra.assignRooms2Seniors();
	ra.printRooms2Seniors();
	ra.trackStatus("8.Seniors");

	// take reserved family rooms 
	status = ra.assignRooms2Families();
	ra.printRooms2Families();
	ra.trackStatus("9.Families");

	// take reserved individual male rooms
	status = ra.assignRooms2Males();
	ra.printRooms2Males();
	ra.trackStatus("10.Males");

	// take reserved individual female rooms
	status = ra.assignRooms2Females();
	ra.printRooms2Females();
	ra.trackStatus("11.Females");

	ra.extrabed_update();

	// print an updated document with the latest room assignment
	ra.printRoomAssignment(filename_new.c_str());

	// print updated document for attendees of each church distributed per building
	ra.printChurchDistributionPerBuilding(filename_building_distr.c_str());

	// print the entire camp data
	ra.printEU_for_cellgroup(cellgroups_name.c_str());

	return 0;
}


int room_assignmentTest()
{
	int32_t status = 0;
	RoomAssign ra;

	status = ra.readInputs(churchname.c_str(), brname.c_str(), filename.c_str(), 2017);
	status = ra.preprocessData1();

	// collect allocated room status
	status = ra.roomAllocationStats();

	// collect lodging people status
	status = ra.lodgePeopleStats(sortedname.c_str());
	ra.trackStatus("0.Begin");

	// take reserved special need rooms
	status = ra.assignRooms2SpecialNeeds();
	ra.printRooms2SpecialNeeds();
	ra.trackStatus("1.SpecialNeeds");

	// take reserved choir rooms
	status = ra.assignRooms2Choir();
	ra.printRooms2Choir();
	ra.trackStatus("2.Choir");

	// take reserved drama team rooms
	status = ra.assignRooms2DramaTeam();
	ra.printRooms2DramaTeam();
	ra.trackStatus("11.Drama");
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

	ra.extrabed_update();

	// print an updated document with the latest room assignment
	ra.printRoomAssignment(filename_new.c_str());

	// print updated document for attendees of each church distributed per building
	ra.printChurchDistributionPerBuilding(filename_building_distr.c_str());
	return status;
}