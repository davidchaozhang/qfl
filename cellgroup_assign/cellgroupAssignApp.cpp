#include "QflReg.h"
#include "churches.h"

static int cellgroupSortByChurch();
static int cellgroup_extern_read();
static int rccc_cellgroup_assignment();
static int cellgroup_printouts();

int32_t year = 2017;
const std::string filename = "D:/users/dzhang/church/rccc/QFL2017/data/registration_reports/report1495684923079_registration_0524.csv";
std::string churchname = "D:/users/dzhang/church/rccc/QFL2017/data/churchlist/churchlist_20170523.csv";
std::string rccc_statistics = "D:/users/dzhang/church/rccc/QFL2017/data/functions/rccc_statistics.csv";

static QflReg gQflReg;

int main()
{
	int ret = 0;
	cellgroupSortByChurch();
	rccc_cellgroup_assignment();

	system("PAUSE");
	return ret;
}

int cellgroupSortByChurch()
{
	gQflReg.readChurchList(churchname.c_str(), year);
	gQflReg.readRegistrants(filename.c_str());
	gQflReg.parseAllFields();
	gQflReg.sortAttendeesByChurches();
	return 0;
}

/*** rccc cell group assignment rules
1. exclude recording, speakers, leaders, finance, ushers,
2. exclude little coworkers (12-17 from childcare worker list)
3. single out: Sheqing, Xiangyin, senior group
4. single out: Taiwanese group, student fellowship,
5. create other groups based on functional group inputs
6. create groups based on zip code

***/

int rccc_cellgroup_assignment()
{
	gQflReg.exclude_rccc_List();
	gQflReg.gen_rccc_functional_groups();
	gQflReg.gen_rccc_zip_groups();
	gQflReg.printOutRCCC_statistics(rccc_statistics.c_str());

	return 0;
}


