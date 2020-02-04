/*
 Thomas Rasmussen
 CSCI 3010
 Homework Two
 Felix Muzny
 */

#define CATCH_CONFIG_MAIN  // This tells Catch to provide a main() - only do this in one cpp file
#include "catch.hpp"
#include <stdlib.h>     /* srand, rand */
#include <time.h> 
#include <stdio.h>
#include <vector>
#include "Store.h"
#include "Item.h"

using namespace std;

TEST_CASE ("Items are Initialized", "[Item]") {

	Item *i1 = new Item(1, "apples", 69, 420);
	Item *i2 = new Item(2, "bananas", 97, 2015);
	Item *i3 = new Item(3, "oranges", 15, 123);
	Item *i4 = new Item(4, "peaches", 16, 666);

	REQUIRE( i1 != nullptr);
	REQUIRE( i2 != nullptr);
	REQUIRE( i3 != nullptr);
	REQUIRE( i4 != nullptr);

	SECTION("Getting Private ID") {
		int id_1 = i1->get_id();
		int id_2 = i2->get_id();

		REQUIRE(id_1 == 1);
		REQUIRE(id_2 == 2);
	}

	SECTION("Getting the Quantity of Item") {
		int q_1 = i1->get_quantity();
		int q_2 = i2->get_quantity();

		REQUIRE(q_1 == 420);
		REQUIRE(q_2 == 2015);
	}

	SECTION("Getting the Cost of Item") {
		double c_1 = i1->get_cost();
		double c_2 = i2->get_cost();

		REQUIRE(c_1 == 69);
		REQUIRE(c_2 == 97);
	}

	SECTION("Getting the type of Item") {
		string t_1 = i1->get_type();
		string t_2 = i2->get_type();

		REQUIRE(t_1 == "apples");
		REQUIRE(t_2 == "bananas");
	}
}


TEST_CASE ("Change Quantity Amounts", "[Item]") {

	Item *i1 = new Item(1, "apples", 69, 420);
	Item *i2 = new Item(2, "bananas", 97, 2015);
	Item *i3 = new Item(3, "oranges", 15, 123);
	Item *i4 = new Item(4, "peaches", 16, 666);

	SECTION("Increasing the Quantity") {
		i1->IncreaseQuantity(0);
		i2->IncreaseQuantity(1);
		i3->IncreaseQuantity(-5);
		i4->IncreaseQuantity(10);

		REQUIRE(i1->get_quantity() == 420);
		REQUIRE(i2->get_quantity() == 2016);
		REQUIRE(i3->get_quantity() == 118);
		REQUIRE(i4->get_quantity() == 676);
	}

	SECTION("Decreasing the Quantity") {
		i1->DecreaseQuantity(0);
		i2->DecreaseQuantity(1);
		i3->DecreaseQuantity(-5);
		i4->DecreaseQuantity(700);

		REQUIRE(i1->get_quantity() == 420);
		REQUIRE(i2->get_quantity() == 2014);
		REQUIRE(i3->get_quantity() == 128);
		REQUIRE(i4->get_quantity() == -34);
	}
}

TEST_CASE("Cloning an Item", "[Item]") {

	Item *i1 = new Item(1, "apples", 69, 420);
	Item *i2 = new Item(2, "bananas", 97, 2015);
	
	Item *i3 = i1->Clone();
	Item *i4 = i2->Clone();

	REQUIRE(i1 != i3);
	REQUIRE(i2 != i4);
	REQUIRE(i1->get_cost() == i3->get_cost());
	REQUIRE(i2->get_id() == i4->get_id());
}

TEST_CASE("Adding/Removing an Item to the Cart","[ShoppingCart]") {

	ShoppingCart *s = new ShoppingCart();

	Item *i1 = new Item(1, "apples", 69, 1);
	Item *i2 = new Item(2, "bananas", 97, 1);

	s->AddItem(i1);
	s->AddItem(i2);

	vector<Item *> test_vect = s->get_items();

	SECTION("Adding Item to cart") {

		REQUIRE(test_vect.size() == 2);
	}

	SECTION("Removing Item to cart") {

		s->RemoveItem(test_vect[1]);
		s->RemoveItem(test_vect[0]);

        test_vect = s->get_items();

		REQUIRE(test_vect.size() == 0);
	}
}

TEST_CASE("Clearing the Cart in ShoppingCart", "[ShoppingCart]") {

	ShoppingCart *s = new ShoppingCart();

	Item *i1 = new Item(1, "apples", 69, 420);
	Item *i2 = new Item(2, "bananas", 97, 2015);

	s->AddItem(i1);
	s->AddItem(i2);

	s->ClearCart();

	vector<Item *> test_vect = s->get_items();

	REQUIRE(test_vect.size() == 0);
}

TEST_CASE("Getting Items from Shopping Cart", "[ShoppingCart]") {
	
	ShoppingCart *s = new ShoppingCart();

	Item *i1 = new Item(1, "apples", 69, 420);
	Item *i2 = new Item(2, "bananas", 97, 2015);

	s->AddItem(i1);
	s->AddItem(i2);

	vector<Item *> test_vect = s->get_items();

	REQUIRE(test_vect.size() == 2);
	REQUIRE(test_vect[0]->get_type() == "apples");
	REQUIRE(test_vect[1]->get_cost() == 97);
	REQUIRE(test_vect[0]->get_id() == 1);
	REQUIRE(test_vect[1]->get_quantity() == 2015);
}

TEST_CASE("Initializing a Store and getting name", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	REQUIRE(s != nullptr);
	REQUIRE(s->get_name() == "My Store");
}

TEST_CASE("Displaying Inventory", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	REQUIRE(s->DisplayInventory() == "Oranges: 3.00 - 9 \n Apples: 5.00 - 3 \n Bananas: 17.00 - 24");

}

TEST_CASE("Testing Items","[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "31" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	map<int, std::string> items = s->Items();

	REQUIRE(items[0] == "Oranges");
}

TEST_CASE("Adding Item to Cart", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	int item_id = 1;



	s->AddItemToCart(item_id);

	map<int, std::string> items = s->CartItems();

	REQUIRE(items[0] == "Oranges");
}

TEST_CASE("Removing Item From Cart", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	int item_id = 1;
	s->AddItemToCart(item_id);
	s->RemoveItemFromCart(item_id);

	map<int, std::string> items = s->CartItems();

	REQUIRE(items[0] == "");

}

TEST_CASE("Displaying the cart", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	int item_id = 1;
	s->AddItemToCart(item_id);

	REQUIRE(s->DisplayCart() == "Oranges: 3.00 - 1\n");
}

TEST_CASE("Clearing the Cart Store","[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	outfile.close();

	Store *s = new Store("test.txt");

	int item_id = 1;
	s->AddItemToCart(item_id);
	s->ClearCart();

	REQUIRE(s->DisplayCart() == "");
}

TEST_CASE("Checking out from the Store", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	Store *s = new Store("test.txt");

	int item_id = 1;
	s->AddItemToCart(item_id);


	REQUIRE(s->Checkout() == 3.26535);
}

TEST_CASE("Getting name", "[Store]") {

	ofstream outfile ("test.txt");

	outfile << "My Store" << endl;
	outfile << "" << endl;
	outfile << "Inventory:" << endl;
	outfile << "Oranges,3.00000,9" << std::endl;
	outfile << "Apples,5.00000,3" << std::endl;
	outfile << "Bananas,17.00000,24" << std::endl;

	Store *s = new Store("test.txt");

	REQUIRE(s->get_name() == "My Store");
}
