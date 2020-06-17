#include "includes.hpp"
//#include "Date.hpp"
//#include<cstdio>

int Date::cal_day() const{
	if(month <= 7) {
		if(month & 1) return 31;
		return 30;
	}
	if(month & 1) return 30;
	return 31;
}

Date Date::operator + (const int num) const{
	Date rs = *this;
	if(num == 0) return rs;
	int x = rs.cal_day() - rs.day;
	if(num <= x) {
		rs.day += num;
		return rs;
	}
	++rs.month; rs.day = 1;
	int NUM = num - x - 1;
	while(NUM >= rs.cal_day()) {
		NUM -= rs.cal_day();
		++rs.month;
	}
	rs.day += NUM;
	return rs;
}

bool Date::operator < (const Date& b) const{
	if(this->month < b.month) return true;
	if(this->month > b.month) return false;
	if(this->day < b.day) return true;
	return false;
}

int Date::operator - (const Date& b) const{
	Date A = *this, B = b;
	int fl = 1, rs = 0;
	if(A < B) { std::swap(A, B); fl = -1; }
	if(A.month == B.month) 
		return fl * (A.day - B.day);
	rs = B.cal_day() - B.day + 1;
	++B.month; B.day = 1;
	while(B.month != A.month) {
		rs += B.cal_day();
		++B.month;
	}
	rs += A.day - B.day;
	return fl * rs;
}

std::ostream &operator<<(std::ostream &os, const Date &date) {
	if(date.month < 10) os << "0";
	os << date.month << "-";
	if(date.day < 10) os << "0";
	os << date.day;
	return os;
}

std::istream &operator>>(std::istream &os, Date &date) {
	os >> date.month >> date.day;
	if(date.day < 0) date.day = -date.day;
	return os;
}

bool Date::exist() const{
	if(month < 1 || month > 12) return false;
	if(day < 1 || day > cal_day()) return false;
	return true;
}