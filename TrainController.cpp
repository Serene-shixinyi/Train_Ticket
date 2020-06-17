#include "includes.hpp"

void TrainController::add_train( const char train_id[],
                                 int station_num,
                                 int seat_num,
                                 const char stations[][STATION_LEN],
                                 int prices[],
                                 const Time& start_time,
                                 int travel_times[],
                                 int stopover_times[],
                                 Date sale_date_begin,
                                 Date sale_date_end,
                                 char type ) {
    // FAILURE if train_id exists
	std::pair<int, int> hash = Hash().hash(train_id);
    if (btree.exist(hash, btree_file)) {printf("-1\n"); return;}
    if (interface->train_controller_released.btree.exist(hash, interface->train_controller_released.btree_file)) {printf("-1\n"); return;}

	for (int i = 1; i < station_num - 1; ++i) {
		stopover_times[i - 1] += travel_times[i - 1];
		travel_times[i] += stopover_times[i - 1];
		prices[i] += prices[i - 1];
	}

	train_cnt++;
    Train todo_train(train_id, station_num, seat_num, stations, prices, start_time, travel_times, stopover_times, sale_date_begin, sale_date_end, type, train_cnt);
    btree.insert(hash, todo_train, btree_file, info_file);
    
    printf("0\n");
}

void TrainController::delete_train( const char train_id[] ) {
    // FAILURE if train_id doesn't exist
	std::pair<int, int> hash = Hash().hash(train_id);
    if (!btree.exist(hash, btree_file)) {printf("-1\n"); return;}

    btree.remove(hash, btree_file);
    //train_cnt--;
    printf("0\n");
}

void TrainController::load( Interface *ifs, const char *id_filename, const char *info_filename ) {
    interface = ifs;
    info_file.open(info_filename);
    btree_file.open(id_filename);
    FileOperator fop;
    std::fstream cnt_file;
    cnt_file.open("counts");
    if (strcmp(id_filename, "train_id_unreleased") == 0) {// train_cnt(unreleased)
        fop.read(cnt_file, 8, &train_cnt, 1);
    }
    else {// train_cnt(released)
        fop.read(cnt_file, 12, &train_cnt, 1);
    }
    cnt_file.close();
}

void TrainController::save( int pos ) {
	btree.write_cache(btree_file, info_file);
    info_file.close();
    btree_file.close();
    FileOperator fop;
    std::fstream cnt_file;
    cnt_file.open("counts");
    fop.write(cnt_file, pos, &train_cnt, 1);
    cnt_file.close();
}
