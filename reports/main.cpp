#include "QflReg.h"
#include "churches.h"
#include "buildings.h"

static int32_t year = 2017;
static int process_qfl_registrants();
static int church_building_listTest();


int main()
{
	int ret = 0;
	//ret = church_building_listTest();
	ret = process_qfl_registrants();
	system("PAUSE");
	return ret;
}


static int process_qfl_registrants()
{
	const std::string filename = "D:/users/dzhang/church/rccc/QFL2017/data/registration_reports/report1494870746033_registration_0515.csv";
	std::string churchname = "D:/users/dzhang/church/rccc/QFL2017/data/churchlist/churchlist_20170514.csv";
	std::string allchurch_dir = "D:/users/dzhang/church/rccc/QFL2017/data/all_church";
	std::string allchurch_EU_dir = "D:/users/dzhang/church/rccc/QFL2017/data/EU";
	std::string allchurch_room_assign_dir = "D:/users/dzhang/church/rccc/QFL2017/data/room_assigns";
	std::string statistics_file = "D:/users/dzhang/church/rccc/QFL2017/data/qfl_statistics.csv";

	std::string childcare_2_5yr_name = "D:/users/dzhang/church/rccc/QFL2017/data/childcare_coworkers_2_5yr.csv";
	std::string childcare_6_11yr_name = "D:/users/dzhang/church/rccc/QFL2017/data/childcare_coworkers_6_11yr.csv";
	std::string logistics_name = "D:/users/dzhang/church/rccc/QFL2017/data/logistics.csv";
	std::string youth_name = "D:/users/dzhang/church/rccc/QFL2017/data/youthcamp.csv";
	std::string doubtful_list = "D:/users/dzhang/church/rccc/QFL2017/data/doubtful.csv";
	std::string rides_list = "D:/users/dzhang/church/rccc/QFL2017/data/rideslist.csv";

	std::string rcccname = "D:/users/dzhang/church/rccc/QFL2017/data/rccc.csv";
	std::string rccc_functions = "D:/users/dzhang/church/rccc/QFL2017/data";

	QflReg qfl_reg;
	qfl_reg.readChurchList(churchname.c_str(), year);
	qfl_reg.readRegistrants(filename.c_str());
	qfl_reg.parseAllFields();
	qfl_reg.classifications();
	qfl_reg.sortAttendeesByChurches();
	qfl_reg.ageStatistics();

	qfl_reg.printOutStatistics(statistics_file.c_str());
	qfl_reg.printSpecialNeeds(allchurch_room_assign_dir.c_str());
	qfl_reg.printAttendeesByAllChurch(allchurch_dir.c_str());
	qfl_reg.printOutEUListAllChurch_separated(allchurch_EU_dir.c_str());
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

int church_building_listTest()
{
	ChurchList clst;
	std::string churchname = "D:/users/dzhang/church/rccc/QFL2017/data/report1493313508803_churchlist.csv";
	clst.readInChurchList(churchname.c_str());
	clst.sortbyState();

	BuildingRoomList bdlst;
	std::string brname = "D:/users/dzhang/church/rccc/QFL2017/data/report1493313291455_room_building.csv";
	bdlst.readInBuildingList(brname.c_str());
	bdlst.updateAllSections();
	bdlst.updateAllRooms();
	bdlst.printRoomStats();
	return 0;
}