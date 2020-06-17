#include "includes.hpp"

#define controller_unreleased itf->train_controller_unreleased
#define controller_released itf->train_controller_released

void ReleasedTrainController::release_train( const char train_id[] ) {
	std::pair<int, int> id = Hash().hash(train_id);
	if(!controller_unreleased.btree.exist(id, controller_unreleased.btree_file)) {
		printf("-1\n");
		return;
	}
	printf("0\n");
	Train train = controller_unreleased.btree.query(id, controller_unreleased.btree_file, controller_unreleased.info_file);
	controller_unreleased.btree.remove(id, controller_unreleased.btree_file);
	controller_released.train_cnt++;
	train.create_time = controller_released.train_cnt; //
	controller_released.btree.insert(id, train, controller_released.btree_file, controller_released.info_file);
	//ticket_file
	for (int i = 0; i < DATE_MAX * SUM; ++i) 
		file_operator.write(ticket_file, -1, &train.seat_num, 1);
	//station_btree (haven't done in TrainController::add_train())
	for (int i = 0; i < train.station_num; ++i) {
		std::pair<int, int> station_id = Hash().hash(train.stations[i]);
		itf->ticket_controller.btree.insert(std::make_pair(station_id, id), std::make_pair(TicketController::Char(train_id), i), itf->ticket_controller.btree_file, itf->ticket_controller.info_file);
	}
}

void ReleasedTrainController::query_train( const char train_id[], Date date ) {
	if(!date.exist()) {
		printf("-1\n"); return;
	}
	bool fl = 0;
	//train_id exist
	std::pair<int, int> id = Hash().hash(train_id);
	Train train;
	if(controller_released.btree.exist(id, controller_released.btree_file)) {
		train = controller_released.btree.query(id, controller_released.btree_file, controller_released.info_file);
	}
	else if(controller_unreleased.btree.exist(id, controller_unreleased.btree_file)) {
		train = controller_unreleased.btree.query(id, controller_unreleased.btree_file, controller_unreleased.info_file);
		fl = 1;
	}
	else {
		printf("-1\n"); return;
	}
	//check date
	if(date < train.sale_date_begin || train.sale_date_end < date) {
		printf("-1\n"); return;
	}
	//read ticket_file
	int *ticket = new int[SUM + 7];
	if(fl) {
		for (int i = 0; i < train.station_num; ++i) ticket[i] = train.seat_num;
	}
	else file_operator.read(ticket_file, ((train.create_time - 1) * DATE_MAX * SUM + (date - train.sale_date_begin) * SUM) * (int)sizeof(int), ticket, SUM);

	printf("%s %c\n", train_id, train.type);
	Time start = train.start_time, now; start.date = date;
	for (int i = 0; i < train.station_num; ++i) {
		printf("%s ", train.stations[i]);
		if(!i) printf("xx-xx xx:xx");
		else {
			now = start + train.travel_times[i - 1];
			std::cout << now.date << " " << now;
		}
		
		printf(" -> ");

		if(i == train.station_num - 1) printf("xx-xx xx:xx");
		else {
			if(!i) now = start;
			else now = start + train.stopover_times[i - 1];
			std::cout << now.date << " " << now;
		}

		if(!i) printf(" 0 ");
		else printf(" %d ", train.prices[i - 1]);

		if(i == train.station_num - 1) printf("x\n");
		else printf("%d\n", ticket[i]);
	}
	delete []ticket;
}

void ReleasedTrainController::modify_ticket( const Train &train, Date date, int from, int to, int num ) {
	int *ticket = new int[SUM + 7];
	file_operator.read(ticket_file, ((train.create_time - 1) * DATE_MAX * SUM + (date - train.sale_date_begin) * SUM) * (int)sizeof(int), ticket, SUM);
	for (int i = from; i < to; ++i) ticket[i] += num;
file_operator.write(ticket_file, ((train.create_time - 1) * DATE_MAX * SUM + (date - train.sale_date_begin) * SUM) * (int)sizeof(int), ticket, SUM);
	delete []ticket;
}

void ReleasedTrainController::add_order( std::pair<int, int> hash, Date date, const char username[], int order_id ) {
	int buy_time = itf->order_controller.order_cnt;
	btree.insert(make_tuple(hash, date, buy_time), std::make_pair(Char(username), order_id), que_btree_file, que_info_file);
}

void ReleasedTrainController::delete_order( std::pair<int, int> id, Date date, std::pair<int, int> userid, int order_id, int buy_time ) {
	if(buy_time == -1) {
		Order order = itf->order_controller.btree.query(std::make_pair(userid, order_id), itf->order_controller.btree_file, itf->order_controller.info_file);
		buy_time = order.buy_time;
	}
	btree.remove(make_tuple(id, date, buy_time), que_btree_file);
}

void ReleasedTrainController::adjust_order( std::pair<int, int> id, Date date ) {
	std::tuple<std::pair<int, int>, Date, int> l = make_tuple(id, date, -1);
	std::tuple<std::pair<int, int>, Date, int> r = make_tuple(id, date, 1e8);
	std::pair<std::tuple<std::pair<int, int>, Date, int>, std::pair<Char, int> > *value;
	int cnt = btree.query_list(l, r, que_btree_file, que_info_file, value);
	std::pair<int, int> userid; int order_id;
	char username[USERNAME_LEN];
	for (int i = 0; i < cnt; ++i) {
		value[i].second.first.copy(username);
		userid = Hash().hash(username);
		order_id = value[i].second.second;
		Order order = itf->order_controller.btree.query(std::make_pair(userid, order_id), itf->order_controller.btree_file, itf->order_controller.info_file);
		Train train = controller_released.btree.query(id, controller_released.btree_file, controller_released.info_file);
		if(query_ticket(train, date, order.from, order.to) >= order.num) {
			delete_order(id, date, userid, order_id, order.buy_time);
			modify_ticket(train, date, order.from, order.to, -order.num); //
			order.status = STATUS_SUCCESS;
			itf->user_controller.modify_order(userid, order_id, order);
		}
	}
	if(cnt) delete []value;
}

int ReleasedTrainController::query_ticket( const Train &train, Date date, int from, int to ) {
	int *ticket = new int[SUM + 7];
	file_operator.read(ticket_file, ((train.create_time - 1) * DATE_MAX * SUM + (date - train.sale_date_begin) * SUM) * (int)sizeof(int), ticket, SUM);
	int rs = 1e8;
	for (int i = from; i < to && rs > 0; ++i) {
		if(ticket[i] < rs) rs = ticket[i];
	}
	delete []ticket;
	return rs;
}

void ReleasedTrainController::load(Interface *interface) {
	itf = interface; 
	ticket_file.open("ticket_info");
	que_btree_file.open("que_btree");
	que_info_file.open("que_info");
}

void ReleasedTrainController::save() {
	file_operator.write_cache(ticket_file);
	btree.write_cache(que_btree_file, que_info_file);
	ticket_file.close();
	que_btree_file.close();
	que_info_file.close();
}

#undef controller_unreleased
#undef controller_released
