#include "includes.hpp"

Order::Order(const char _username[],
		  int _order_id,
		  int _status,
		  const char _train_id[],// QUESTION: should I write `const char` or `char`?
		  int _from, 
		  int _to,
		  const Date& _sale_date,
		  int _num,
		  int _buy_time):
          order_id(_order_id),
          status(_status),
		  from(_from),
		  to(_to),
          num(_num),
          buy_time(_buy_time) {
    strcpy(username, _username);
    strcpy(train_id, _train_id);
    sale_date = _sale_date; // WARNING: no operator= for class Date
}

Order::~Order() {}