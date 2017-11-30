#include "QflReg.h"
#include "churches.h"
#include "buildings.h"
#include "attendees.h"
#include "roomassign.h"

static int32_t year = 2017;
static int process_qfl_registrants();
static int building_listTest();
static int church_listTest();
static int attendee_listTest();
static int room_assignmentTest();

int main()
{
	int ret = 0;
	ret = room_assignmentTest();
	//ret = attendee_listTest();
	//ret = building_listTest();
	//ret = church_listTest();
	//ret = process_qfl_registrants();
	system("PAUSE");
	return ret;
}


static int process_qfl_registrants()
{
	const std::string filename = "D:/users/dzhang/church/rccc/QFL2017/data/registration_reports/report1496368471394_checkout_0601.csv";
	std::string churchname = "D:/users/dzhang/church/rccc/QFL2017/data/churchlist/churchlist_20170523.csv";
	std::string allchurch_dir = "D:/users/dzhang/church/rccc/QFL2017/data/all_church";
	std::string allchurch_EU_dir = "D:/users/dzhang/church/rccc/QFL2017/data/EU";
	std::string allchurch_room_assign_dir = "D:/users/dzhang/church/rccc/QFL2017/data/room_assigns";
	std::string statistics_file = "D:/users/dzhang/church/rccc/QFL2017/data/qfl_statistics.csv";
	std::string eu_report = "D:/users/dzhang/church/rccc/QFL2017/data/EU/eu_report.csv";
	std::string cabrini_report = "D:/users/dzhang/church/rccc/QFL2017/data/Cabrini/cabrini_report.csv";
	std::string childcare_2_5yr_name = "D:/users/dzhang/church/rccc/QFL2017/data/childcare_coworkers_2_5yr.csv";
	std::string childcare_6_11yr_name = "D:/users/dzhang/church/rccc/QFL2017/data/childcare_coworkers_6_11yr.csv";
	std::string logistics_name = "D:/users/dzhang/church/rccc/QFL2017/data/logistics.csv";
	std::string youth_name = "D:/users/dzhang/church/rccc/QFL2017/data/youthcamp.csv";
	std::string doubtful_list = "D:/users/dzhang/church/rccc/QFL2017/data/doubtful.csv";
	std::string rides_list = "D:/users/dzhang/church/rccc/QFL2017/data/rideslist.csv";

	std::string rcccname = "D:/users/dzhang/church/rccc/QFL2017/data/rccc.csv";
	std::string rccc_functions = "D:/users/dzhang/church/rccc/QFL2017/data";
	std::string financial_report = "D:/users/dzhang/church/rccc/QFL2017/data/financial/financial.csv";
	QflReg qfl_reg;
	qfl_reg.readChurchList(churchname.c_str(), year);
	qfl_reg.readRegistrants(filename.c_str());
	qfl_reg.parseAllFields();
	qfl_reg.classifications();
	qfl_reg.sortAttendeesByChurches();
	qfl_reg.printFinancialReport(financial_report.c_str());
	qfl_reg.printOutEU_statistics(eu_report.c_str());
	qfl_reg.printOutCabrini_statistics(cabrini_report.c_str());
	qfl_reg.ageStatistics();

	qfl_reg.printOutStatistics(statistics_file.c_str());
	qfl_reg.printSpecialNeeds(allchurch_room_assign_dir.c_str());
	qfl_reg.printAttendeesByAllChurch(allchurch_dir.c_str());
//	qfl_reg.printOutEUListAllChurch_separated(allchurch_EU_dir.c_str());
	qfl_reg.printOutEUListAllChurchInOne(allchurch_EU_dir.c_str());

	qfl_reg.printOutForChildWorkers_2_5yr(childcare_2_5yr_name.c_str());
	qfl_reg.printOutForChildWorkers_6_11yr(childcare_6_11yr_name.c_str());
	qfl_reg.printOutForLogistics(logistics_name.c_str());
	qfl_reg.printOutForYouth(youth_name.c_str());

	qfl_reg.printDoubtfulRegistrants(doubtful_list.c_str());
	qfl_reg.printRidesList(rides_list.c_str());

	qfl_reg.printRCCCFunctions(rccc_functions.c_str());


	return 0;
}

int church_listTest()
{
	ChurchList clst;
	std::string churchname = "D:/users/dzhang/church/rccc/QFL2018/building_layout/churchlist_20170523.csv";
	clst.readInChurchList(churchname.c_str());
	clst.sortbyState();
	return 0;
}

int building_listTest()
{
	int i;

	BuildingRoomList bdlst;
	std::string brname = "E:/projects/qfl/doc/buildingAndRoom-update.csv";
	std::string brname_updates = "E:/projects/qfl/doc/buildingAndRoom-new.csv";
	bdlst.readInBuildingLists(brname.c_str(), ',');
	bdlst.accumulateRoomInfo();

	for (i = (int)BuildingRoomList::tFamily_Private; i < (int)BuildingRoomList::tExtra_beds; i++) {
		std::vector<BuildingRoomList::EURoom*> eurlst = bdlst.queryRoomList(BuildingRoomList::RoomState(i));
		std::vector<BuildingRoomList::EURoom*> eubrlst = bdlst.queryRoomList(BuildingNames::qEagleHall, BuildingRoomList::RoomState(i));
	}

	std::vector<BuildingRoomList::EURoom*> eub_shared = bdlst.queryBathSharedRooms();
	std::vector<BuildingRoomList::EURoom*> eubr_shared = bdlst.queryBathSharedRooms(BuildingNames::qEagleHall);
	std::vector<BuildingRoomList::EURoom*> eub_avail = bdlst.queryAvailableRooms();
	std::vector<BuildingRoomList::EURoom*> eubr_avail = bdlst.queryAvailableRooms(BuildingNames::qEagleHall);
	std::vector<BuildingRoomList::EURoom*> eub_assign = bdlst.queryAssignedRooms();
	std::vector<BuildingRoomList::EURoom*> eubr_assign = bdlst.queryAssignedRooms(BuildingNames::qEagleHall);

	std::vector<BuildingRoomList::EURoom*> eub_gerooms = bdlst.queryReservedGERooms();
	std::vector<BuildingRoomList::EURoom*> eubr_gerooms = bdlst.queryReservedGERooms(BuildingNames::qEagleHall);

	bdlst.printRoomStats();
	bdlst.writeUpdatedBuildingRoomDoc(brname_updates.c_str());

	return 0;
}

int attendee_listTest()
{
	std::string churchname = "D:/users/dzhang/church/rccc/QFL2018/churchlist/churchlist_20170523.csv";
	std::string brname = "D:/users/dzhang/church/rccc/QFL2018/building_layout/buildingAndRoom-update.csv";
	const std::string filename = "D:/users/dzhang/church/rccc/QFL2018/attendeelist/report1496368471394_checkout_0601.csv";
	Attendees qfl_attendees;
	qfl_attendees.readChurchList(churchname.c_str(), Attendees::fByRank, 2017);
	qfl_attendees.readBuildingRooms(brname.c_str());
	qfl_attendees.readRegistrants(filename.c_str());
	qfl_attendees.parseAllFields();
	qfl_attendees.classifications();
	qfl_attendees.refinement();
	qfl_attendees.sortAttendeesByChurches();
	//qfl_attendees.classifications();
	//qfl_attendees.sortAttendeesByChurches();
	return 0;
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