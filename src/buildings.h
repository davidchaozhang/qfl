#ifndef _BUILDINGS_H_
#define _BUILDINGS_H_

#include "qflLib_defs.h"
#include <string> /*for size_t*/
#include <cstdint>
#include <vector>
#include <opencv2/opencv.hpp>

namespace RoomTypes {
	const char* const qFamily_Private = "Family Private";
	const char* const qFamily_Male = "Family Male";
	const char* const qFamily_Female = "Family Female";
	const char* const qMale = "Male";
	const char* const qFemale = "Female";
	const char* const qNo_Bath = "No Bath";
};

namespace RoomStatus {
	const char* const qAvailable = "Available";
	const char* const qInactive = "Inactive";
	const char* const qSharedBathroom = "SharedBathroom";
	const char* const qReservedGE = "Reserved-GE"; // general purpose
	const char* const qReservedSK = "Reserved-SK"; // speakers
	const char* const qReservedSP = "Reserved-SP"; // special need, (baby, senior, handicapped)
	const char* const qReservedCH = "Reserved-CH"; // choir
	const char* const qReservedCC = "Reserved-CC"; // teen coworker for childcare program
	const char* const qFullyAssigned = "Fully Assigned";
	const char* const qPartiallyAssigned = "Partially Assigned";
	const char* const qExtraBed = "Extra Bed";
};

// this is the order of the report
namespace BuildingReport {
	const char* const qBuilding = "Building";
	const char* const qNoBuilding = "No Building";
	const char* const qName = "Name";
	const char* const qDirection = "Direction";
	const char* const qRoom_ID = "Room_ID";
	const char* const qRoom = "Room";
	const char* const qLevel = "Level";
	const char* const qRoomType = "Room Type";
	const char* const qBathroom = "Bathroom";
	const char* const qRoomStatus = "Room Status";
	const char* const qCapacity = "Capacity";
	const char* const qBedAssigned = "Bed Assigned";
	const char* const qExtra = "Extra";
	const char* const qBuildingDistance = "Building Distance";
	const char* const qBathDistance = "Bath Distance";
	const char* const qScore = "Score";
	const char* const qAC = "AC";
	const char* const qFridge = "Fridge";
	const char* const qElevator = "Elevator";
	const char* const qRoomConditions = "Room Conditions";
	const char* const qNote = "Note";
};

namespace BuildingNames {
	const char* const qEagleHall = "Eagle Hall";
	const char* const qGallup = "Gallup";
	const char* const qGoughHall = "Gough Hall";
	const char* const qGuffin = "Guffin";
	const char* const qHainer = "Hainer";
	const char* const qKea = "Kea";
	const char* const qSparrowk = "Sparrowk";
};

namespace BuildingCode {
	const int32_t const pEagleHall = 3;
	const int32_t const pGallup = 1;
	const int32_t const pGoughHall = 0;
	const int32_t const pGuffin = 6;
	const int32_t const pHainer = 4;
	const int32_t const pKea = 5;
	const int32_t const pSparrowk = 2;
};

class QFLLIB_EXPORT BuildingRoomList {
public:

	static const std::string m_buildings[7];
	static const int32_t m_OpLevel[5];

	typedef enum SexType {
		eMix = 0,
		eMale = 1,
		eFemale = 2,
		eNull = -99
	}SexType;

	typedef struct EURoom {
		int32_t room_id;
		std::string room;
		std::string room_type;
		std::string bathroom;
		std::string room_status;
		int32_t capacity;
		int32_t bed_assigned;
		int32_t extra;
		int32_t bath_distance;
		int32_t room_conditions;
		int32_t score;
		SexType stype;
		std::vector<EURoom*> neighbors;
		void *section;
		void *building;
		std::vector<int32_t> persons; // person id list 
		bool considered;
	} EURoom;

	typedef struct {
		std::string name;
		std::string direction;
		int32_t level;
		int32_t sect_number;
		std::vector<EURoom> rooms;
		int32_t sect_beds;
		int32_t sect_rooms;
		void *building;
	} EUSection;

	typedef struct {
		int32_t build_no;
		std::string building_name;
		int32_t building_distance;
		bool ac;
		bool fridge;
		bool elevator;
		std::vector<EUSection> sects;
		int32_t building_beds;
		int32_t building_rooms;
	} EUBuilding;

