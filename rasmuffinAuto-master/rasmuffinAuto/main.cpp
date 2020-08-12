//
//  classesPractice.cpp
//  
//
//  Created by Thomas Rasmussen on 3/24/20.
//

#include "rasmuffinAuto.hpp"

/*
 --Displays the menu options to
    the user.
 --Returns the user's choice from
    the menu.
 */
char shopMenu() {
    
    
    printf("**** Menu ****\n");
    printf("1. Browse Cars\n");
    printf("2. Buy Car\n");
    printf("3. Sell Car\n");
    printf("4. Leave Store\n\n");
    printf("Choice: ");
    
    return getc(stdin);
}

int main() {
    
    char menuChoice;
    Shop *boulderShop = new Shop("Boulder Shop.txt"); //Create new shop instance
    
    while(1) {//Display menu to user until they choose to exit
        
        fflush(stdin);//Clear user input
        menuChoice = shopMenu();//Display menu to user
        
        switch (menuChoice) {//Execute appropriate code dependent on user input
            case '1'://Show Shop inventory to user
                boulderShop->showInventory();
                break;
            case '2'://Allow the user to buy a car from the shop
                //Has not been implemented
                printf("No you will get nothing and like it\n");
                break;
            case '3'://Allow user to sell their car to the shop
                //Has not been implemented
                printf("Yea like your car is actually worth something\n");
                break;
            case '4'://Allow the user to exit the program (Leave the shop.)
                printf("\nThank you come again\n");
                delete boulderShop;
                return 0;
            default://User gave a garbage input
                printf("\nYour choice '%c' is not valid\n", menuChoice);
                break;
        }
    }
}
