#ifndef ROOM_ASSIGN_H
#define ROOM_ASSIGN_H


#include "roomassign_defs.h"
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <cstdint>
#include <attendees.h>
#include <churches.h>
#include <buildings.h>


using namespace std;



class ROOMASSIGN_EXPORT RoomAssign : public Attendees {

public:

	RoomAssign();
	~RoomAssign();

	int32_t readInputs(const char* church_name, const char *buildings_name, const char *registration_name, int32_t year);
	int32_t preprocessData();

	int32_t assignRooms2Seniors();
	int32_t assignRooms2Babies();
	// family rooms from 1 bed to 4 beds
	int32_t assignRooms2Families();
	int32_t assignRooms2Males();
	int32_t assignRooms2Females();
	int32_t assignRooms2Speakers();
	int32_t assignRooms2Logistics();
	int32_t assignRooms2Choir();
	int32_t assignRooms2ChildcareWorkers();

	std::vector<BuildingRoomList::EURoom*> queryRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num);
	std::vector<BuildingRoomList::EURoom*> queryFamilyRoomList(std::vector<BuildingRoomList::EURoom*> &myroomlist, int32_t num, Registrant* registrant, bool enable_extrabed=false);

protected:
	int32_t familyRoomAssign(std::map<int32_t, std::vector<Registrant*>> &family_lst, std::vector<BuildingRoomList::EURoom*> &roomlist, bool enable_extrabeds=false);
	int32_t IndividualRoomAssign(std::map<int32_t, std::vector<Registrant*>> &individual_lst, BuildingRoomList::SexType stype, std::vector<BuildingRoomList::EURoom*> &roomlist);

};
#endif
// ROOM_ASSIGN_H


