#include "defs.h"

//initialize each of the house's fields
void initHouse(HouseType *house){
    initHunterList(&house->HunterList);
    initRoomList(&house->rooms);
    initEvidenceList(&house->evList);
}

void cleanupHouse(HouseType *house){
    cleanupHunterList(&house->HunterList);
    cleanupRoomData(&house->rooms);
    cleanupRoomList(&house->rooms);
    cleanupEvidenceList(&house->evList);
}

/*
    Dynamically allocates several rooms and populates the provided house.
    Note: You may modify this as long as room names and connections are maintained.
        out: house - the house to populate with rooms. Assumes house has been initialized.
*/
void populateRooms(HouseType* house) {
    // First, create each room

    // createRoom assumes that we dynamically allocate a room, initializes the values, and returns a RoomType*
    // create functions are pretty typical, but it means errors are harder to return aside from NULL
    RoomType *van                = createRoom("Van");
    RoomType *hallway            = createRoom("Hallway");
    RoomType *master_bedroom     = createRoom("Master Bedroom");
    RoomType *boys_bedroom       = createRoom("Boy's Bedroom");
    RoomType *bathroom           = createRoom("Bathroom");
    RoomType *basement           = createRoom("Basement");
    RoomType *basement_hallway   = createRoom("Basement Hallway");
    RoomType *right_storage_room = createRoom("Right Storage Room");
    RoomType *left_storage_room  = createRoom("Left Storage Room");
    RoomType *kitchen            = createRoom("Kitchen");
    RoomType *living_room        = createRoom("Living Room");
    RoomType *garage             = createRoom("Garage");
    RoomType *utility_room       = createRoom("Utility Room");
    // RoomType* van;
    // RoomType* hallway;
    // RoomType* master_bedroom;
    // RoomType* boys_bedroom;
    // RoomType* bathroom;
    // RoomType* basement;
    // RoomType* basement_hallway;
    // RoomType* right_storage_room;
    // RoomType* left_storage_room;
    // RoomType* kitchen;
    // RoomType* living_room;
    // RoomType* garage;
    // RoomType* utility_room;
    // initRoom("Van", &van);
    // initRoom("Hallway", &hallway);
    // initRoom("Master Bedroom", &master_bedroom);
    // initRoom("Boy's Bedroom", &boys_bedroom);
    // initRoom("Bathroom", &bathroom);
    // initRoom("Basement", &basement);
    // initRoom("Basement Hallway", &basement_hallway);
    // initRoom("Right Storage Room", &right_storage_room);
    // initRoom("Left Storage Room", &left_storage_room);
    // initRoom("Kitchen", &kitchen);
    // initRoom("Living Room", &living_room);
    // initRoom("Garage", &garage);
    // initRoom("Utility Room", &utility_room);


    // This adds each room to each other's room lists
    // All rooms are two-way connections
    connectRooms(van, hallway);
    connectRooms(hallway, master_bedroom);
    connectRooms(hallway, boys_bedroom);
    connectRooms(hallway, bathroom);
    connectRooms(hallway, kitchen);
    connectRooms(hallway, basement);
    connectRooms(basement, basement_hallway);
    connectRooms(basement_hallway, right_storage_room);
    connectRooms(basement_hallway, left_storage_room);
    connectRooms(kitchen, living_room);
    connectRooms(kitchen, garage);
    connectRooms(garage, utility_room);

    // Add each room to the house's room list
    addRoom(&house->rooms, van);
    addRoom(&house->rooms, hallway);
    addRoom(&house->rooms, master_bedroom);
    addRoom(&house->rooms, boys_bedroom);
    addRoom(&house->rooms, bathroom);
    addRoom(&house->rooms, basement);
    addRoom(&house->rooms, basement_hallway);
    addRoom(&house->rooms, right_storage_room);
    addRoom(&house->rooms, left_storage_room);
    addRoom(&house->rooms, kitchen);
    addRoom(&house->rooms, living_room);
    addRoom(&house->rooms, garage);
    addRoom(&house->rooms, utility_room);
}