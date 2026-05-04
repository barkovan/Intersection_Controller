#ifndef VEHICLE_H
#define VEHICLE_H

#include "common.h"

Vehicle* createVehicle(float x, float y, GLuint texID);
void deleteVehicle(Vehicle* v);
void clearAllVehicles(void);

void spawnLogic(float deltaTime);
void updateVehicles(float deltaTime);

#endif