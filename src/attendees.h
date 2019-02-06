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

namespace Services {
	const char* const CellGroupLeader = "Cell Group Leader";
	const char* const ServiceChild_2_5 = "Service Child (2-5Yr)";
	const char* const ServiceChild_6_11 = "Service Child (6-11Yr)";
	const char* const QFL_Youth_Counselor = "QFL Youth Counselor";
	const char* const Logistics = "Logistics";
	const char* const QFL_Choir = "QFL Choir";
	const char* const Recording = "Recording";
	const char* const TrafficControl = "Traffic Control";
	const char* const Usher = "Usher";
	const char* const QFL_Drama = "QFL Drama";
	const char* const Speaker = "Speaker";
	const char* const Minister = "Minister";
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

namespace GradeGroup {
	const char* const G0[] = { "Toddler", "Tykes", "PreK", "KinderGarten" };
	const char* const G1[] = { "1st Grade", "2nd Grade", "3rd Grade", "4th Grade", "5th Grade"};
	const char* const G2[] = { "6th Grade", "7th Grade", "8th Grade", "9th Grade", "10th Grade", "11th Grade", "12th Grade" };
	const char* const G2p[] = { "6th Grade,Stay with Parent", "7th Grade,Stay with Parent", "8th Grade,Stay with Parent", 
		"9th Grade,Stay with Parent", "10th Grade,Stay with Parent", "11th Grade,Stay with Parent", "12th Grade,Stay with Parent" };
	const char* const G3 = "College";
	const char* const G4 = "Post College";
	const char* const G5 = "";
};

namespace PartyType {
	const char* const qFamily = "Family";
	const char* const qIndividual = "Individual";
};

namespace BondType {
	const char* const shareRoom = "SRM";
	const char* const shareBathRoom = "SBA";
	const char* const stayLevel = "SLV";
	const char* const staySect = "SST";
	const char* const stayBuilding = "SBD";
	const char* const SpecialNeeded = "SPN";
}

typedef enum {
	qEU = 'e', // EU
	qCabrini = 'c', // cabrini
	qChild = 'b', // child
	qSenior = 's', // senior
	qNotSure = ' ' // not sure
} Campus;

class QFLLIB_EXPORT Attendees {

public:

	typedef struct EULodgePeople {
		int32_t family_seniors;
		int32_t family_babies;
		int32_t family_speakers_recordings;
		int32_t family_choir;
		int32_t family_drama;
		int32_t person_childcare_workers;
		int32_t families;
		int32_t single_males;
		int32_t single_females;
		int32_t special_needs;
	} EULodgePeople;

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
		std::string church_cname;
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
		bool need_key;
		int32_t key_deposit;
		bool eu_checkin;
		bool youth_checkin;
		bool youth_camp_flag;
		bool key_returned;
		bool paid;
		bool temp_flag;
		std::string bond_type;
		std::vector<int32_t> bond_parties;
		std::string qrcode;
		std::vector<Registrant*> party_members;
		std::vector<Registrant*> room_shared;
		std::vector<Registrant*> bathroom_shared;
		BuildingRoomList::EURoom* assigned_room;
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
		std::map<int32_t, std::vector<Registrant*>> attendee_list_b;
		std::vector<BuildingRoomList::EURoom*> assigned_rooms;
		ChurchList::QFLChurch* from_church;
	} Party;

	typedef struct {
		std::string church_name;
		ChurchList::QFLChurch* from_church;
		// list of selected individual registrants in each church. 
		std::vector<Registrant* > persons;
		std::vector<Registrant*> christian_list;
		std::vector<Registrant*> non_christian_list;
/**** 
		The following lists are exclusive. 
		For example, if the person is in the senior list, then the person shall not be listed in any others.
		senior list, baby list, special need list are family oriented. 
		male and female lists are party id based. Each entry may contain more than 1 person.
		The following church attendee lists do not contains youth leaders, speakers, recordings, choir, etc. 
*****/
		// list of seniors, including their family members (this is mainly used for room assignment)
		std::vector<Registrant*> senior_list;
		// list of babies, including their family members
		std::vector<Registrant*> baby_list;
		// list of special need people, including their family members
		std::vector<Registrant*> special_need_list;
		// family list based on party id
		std::map<int32_t, Party> family_list; // party id
		// male only list based on party id
		std::map<int32_t, std::vector<Registrant*>> male_list; // party id
		// female only list based on party id
		std::map<int32_t, std::vector<Registrant*>> female_list; // party id
	} Church;

