#ifndef ORDER_HPP
#define ORDER_HPP

#include "const_variable.hpp"
#include "Date.hpp"

class Order {
public: 
	char username[USERNAME_LEN];
	int order_id;
	int status;
	char train_id[TRAIN_ID_LEN];
	int from;
	int to;
	Date sale_date;
	int num;
	int buy_time;

	Order(const char _username[],
		  int _order_id,
		  int _status,
		  const char _train_id[],
		  int _from,
		  int _to,
		  const Date& _sale_date,
		  int _num,
		  int _buy_time);
	Order() {}
	~Order();
};

#endif // ORDER_HPP
