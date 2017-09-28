#ifndef ATTENDEES_H
#define ATTENDEES_H


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

#define Cell_group_size 14

namespace Status {
	const char* const Cancelled = "\"Cancelled\"";
	const char* const KeyReturned = "\"1\"";
	const char* const CheckedIn = "\"1\"";
	const char* const YouthCheckedIn = "\"1\"";
};

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


class QFLLIB_EXPORT Attendees {

public:

	typedef struct Registrant {
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
		int32_t registrtion_fee;
		int32_t key_deposit;
		bool checkin;
		bool youth_checkins;
		bool key_returned;
		int32_t paid;
		std::vector<Registrant*> party_members;
		std::vector<BuildingRoomList::EURoom*> assigned_rooms;
		ChurchList::QFLChurch* from_church;
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
		std::vector<int32_t> attendee_list_a;
		std::vector<Registrant*> attendee_list_b;
		std::vector<BuildingRoomList::EURoom*> assigned_rooms;
		ChurchList::QFLChurch* from_church;
	} Party;

	typedef enum {
		qEU = 'e', // EU
		qCabrini = 'c', // cabrini
		qChild = 'b', // child
		qSenior = 's', // senior
	} Campus;

	typedef struct {
		std::string church_name;
		ChurchList::QFLChurch* from_church;
		std::vector<Registrant* > persons;
		std::vector<Registrant*> christian_list;
		std::vector<Registrant*> non_christian_list;
		std::vector<Registrant*> senior_list;
		std::vector<Registrant*> baby_list;
		std::vector<Registrant*> special_need_list;
		std::map<int32_t, Party> family_list;
		std::vector<Registrant*> male_list;
		std::vector<Registrant*> female_list;
	} Church;

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

	Attendees();
	~Attendees();

	int32_t readChurchList(const char* churchname, int32_t year);
	int32_t readRegistrants(const char *filename);
	int32_t parseAllFields();
	int32_t classifications();
	int32_t refinement();
	int32_t sortAttendeesByChurches();

	void upperCaseConvert(std::vector<int> entries);

	inline 	ChurchList *getChurchHandle() { return &m_church_list; }
	inline BuildingRoomList *getBuildingRoomHandle() { return &m_br_list; }

	std::string intToString(int i);
	std::string getCurTime();

	std::vector<std::vector<std::string>> m_data;
	std::vector<Registrant> m_registrants;
	std::map<int32_t, Registrant*> m_person_info; // person id
	std::map<int32_t, std::vector<Registrant*>> m_family_info; // party id
	std::map<std::string, Church> m_attendee_list_byChurch;

	std::map<int32_t, std::vector<Registrant*>> m_male_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_female_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_child_leader_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_choir_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_recording_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_speaker_list;// party id

	std::map<int32_t, std::vector<Registrant*>> m_senior_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_baby_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_special_need_list;// party id
	std::map<int32_t, std::vector<Registrant*>> m_note_pairup_list;// party id

	std::map<int32_t, Party> m_assigned_family_list;
	std::map<int32_t, Party> m_assigned_family_mail_list;
	std::map<int32_t, Party> m_assigned_family_female_list;
	std::map<int32_t, Party> m_assigned_male_list;
	std::map<int32_t, Party> m_assigned_female_list;
	//std::map<std::string, Party> m_assigned_list_by_church; // church name, party
	std::map<std::string, Church> m_assigned_list_by_church; // church name, church
	std::map<std::string, Party> m_assigned_list_by_building; // building name, party

	// statistics
	std::vector<int32_t> m_christian_list;
	std::vector<int32_t> m_non_christian_list;
	std::vector<int32_t> m_youth_stays_with_parent_list;
	std::vector<int32_t> m_youth_camp_list;
	std::vector<int32_t> m_youth_leader_list;
	std::vector<int32_t> m_logistics_list;
	std::vector<int32_t> m_rccc_xiangyin_list;
	std::vector<int32_t> m_rccc_sheqin_list;
	

	std::vector<int32_t> m_ride_list;
	std::vector<int32_t> m_need_ride_list;
	std::vector<int32_t> m_commute_list;

	std::vector<int32_t> m_EU_list;
	std::map<std::string, std::vector<CellGroup>> m_cell_groups;

	ChurchList m_church_list;
	BuildingRoomList m_br_list;
	int32_t m_cellid;
	int32_t m_cancelled;
	int32_t m_uncertain;
	ChristianStats m_allchurch;
};
#endif
// ATTENDEES_H

