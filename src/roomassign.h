#ifndef ROOM_ASSIGN_H
#define ROOM_ASSIGN_H


#include "qflLib_defs.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include "attendees.h"
#include "churches.h"
#include "buildings.h"


using namespace std;



class QFLLIB_EXPORT RoomAssign : public Attendees {

public:
	/*
	total_rooms = available_rooms + general_reserves + rooms_special_needs + rooms_speakers_recordings + rooms_choir + rooms_childcare_workers
	            = rooms_family_private + rooms_family_male + rooms_family_female + rooms_males + rooms_females
	*/
	typedef struct AllocatedRooms {
		int32_t total_rooms;
		int32_t available_rooms;
		int32_t general_reserves;
		int32_t rooms_special_needs;
		int32_t rooms_speakers_recordings;
		int32_t rooms_choir;
		int32_t rooms_childcare_workers;
		int32_t rooms_family_private;
		int32_t rooms_family_male;
		int32_t rooms_family_female;
		int32_t rooms_males;
		int32_t rooms_females;
	} AllocatedRooms;

	/*!
	* TrackData tracks room and attendee assignment status
	*/
	typedef struct TrackData {
		int32_t assigned_room_cnt;
		int32_t unassigned_room_cnt;
		int32_t assigned_party_cnt;
		int32_t unassigned_party_cnt;
		int32_t assigned_attendee_cnt;
		int32_t unassigned_attendee_cnt;
	}TrackData;

	//! ctor
	RoomAssign();
	//! dtor
	~RoomAssign();

	int32_t readInputs(const char* church_name, const char *buildings_name, const char *registration_name, int32_t year);
	int32_t preprocessData();
	int32_t preprocessData1(bool disable_old_assignment_flag=true);

	int32_t trackStatus(const char* op);

	int32_t roomAllocationStats();
	int32_t lodgePeopleStats(const char* filename);

	int32_t assignRooms2SpecialNeeds();
	int32_t assignRooms2Seniors();
	int32_t assignRooms2Babies();
	// family rooms from 1 bed to 4 beds
	int32_t assignRooms2Families();
	int32_t assignRooms2Males();
	int32_t assignRooms2Females();
	int32_t assignRooms2Speakers();
	int32_t assignRooms2Recordings();
	int32_t assignRooms2Choir();
	int32_t assignRooms2ChildcareWorkers();
	int32_t assignRooms2DramaTeam();
	bool extrabed_update();

	std::vector<BuildingRoomList::EURoom*> queryRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num);
	std::vector<BuildingRoomList::EURoom*> queryFamilyRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num, Registrant* registrant, bool enable_extrabed=false);

	/* only attendees lodge on EU campus is printed*/
	bool printRoomAssignment(const char* filename);
	bool printChurchDistributionPerBuilding(const char* filename);
	bool printEU_for_cellgroup(const char* filename);
	int32_t printSortedAttendees(const char* filename, std::map<int32_t, std::vector<Registrant*>> slist, bool print_title=false);
	int32_t printRooms2SpecialNeeds();
	int32_t printRooms2Seniors();
	int32_t printRooms2Babies();
	int32_t printRooms2Speakers();
	int32_t printRooms2Recordings();
	int32_t printRooms2Choir();
	int32_t printRooms2ChildcareWorkers();
	int32_t printRooms2DramaTeam();
	int32_t printRooms2Families();
	int32_t printRooms2Males();
	int32_t printRooms2Females();

protected:
	int32_t familyRoomAssign(std::map<int32_t, std::vector<Registrant*>> &family_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds=false);
	int32_t familyMaleRoomAssign(std::vector<Registrant*> &family_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds = false);
	int32_t familyFemaleRoomAssign(std::vector<Registrant*> &family_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds = false);

	int32_t IndividualRoomAssign(std::map<int32_t, std::vector<Registrant*>> &individual_lst, BuildingRoomList::SexType stype, std::vector<BuildingRoomList::EURoom*> &roomlist);
	int32_t IndividualRoomAssign2(std::map<int32_t, std::vector<Registrant*>> &individual_lst, BuildingRoomList::SexType stype, std::vector<BuildingRoomList::EURoom*> &roomlist);

	std::vector<BuildingRoomList::EURoom* > RoomCapacityMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, Registrant* registrant, bool enable_extrabed);
	std::vector<BuildingRoomList::EURoom* > RoomSpecialMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, Registrant* registrant, bool enable_extrabed);
	std::vector<BuildingRoomList::EURoom* > RoomExtraMatch(std::map<int32_t, std::vector<BuildingRoomList::EURoom*>> &rms, int32_t num, bool enable_extrabed);

private:

	AllocatedRooms m_alloc_rooms;
	std::map<std::string, TrackData> m_track_data;
};
#endif
// ROOM_ASSIGN_H