	typedef struct {
		BuildingRoomList brlist;
		std::string building_name;
		std::vector<Registrant* > persons;
		std::vector<Registrant*> christian_list;
		std::vector<Registrant*> non_christian_list;
		std::vector<Registrant*> senior_list;
		std::vector<Registrant*> baby_list;
		std::vector<Registrant*> special_need_list;
		std::map<int32_t, Party> family_list;
		std::map<int32_t, std::vector<Registrant*>> male_list;
		std::map<int32_t, std::vector<Registrant*>> female_list;
	} Building;

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

	typedef enum {
		fByState = 0,
		fByZip = 1,
		fByName = 2,
		fByChurchId = 3,
		fByChurchCode = 4,
		fByRank = 5,
	}SortChurchList;

	typedef struct Roster {
		int32_t num_eu;
		int32_t num_cabrini;
	} Roster;

	Attendees();
	~Attendees();

	int32_t readChurchList(const char* churchname, SortChurchList sortmethod, int32_t year);
	int32_t readRegistrants(const char *filename);
	int32_t readBuildingRooms(const char *filename);
	int32_t parseAllFields(bool disable_old_assignment_flag=true);
	int32_t camper_christians_statistics();
	int32_t removeNoShowRegistrants(); // this is the function for post-camp processing

	int32_t separateEU_CabriniCampusByRoom();
	int32_t separateEU_CabriniCampusByAttendence();
	int32_t classifications1();

	int32_t classifications();
	int32_t refinement();
	int32_t sortAttendeesByChurches();

	int32_t age_distributions();
	int32_t church_distributions();
	int32_t eu_room_distribution();

	ChurchList::QFLChurch* getChurch(int32_t person_id);
	BuildingRoomList::EURoom *getRoom(int32_t person_id);
	Registrant *getRegistrant(int32_t person_id);

	void upperCaseConvert(std::vector<int> entries);

	inline 	ChurchList *getChurchHandle() { return &m_church_list; }
	inline BuildingRoomList *getBuildingRoomHandle() { return &m_br_list; }

	std::string intToString(int i);
	std::string getCurTime();
	void gradeMapping();

	protected:
	//! raw input data
	std::vector<std::vector<std::string>> m_data;
	//! parsed input data (EU + Cabrini, lodged and commute)
	std::vector<Registrant> m_registrants; 
	//! EU and Cabrini, excludes cancelled and uncertain
	std::map<int32_t, Registrant*> m_person_info; // person id
	//! EU and Cabrini, excludes cancelled and uncertain
	std::map<int32_t, std::vector<Registrant*>> m_party_info; // party id
	std::map<std::string, Church> m_attendee_list_byChurch;

	std::map<int32_t, Registrant*> m_EU_person_info; // person id
	 //! EU and Cabrini, excludes cancelled and uncertain
	std::map<int32_t, std::vector<Registrant*>> m_EU_party_info; // party id

	// m_person_info or m_party_info is divided into the following lists filtered by EU:
	std::map<int32_t, std::vector<Registrant*>> m_family_info; // party id
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
	std::map<int32_t, std::vector<Registrant*>> m_drama_list;// party id
	std::map<std::string, int32_t> m_grade_map;

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

	std::vector<Registrant*> m_Cabrini_list;
	std::vector<Registrant*> m_EU_list;
	std::map<std::string, std::vector<CellGroup>> m_cell_groups;

	ChurchList m_church_list;
	BuildingRoomList m_br_list;
	int32_t m_cellid;
	int32_t m_cancelled;
	int32_t m_uncertain;
	int32_t m_checkedin;
	ChristianStats m_allchurch;

	EULodgePeople m_eu_lodge_people;
};
#endif
// ATTENDEES_H


