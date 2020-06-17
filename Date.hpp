#ifndef DATE_HPP
#define DATE_HPP

#include<iostream>

class Date{
	int cal_day() const;
public:
	int month, day;
	Date(int m = 0, int d = 0) : month(m), day(d){};
	Date operator + (const int num) const; //num >= 0
	bool operator < (const Date& b) const;
	int operator - (const Date& b) const;
	friend std::ostream &operator<<(std::ostream &os, const Date &date);
	friend std::istream &operator>>(std::istream &os, Date &date);
	bool exist() const;
};

#endif //DATE_HPP