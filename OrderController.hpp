#ifndef ORDER_CONTROLLER_HPP
#define ORDER_CONTROLLER_HPP

#include <fstream>

class Interface;

class OrderController {
public:
	int order_cnt;
	std::fstream btree_file;
	std::fstream info_file;
	Interface *interface;

	void load( Interface *ifs );
	void save();
};

#endif // ORDER_CONTROLLER_HPP
