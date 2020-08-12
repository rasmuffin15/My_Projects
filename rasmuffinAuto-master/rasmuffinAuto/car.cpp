//
//  Cars.cpp
//  rasmuffinAuto
//
//  Created by Thomas Rasmussen on 3/24/20.
//  Copyright © 2020 Thomas Rasmussen. All rights reserved.
//

#include "rasmuffinAuto.hpp"

/*
 Paramters:
    cpName - Name of company that makes the car
    cName - Name of the car
    cType - Luxury, Sport, Utility, etc...
    dType - The driveterain of the car
 Return:
    None
 Description:
    This is the constructor for the 'Car' class.
    Takes in multiple parameters and sets them
    equal to the attributes of the 'Car' class.
    
 */


Car::Car(string *vehicle)
: Vehicle()
{
    carType = vehicle[3];
    
    companyName = vehicle[1];
    vehicleName = vehicle[2];
    vehicleType = vehicle[0];
    vehicleDrive = vehicle[4];
    vehicleSize = "NA";
    yearMade = stoi(vehicle[5]);
}

Car::~Car() {
    
}

