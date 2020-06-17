#ifndef USER_CONTROLLER_HPP
#define USER_CONTROLLER_HPP

#include <fstream>
#include <iostream>
#include "Order.hpp"
#include "User.hpp"
#include "BTree.hpp"

class Interface;

class UserController {
public:
	int user_cnt; // WARNING: init value yet to be set as 0
	std::fstream btree_file; // updated
	std::fstream info_file; // updated
	Interface *interface;
	BTree<std::pair<int, int>, User> btree;
	bool is_online[100007];
	
	void add_user( const char cur_username[],
				   const char username[],
				   const char password[],
				   const char name[],
				   const char mail_addr[],
				   int privilege );
	void login( const char username[],
				const char password[] );
	void logout( const char username[] );
	void query_profile( const char cur_username[],
						const char username[] );
	void modify_profile( const char cur_username[],
						 const char username[],
						 const char password[], // empty is NULL
						 const char name[], // empty is NULL
						 const char mail_addr[], // empty is NULL
						 int privilege ); // empty is -1
	void query_order( const char username[] );
	
	void load( Interface *ifs );
	void save();
	
	void modify_order( std::pair<int, int> hash, int order_id, Order order );
	Order get_order( std::pair<int, int> hash, int order_id );
	void print_profile( const char username[], std::pair<int, int> hash );
	void print_order( std::pair<int, int> hash );
	void add_order( std::pair<int, int> hash, const Order& order );
};

#endif // USER_CONTROLLER_HPP
