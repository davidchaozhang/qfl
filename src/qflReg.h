#ifndef QFLREG_H
#define QFLREG_H


#include "qflLib_defs.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "churches.h"
#include "buildings.h"
#include "attendees.h"

using namespace std;


class QFLLIB_EXPORT QflReg {

public:

	typedef struct {
		int32_t person_id;
		int32_t party;
		std::string party_type;
		std::string first_name;
		std::string last_name;
		std::string chinese_name;
		std::string age_group;
		std::string gender;
		std::string grade;
		std::string contact_person;
		std::string church;
		std::string email;
		std::string city;
		std::string state;
		int32_t zip;
		std::string mobile_phone;
		std::string services;
		std::string functional_group;
		bool need_room;
		std::string room;
		std::string building;
		std::string cell_group;
		bool is_christian;
		std::string occupation;
		std::string need_ride;
		std::string offer_ride;
		std::string special_need;
		std::string notes;
		bool cancelled;
		int32_t registration_fee;
		int32_t key_deposit;
		bool checkin;
		bool youth_checkins;
		bool key_returned;
		int32_t paid;
	} Registrant;


	typedef struct {
		int32_t party;
		std::string contact_person;
		std::string church;
		std::string email;
		std::string city;
		std::string state;
		int32_t zip;
		std::string mobile_phone;
		std::vector<int32_t> attendee_list;
	} Family;

	typedef enum {
		qEU = 'e', // EU
		qCabrini = 'c', // cabrini
		qChild = 'b', // child
		qSenior = 's', // senior
	} Campus;

	typedef struct {
		std::string church_name;
		std::map<Campus, std::vector<int32_t >> persons;
	} QFL_Church;

	typedef struct {
		int32_t cell_group_id;
		std::string church;
		std::string functions;
		std::vector<int32_t> christian_list;
		std::vector<int32_t> non_christian_list;
	} CellGroup;

	typedef struct {
		int32_t adult_christians;
		int32_t adult_non_christians;
		int32_t adult_christians_in_cell_group;
		int32_t youth_christians;
		int32_t youth_non_christians;
		int32_t youth_christians_in_cell_group;
		int32_t youth_checkins;
		int32_t youth_paid;
		int32_t children;
		float adult_christian_ratio;
		float youth_christian_ratio;
		float christian_ratio;
		int32_t eu_overnight;
		int32_t eu_commute;
		int32_t eu_checkins;
		int32_t eu_paid;
		int32_t cabrini_novernight;
		int32_t cabrini_commute;
		int32_t checkins;
		int32_t total_paid;
	} ChristianStats;

	QflReg();
	~QflReg();

	int32_t readChurchList(const char* churchname, int32_t year);
	int32_t readRegistrants(const char *filename);
	int32_t parseAllFields();
	int32_t parseAllFields1();
	int32_t classifications();
	int32_t sortAttendeesByChurches();
	int32_t ageStatistics();
	int32_t exclude_rccc_List();
	int32_t gen_rccc_functional_groups();
	int32_t gen_rccc_zip_groups();

	void printOutStatistics(const char*filename);
	void printOutRCCC_statistics(const char*filename);
	void printOutEU_statistics(const char*filename);
	void printOutCabrini_statistics(const char*filename);

	void printOutForChildWorkers_2_5yr(const char*filename);
	void printOutForChildWorkers_6_11yr(const char*filename);

	void printOutForYouth(const char*filename);
	void printOutForLogistics(const char*filename);

	void printRCCCFunctions(const char *dirname);
	void printDoubtfulRegistrants(const char*filename);
	void printRidesList(const char*filename);

	void printAttendeesByAllChurch(const char* filedir);
	void printOutEUListAllChurch_separated(const char*filedir);
	void printOutEUListAllChurchInOne(const char*filedir);
	void printSpecialNeeds(const char*filedir);
	void printFinancialReport(const char *filename);


	void upperCaseConvert(std::vector<int> entries);

	inline 	ChurchList *getChurchHandle() { return &m_church_list; }
	inline BuildingRoomList *getBuildingRoomHandle() { return &m_br_list; }
	QFL_Church *getRCCCLilst();

	std::string intToString(int i);
	std::string getCurTime();

	std::vector<std::vector<std::string>> m_data;
	std::vector<Registrant> m_registrants;
	std::map<int32_t, Registrant> m_person_info;
	std::map<int32_t, Family> m_family_list;
	std::vector<int32_t> m_male_list;
	std::vector<int32_t> m_female_list;
	std::vector<std::pair<std::string, QFL_Church>> m_attendee_list_byChurch;
	std::map<int32_t, std::vector<int32_t>> m_room_building_list;

	std::vector<int32_t> m_christian_list;
	std::vector<int32_t> m_non_christian_list;
	std::vector<int32_t> m_senior_list;
	std::vector<int32_t> m_baby_list;
	std::vector<int32_t> m_special_need_list;

	std::vector<int32_t> m_youth_stays_with_parent_list;
	std::vector<int32_t> m_youth_camp_list;
	std::vector<int32_t> m_youth_leader_list;
	std::vector<int32_t> m_logistics_list;
	std::vector<int32_t> m_choir_list;
	std::vector<int32_t> m_rccc_xiangyin_list;
	std::vector<int32_t> m_rccc_sheqin_list;
	std::vector<int32_t> m_child_leader_list;

	std::vector<int32_t> m_ride_list;
	std::vector<int32_t> m_need_ride_list;
	std::vector<int32_t> m_commute_list;

	std::vector<int32_t> m_age_statistics;
	std::vector<int32_t> m_EU_list;
	std::map<std::string, std::vector<CellGroup>> m_cell_groups;
	CellGroup m_temp_group;

	ChurchList m_church_list;
	BuildingRoomList m_br_list;
	int32_t m_cellid;
	int32_t m_cancelled;
	int32_t m_uncertain;
	ChristianStats m_allchurch;
	ChristianStats m_rccc;
	int32_t m_year;
};
#endif
// QFLREG_H


