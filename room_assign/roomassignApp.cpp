#include "QflReg.h"
#include "churches.h"
#include "buildings.h"
#include "attendees.h"
#include "roomassign.h"
#include "ra.h"

/*
-r D:/users/dzhang/QFL2019/reports/report1556810106900.csv -s D:/users/dzhang/QFL2019/roomassign/sorted_report.csv -c D:/users/dzhang/QFL2019/roomassign/churchlist_20190501.csv -b D:/users/dzhang/QFL2019/roomassign/buildingAndRoom-0411_2019.csv
*/
int main(int argc, char**argv)
{
	int ret = 0;
	parsing2019(argc, argv);
	ret = auto_roomassign_2019(argc, argv);
	system("PAUSE");
	return ret;
}
