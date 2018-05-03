#include "QflReg.h"
#include "churches.h"
#include "buildings.h"
#include "attendees.h"
#include "roomassign.h"

static int32_t year = 2018;
std::string churchname = "D:/users/dzhang/QFL/roomassign/churchlist_20180429.csv";
std::string brname = "D:/users/dzhang/QFL/roomassign/buildingAndRoom-0502_2018.csv";
const std::string filename = "D:/users/dzhang/QFL/reports/report1525015569672.csv";
std::string brname_updates = "D:/users/dzhang/QFL/roomassign/buildingAndRoom-new.csv";

static int process_qfl_registrants();
static int building_listTest();
static int church_listTest();
static int attendee_listTest();

int main()
{
	int ret = 0;
	//ret = attendee_listTest();
	ret = building_listTest();
	//ret = church_listTest();
	//ret = process_qfl_registrants();
	system("PAUSE");
	return ret;
}


static int process_qfl_registrants()
{
	std::string allchurch_dir = "D:/users/dzhang/QFL/statistics/all_church";
	std::string allchurch_EU_dir = "D:/users/dzhang/QFL/statistics/EU";
	std::string allchurch_room_assign_dir = "D:/users/dzhang/QFL/statistics/room_assigns";
	std::string statistics_file = "D:/users/dzhang/QFL/statistics/qfl_statistics.csv";
	std::string eu_report = "D:/users/dzhang/QFL/statistics/EU/eu_report.csv";
	std::string cabrini_report = "D:/users/dzhang/QFL/statistics/Cabrini/cabrini_report.csv";
	std::string childcare_2_5yr_name = "D:/users/dzhang/QFL/statistics/childcare_coworkers_2_5yr.csv";
	std::string childcare_6_11yr_name = "D:/users/dzhang/QFL/statistics/childcare_coworkers_6_11yr.csv";
	std::string logistics_name = "D:/users/dzhang/QFL/statistics/logistics.csv";
	std::string youth_name = "D:/users/dzhang/QFL/statistics/youthcamp.csv";
	std::string doubtful_list = "D:/users/dzhang/QFL/statistics/doubtful.csv";
	std::string rides_list = "D:/users/dzhang/QFL/statistics/rideslist.csv";

	std::string rcccname = "D:/users/dzhang/QFL/statistics/rccc.csv";
	std::string rccc_functions = "D:/users/dzhang/QFL/statistics";
	std::string financial_report = "D:/users/dzhang/QFL/statistics/financial/financial.csv";
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
	clst.readInChurchList(churchname.c_str());
	clst.sortbyState();
	return 0;
}

int building_listTest()
{
	int i;

	BuildingRoomList bdlst;
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
	Attendees qfl_attendees;
	qfl_attendees.readChurchList(churchname.c_str(), Attendees::fByRank, year);
	qfl_attendees.readBuildingRooms(brname.c_str());
	qfl_attendees.readRegistrants(filename.c_str());
	qfl_attendees.parseAllFields();
	qfl_attendees.classifications();
	qfl_attendees.refinement();
	qfl_attendees.sortAttendeesByChurches();
	return 0;
}

