//
//  classesPractice.hpp
//
//
//  Created by Thomas Rasmussen on 3/24/20.
//

#ifndef rasmuffinAuto_hpp
#define rasmuffinAuto_hpp

#include <stdio.h>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

/*
 Look into making a parent class called vehicle.
 Then we can have a Car subclass, truck subclass, etc...
 The subclasses then would inherit the vehicle class.
 */
class Vehicle {
public:
    Vehicle();
    virtual ~Vehicle();
    
    string companyName;//Volkswagon, Acura, Subaru, etc...
    string vehicleName;//Jetta, ILX, Legacy, etc...
    string vehicleType;//Car, Truck, Motorcycle, etc...
    string vehicleDrive;//rear-wheel, front-wheel, all-wheel
    string vehicleSize;//Full-size, Mid-size, compact
    
    int yearMade;
    int miles;
    int price;
};

class Car : public Vehicle {
public:
    Car(string vehicle[6]);
    ~Car();
    
    string carType; //compact, sport, hatchback, etc...
};

class Suv : public Vehicle {
public:
    Suv();
    ~Suv();
    
    string suvType; //Crossover,
};

class Truck : public Vehicle {
public:
    Truck();
    ~Truck();
    
    string truckType; //Luxury, Utility, Heavy Duty, Offroad, etc...
};

class Motorcycle : public Vehicle {
public:
    Motorcycle();
    ~Motorcycle();
};

class Inventory {
public:
    Inventory(string fileName);
    Inventory();
    ~Inventory();
    
    void fillInventory(string fileName);
    
    Vehicle* vehicleType(string vType[6]);
    Vehicle* getVehicle(int index);
    
    int add_();//Adds a Car to the inventory
    int delete_();//Deletes a Car from the inventory
    int save_();//Writes the inventory to a file
    
    
private:
    Vehicle *inventory[25];//Stores Car instances
};

//Maybe Shop should inherit class 'Inventory'
//Make private because 'Shop has a Inventory'
class Shop : private Inventory {
public:
    Shop(string storeInventory);
    ~Shop();
    
    string getShopName();//Returns the name of the shop instance
    
    void setShopName(string newShopName);//Changes the name of shop instance
    
    void showInventory();//Show the inventory of a shop
private:
    string shopName;//The name of the shop instance
};

#endif /* rasmuffinAuto_hpp */
