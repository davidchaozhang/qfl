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


using namespace std;

namespace NeedRoom {
	const char* const RoomNeeded = "Room Needed";
	const char* const Commute = "Commute";
};

namespace AgeGroup {
	const char* const A1 = "0-1";
	const char* const A2 = "2";
	const char* const A3 = "3";
	const char* const A4_5 = "4-5";
	const char* const A6_11 = "6-11";
	const char* const A12_14 = "12-14";
	const char* const A15_17 = "15-17";
	const char* const A18_25 = "18-25";
	const char* const A26_39 = "26-39";
	const char* const A40_55 = "40-55";
	const char* const A56_65 = "56-65";
	const char* const A66_69 = "66-69";
	const char* const A70 = "70+";
};

namespace PartyType {
	const char* const qFamily = "Family";
	const char* const qIndividual = "Individual";
};

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

	QflReg();
	~QflReg();

	int32_t readChurchList(const char* churchname);
	int32_t readRegistrants(const char *filename);
	int32_t parseAllFields();
	int32_t classifications();
	int32_t sortAttendeesByChurches();
	int32_t ageStatistics();
	void printOutStatistics(const char*filename);
	void printOutForChildWorkers(const char*filename);
	void printOutForYouth(const char*filename);
	void printOutForLogistics(const char*filename);
	void printOutEU_RoomNeededList(const char*filename);

	void printRCCCAttendees(const char*filename);
	void printRCCCFunctions(const char *dirname);
	void printLivingStone(const char*filename);
	void printCherryHill(const char*filename);
	void printPhillyTrinity(const char*filename);
	void printPhillyCCC(const char*filename);
	void printSmallChurches();
	void printDoubtfulRegistrants(const char*filename);
	void printRidesList(const char*filename);

	void printAttendeesByAllChurch(const char* filedir);
	void printAttendeesEU(const char* filename);
	void printAttendeesCabrini(const char* filename);
	void printAllChurchesForRoomAssign(const char* filedir);
	void printSpecialNeeds(const char*filedir);

	void upperCaseConvert(std::vector<int> entries);

	inline 	ChurchList *getChurchHandle() { return &m_church_list; }
	inline BuildingRoomList *getBuildingRoomHandle() { return &m_br_list; }


	std::string intToString(int i);
	std::string getCurTime();

	std::vector<std::vector<std::string>> m_data;
	std::vector<Registrant> m_registrants;
	std::map<int32_t, Family> m_family_list;
	std::vector<int32_t> m_male_list;
	std::vector<int32_t> m_female_list;
	std::vector<std::pair<std::string,  std::vector<int32_t>>> m_attendee_list_byChurch;
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

	std::vector<int32_t> m_age_statistics;
	std::vector<int32_t> m_EU_list;

	ChurchList m_church_list;
	BuildingRoomList m_br_list;
};
#endif
// QFLREG_H


