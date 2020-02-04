#include "Item.h"
#include "Store.h"
#include "TextUI.h"
#include <iostream>
#include <fstream>
using namespace std;

int main(int argc, char const *argv[])
{
	string list = argv[1];

	Store s = Store(list);
	TextUI *menu = new TextUI();
	string choice;

	while(1){
		choice = menu->MainMenu();
		menu->RouteChoice(s, choice);

		if(choice == "Leave"){
			break;
		}
	}
	

	return 0;
}