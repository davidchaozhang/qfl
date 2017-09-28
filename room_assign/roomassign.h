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
	int32_t assignRooms2Families();
	int32_t assignRooms2Males();
	int32_t assignRooms2Females();
	int32_t assignRooms2Speakers();
	int32_t assignRooms2Logistics();
	int32_t assignRooms2Choir();
	int32_t assignRooms2ChildcareWorkers();

private:

};
#endif
// ROOM_ASSIGN_H


