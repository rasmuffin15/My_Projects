//
//  inventory.cpp
//  rasmuffinAuto
//
//  Created by Thomas Rasmussen on 3/26/20.
//  Copyright © 2020 Thomas Rasmussen. All rights reserved.
//

#include "rasmuffinAuto.hpp"

Inventory::Inventory() {
    
}

void Inventory::fillInventory(string fileName) {
    ifstream file;
    file.open(fileName);
    
    if(file.is_open()) {
        int i = 0;
        while(!file.eof() || i <= 24) {
            int j = 0;
            string s;
            
            if(!getline(file, s))
                break;
            
            istringstream ss(s);
            string record[6];
            
            while(ss) {
                s = "";
                if(!getline(ss, s, ','))
                    break;
                record[j] = s;
                j++;
            }
            
            
            Vehicle *vehicleInstance = vehicleType(record);
            inventory[i] = vehicleInstance;
            i++;
        }
        
        if(file.eof() && i >= 24) {
            printf("Inventory for shop %s was successfully filled!\n", fileName.c_str());
        } else {
            printf("Some cars were not added\n because %s inventory is not full\n", fileName.c_str());
            printf("Filling empty values with 'NA'\n");
            
            while(i <= 24) {
                inventory[i] = new Vehicle();
                i++;
            }
        }
        file.close();
    } else {
        printf("Failed to initialize shop %s\n", fileName.c_str());
    }
}

Vehicle* Inventory::vehicleType(string vType[]) {
    if(vType[0] == "Car") {
        return (new Car(vType));
    } else {
        return (new Vehicle());
    }
}

Vehicle* Inventory::getVehicle(int index) {
    return inventory[index];
}

/*
int Inventory::add_() {
    
}

int Inventory::delete_() {
    
}

int Inventory::save_() {
    
}
*/

Inventory::~Inventory() {
    
}