	typedef enum {
		tFamily_Private = 10,
		tFamily_Male = 11,
		tFamily_Female = 12,
		tMale = 13,
		tFemale = 14,
		tAvailable = 15,
		tReservedGE = 16, // general purpose reservations
		tReservedSK = 17, // reservation for speakers
		tReservedSP = 18, // reservation for special needs (baby, senior, handicapped, etc)
		tReservedCH = 19, // reservation for choir
		tReservedCC = 20, // reserved for teen coworker for childcare program
		tFullyAssigned = 21,
		tPartiallyAssigned = 22,
		tShared_Bathroom = 23,
		tNo_Bath = 24,
		tExtra_beds = 25,
		tError = -99
	} RoomState;

	typedef struct {
		int32_t total_rooms; // total rooms are all rooms including inactive rooms
		int32_t total_beds;  // total beds are all beds including beds in inactive rooms (extra bed is counted separately) 
		int32_t total_family_rooms; // total family rooms excluding inactive rooms
		int32_t total_family_beds;  // total family beds excluding inactive rooms
		int32_t total_male_rooms; // total male rooms excluding inactive rooms
		int32_t total_male_beds; // total male beds excluding inactive rooms
		int32_t total_female_rooms; // total female rooms excluding inactive rooms
		int32_t total_female_beds; // total female beds excluding inactive rooms
		int32_t total_extra_beds; // total extra beds assigned 
		int32_t total_available_rooms; // total available rooms excluding reserved rooms
		int32_t total_available_beds; // total available beds excluding resrved rooms
		int32_t total_reserved_rooms; // reserved rooms
		int32_t total_reserved_beds; // reserved beds
		int32_t total_assigned_rooms; // fully assigned rooms
		int32_t total_assigned_beds; // beds in fully assigned rooms
		int32_t total_partial_assigned_rooms; // partically assigned rooms
		int32_t total_inactive_rooms; // inactive rooms
		int32_t total_inactive_beds; // inactive beds
		int32_t total_shared_family_beds; // rooms that shared beds
	} RoomStats;

	// score = BuildingCode*brBuilding + brLevel*OpLevel + brSect + room_number + bath_dist
	typedef struct RoomMeasure {
		int32_t brBuilding;
		int32_t brLevel;
		int32_t brSect;
		int32_t brRoom;
		int32_t brAC;
		int32_t brCondition;
		int32_t brElevator;
	} RoomMeasure;

	BuildingRoomList();
	~BuildingRoomList();

	int readInBuildingLists(const char * building_rooms_list, char dataformat=',');
	int accumulateRoomInfo();

	int printRoomStats();

	int writeRoomStats(const char* filename);

	std::vector<EURoom*> queryRoomList(RoomState rs);
	std::vector<EURoom*> queryRoomList(const std::string &building_name, RoomState rs);

	std::vector<EURoom*> queryBathSharedRooms();
	std::vector<EURoom*> queryBathSharedRooms(std::string building_name);

	std::vector<EURoom*> query1personRooms(RoomState rs);
	std::vector<EURoom*> query1personRooms(std::string &building_name, RoomState rs);
	std::vector<EURoom*> query2personRooms(RoomState rs);
	std::vector<EURoom*> query2personRooms(std::string &building_name, RoomState rs);
	std::vector<EURoom*> query3personRooms(RoomState rs);
	std::vector<EURoom*> query3personRooms(std::string &building_name, RoomState rs);
	std::vector<EURoom*> query4personRooms(RoomState rs);
	std::vector<EURoom*> query4personRooms(std::string &building_name, RoomState rs);
	std::vector<EURoom*> queryExtraBedRooms(RoomState rs);
	std::vector<EURoom*> queryExtraBedRooms(std::string &building_name, RoomState rs);

	std::vector<EURoom*> queryAvailableRooms();
	std::vector<EURoom*> queryAvailableRooms(std::string building_name);

	std::vector<EURoom*> queryAssignedRooms();
	std::vector<EURoom*> queryAssignedRooms(std::string building_name);

