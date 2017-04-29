#ifndef _BUILDINGS_H_
#define _BUILDINGS_H_

#include "qflLib_defs.h"
#include <string> /*for size_t*/
#include <cstdint>
#include <vector>
#include <opencv2/opencv.hpp>

namespace RoomTypes {
	const char* const qFamily_Private = "Family Private";
	const char* const qMale = "Male";
	const char* const qFemale = "Female";
	const char* const qNo_Bath = "No Bath";
};

namespace RoomStatus {
	const char* const qAvailable = "Available";
	const char* const qInactive = "Inactive";
	const char* const qReservedGE = "Reserved-GE";
	const char* const qReservedSK = "Reserved-SK";
	const char* const qReservedSP = "Reserved-SP";
	const char* const qReservedCH = "Reserved-CH";
	const char* const qFullyAssigned = "Fully Assigned";
	const char* const qPartiallyAssigned = "Partially Assigned";
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


class QFLLIB_EXPORT BuildingRoomList {
public:

	typedef struct {
		int32_t room_id;
		std::string room;
		std::string room_type;
		std::string room_status;
		int32_t capacity;
		int32_t bed_asigned;
		int32_t extra;
		int32_t bath_distance;
		int32_t room_conditions;
		int32_t score;
	} EURoom;

	typedef struct {
		std::string direction;
		int32_t level;
		std::vector<EURoom> rooms;
		int32_t sect_beds;
		int32_t sect_rooms;
	} EUSection;

	typedef struct {
		int32_t build_no;
		std::string building_name;
		int32_t building_distance;
		bool ac;
		bool fridge;
		bool elevator;
		int32_t total_rooms;
		int32_t total_beds;
		std::vector<EUSection> sects;
		int32_t building_beds;
		int32_t building_rooms;
	} EUBuilding;


	BuildingRoomList();
	~BuildingRoomList();

	int readInBuildingList(const char * building_rooms_list);

	int updateAllSections();

	int updateAllRooms();

	int printRoomStats();

	int writeRoomStats(const char* filename);

	inline std::vector<EUBuilding>* getBuilding_rooms_list() { return &m_eu_buildings; }

private:
	int32_t m_total_rooms;
	int32_t m_total_beds;
	std::vector<EUBuilding> m_eu_buildings;
	std::vector<std::vector<std::string>> m_room_array;
	std::vector<int32_t> family_rooms_array;
	std::vector<int32_t> male_rooms_array;
	std::vector<int32_t> female_rooms_array;
	std::vector<int32_t> reserved_rooms_array;

};

#endif //_BUILDINGS_H_
