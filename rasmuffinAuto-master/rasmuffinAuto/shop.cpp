//
//  shop.cpp
//  rasmuffinAuto
//
//  Created by Thomas Rasmussen on 3/25/20.
//  Copyright © 2020 Thomas Rasmussen. All rights reserved.
//

#include "rasmuffinAuto.hpp"

/*
 Parameters:
    storeInventory - name of the file containing our inventory of cars
 Return:
    None
 Description:
    This method is the constructor for the 'Shop' class.
    It sets the store name equal to the file name and it
    then creates/stores the lines as instances of the 'Car' class.
 
 */
Shop::Shop(string storeInventory)
: Inventory()
{
    
    //Parse file name to get the store name
    istringstream ss(storeInventory);
    
    if(!getline(ss, shopName, '.'))
        printf("We don't know our shop name\n");
    else
        printf("Welcome to the %s :)\n\n", shopName.c_str());
    
    this->fillInventory(storeInventory);
}

void Shop::setShopName(string newShopName) {
    shopName = newShopName;
}

string Shop::getShopName() {
    return shopName;
}

/*
Parameters:
   None
Return:
   None
Description:
    This method will display a 'Inventory' instance's inventory.
    Loop will break early if it detects that the inventory
    is not full. Also the program will hault until the user
    hits the return key to allow the inventory view to be more clear.

*/
void Shop::showInventory() {
    for (int i = 0; i < 24; i++) {
        if(this->getVehicle(i)->companyName == "NA")
            break;
        else {
            cout << "\n**********\n";
            cout << "Company: " << this->getVehicle(i)->companyName << "\n";
            cout << "Name: " << this->getVehicle(i)->vehicleName << "\n";
            cout << "Type: " << this->getVehicle(i)->vehicleType << "\n";
            cout << "Drive: " << this->getVehicle(i)->vehicleDrive << "\n";
            cout << "**********\n\n";
        }
    }
    cout << "Scroll up to see inventory :)\n";
    cout << "Press 'return' key to continue";
    
    system("read");
    
    cout << "\n\n";
}

Shop::~Shop() {
    
}