	std::vector<EURoom*> queryReservedGERooms();
	std::vector<EURoom*> queryReservedGERooms(std::string building_name);
	std::vector<EURoom*> queryReservedSKRooms();
	std::vector<EURoom*> queryReservedSKRooms(std::string building_name);
	std::vector<EURoom*> queryReservedSPRooms();
	std::vector<EURoom*> queryReservedSPRooms(std::string building_name);
	std::vector<EURoom*> queryReservedCHRooms();
	std::vector<EURoom*> queryReservedCHRooms(std::string building_name);
	std::vector<EURoom*> queryReservedCCRooms();
	std::vector<EURoom*> queryReservedCCRooms(std::string building_name);

	inline std::vector<EUBuilding>* getBuilding_list() { return &m_eu_buildings; }

	EURoom* getRoomById(int32_t room_id) { return m_room_list_by_id[room_id]; }
	EURoom* getRoomByName(std::string room_name) { return m_room_list_by_roomname[room_name]; }
	std::vector<EURoom*> getRoomByScore(int32_t score) { return m_room_list_by_score[score]; }

protected:

	int readInBuildingList_quotes(const char * building_rooms_list);
	int updateAllSections_quotes();
	int updateAllRooms_quotes();
	int scoreRooms();

	int readInBuildingList_commas(const char * building_rooms_list);
	int updateAllSections(char dataformat=',');
	int updateAllRooms_commas();
	void resetStats(RoomStats &rs);
	int checkRoomType(const std::string rt, const std::string art);
	int checkRoomStatus(const std::string rt, const std::string art);
	EURoom *checkRoomState(EURoom *eur, RoomState rs);

	void findNeighbors();
	void computeRoomStats();
	void makeRoomListPerBuilding();
	std::string getStringNames(RoomState rs);

private:
	int32_t m_total_rooms;
	int32_t m_total_beds;
	int32_t m_total_inactive_rooms;
	std::map<std::string, int32_t> m_buildingCode;
	std::vector<std::vector<std::string>> m_room_array; // based on room name
	std::vector<EUBuilding> m_eu_buildings;
	RoomMeasure m_room_measure;

	std::map<std::string, RoomStats> m_room_stats;
	std::map<int32_t, EURoom*> m_room_list_by_id;
	std::map<std::string, EURoom*> m_room_list_by_roomname;
	std::map<int32_t, std::vector<EURoom*>> m_room_list_by_score;

	/*
	 room_list= family rooms + male rooms + female rooms
	          = available rooms + reserved rooms + fully assigned rooms + partially assigned rooms 
			  = available rooms + reserved (GE, SK, SP, CH) rooms + fully assigned rooms + partially assigned rooms 
	*/
	std::map<std::string, std::vector<EURoom*>> m_room_list_per_building;
	std::map<std::string, std::vector<EURoom*>> m_family_rooms_array;
	std::map<std::string, std::vector<EURoom*>> m_male_rooms_array;
	std::map<std::string, std::vector<EURoom*>> m_female_rooms_array;
	std::map<std::string, std::vector<EURoom*>> m_reserved_rooms_array;
	std::map<std::string, std::vector<EURoom*>> m_available_rooms_array;

	std::map<std::string, std::vector<EURoom*>> m_ReservedGE_rooms;
	std::map<std::string, std::vector<EURoom*>> m_ReservedSK_rooms;
	std::map<std::string, std::vector<EURoom*>> m_ReservedSP_rooms;
	std::map<std::string, std::vector<EURoom*>> m_ReservedCH_rooms;
	std::map<std::string, std::vector<EURoom*>> m_ReservedCC_rooms;
	std::map<std::string, std::vector<EURoom*>> m_FullyAssigned_rooms;
	std::map<std::string, std::vector<EURoom*>> m_PartiallyAssigned;
	std::map<std::string, std::vector<EURoom*>> m_ExtraBeds;

	std::map<std::string, std::vector<EURoom*>> m_1person_rooms;
	std::map<std::string, std::vector<EURoom*>> m_2person_rooms;
	std::map<std::string, std::vector<EURoom*>> m_3person_rooms;
	std::map<std::string, std::vector<EURoom*>> m_4person_rooms;
	std::map<std::string, std::vector<EURoom*>> m_5person_rooms;
};

#endif //_BUILDINGS_H_
