#ifndef SRC_HPP
#define SRC_HPP

#include <iostream>
#include <algorithm>
#include "base.hpp"

using namespace std;

struct date {
    int year, month, day;
    date() = default;
    date(int y, int m, int d) : year(y), month(m), day(d) {}
    
    bool operator<(const date& other) const {
        if (year != other.year) return year < other.year;
        if (month != other.month) return month < other.month;
        return day < other.day;
    }
    
    friend std::istream& operator>>(std::istream& is, date& d) {
        is >> d.year >> d.month >> d.day;
        return is;
    }
};

class mail : public object {
protected:
    string postmark;
    date send_date;
    date arrive_date;

public:
    mail() = default;

    mail(string _contain_, string _postmark_, date send_d, date arrive_d) : object(_contain_) {
        postmark = _postmark_;
        send_date = send_d;
        arrive_date = arrive_d;
    }

    void copy(object *o) override {
        contain = reinterpret_cast<mail *>(o)->contain;
        postmark = reinterpret_cast<mail *>(o)->postmark;
        send_date = reinterpret_cast<mail *>(o)->send_date;
        arrive_date = reinterpret_cast<mail *>(o)->arrive_date;
    }

    string send_status(int y, int m, int d) override {
        return "not send";
    }

    string type() override {
        return "no type";
    }

    void print() override {
        object::print();
        cout << "[mail] postmark: " << postmark << endl;
    }
    
    ~mail() override {}
};

class air_mail : public mail {
protected:
    string airlines;
    date take_off_date;
    date land_date;

public:
    air_mail() = default;

    air_mail(string _contain_, string _postmark_, date send_d, date arrive_d, date take_off, date land, string _airline) 
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        take_off_date = take_off;
        land_date = land;
        airlines = _airline;
    }

    void copy(object *o) override {
        mail::copy(o);
        airlines = reinterpret_cast<air_mail *>(o)->airlines;
        take_off_date = reinterpret_cast<air_mail *>(o)->take_off_date;
        land_date = reinterpret_cast<air_mail *>(o)->land_date;
    }

    string send_status(int y, int m, int d) override {
        date query_date(y, m, d);
        if (query_date < send_date) return "mail not send";
        if (query_date < take_off_date) return "wait in airport";
        if (query_date < land_date) return "in flight";
        if (query_date < arrive_date) return "already land";
        return "already arrive";
    }

    string type() override {
        return "air";
    }

    void print() override {
        mail::print();
        cout << "[air] airlines: " << airlines << endl;
    }
    
    ~air_mail() override {}
};

class train_mail : public mail {
protected:
    string *station_name;
    date *station_time;
    int len;

public:
    train_mail() : station_name(nullptr), station_time(nullptr), len(0) {}

    train_mail(string _contain_, string _postmark_, date send_d, date arrive_d, string *sname, date *stime, int station_num)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        len = station_num;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = sname[i];
                station_time[i] = stime[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }

    void copy(object *o) override {
        mail::copy(o);
        train_mail *tm = reinterpret_cast<train_mail *>(o);
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
        len = tm->len;
        if (len > 0) {
            station_name = new string[len];
            station_time = new date[len];
            for (int i = 0; i < len; ++i) {
                station_name[i] = tm->station_name[i];
                station_time[i] = tm->station_time[i];
            }
        } else {
            station_name = nullptr;
            station_time = nullptr;
        }
    }

    string send_status(int y, int m, int d) override {
        date query_date(y, m, d);
        if (query_date < send_date) return "mail not send";
        if (len == 0) {
            if (query_date < arrive_date) return "wait in station";
            return "already arrive";
        }
        if (query_date < station_time[0]) return "wait in station";
        for (int i = 0; i < len - 1; ++i) {
            if (query_date < station_time[i + 1]) {
                return "run between " + station_name[i] + " and " + station_name[i + 1];
            }
        }
        if (query_date < arrive_date) return "already arrive " + station_name[len - 1];
        return "already arrive";
    }

    string type() override {
        return "train";
    }

    void print() override {
        mail::print();
        cout << "[train] station_num: " << len << endl;
    }

    ~train_mail() override {
        if (station_name) delete[] station_name;
        if (station_time) delete[] station_time;
    }
};

class car_mail : public mail {
protected:
    int total_mile;
    string driver;

public:
    car_mail() = default;

    car_mail(string _contain_, string _postmark_, date send_d, date arrive_d, int mile, string _driver)
        : mail(_contain_, _postmark_, send_d, arrive_d) {
        total_mile = mile;
        driver = _driver;
    }

    void copy(object *o) override {
        mail::copy(o);
        total_mile = reinterpret_cast<car_mail *>(o)->total_mile;
        driver = reinterpret_cast<car_mail *>(o)->driver;
    }

    string send_status(int y, int m, int d) override {
        date query_date(y, m, d);
        if (query_date < send_date) return "mail not send";
        if (query_date < arrive_date) {
            int used_time = (query_date.year - send_date.year) * 360 + (query_date.month - send_date.month) * 30 + (query_date.day - send_date.day);
            int total_time = (arrive_date.year - send_date.year) * 360 + (arrive_date.month - send_date.month) * 30 + (arrive_date.day - send_date.day);
            double current_mile = (double)used_time / (double)total_time * (double)total_mile;
            return to_string(current_mile);
        }
        return "already arrive";
    }

    string type() override {
        return "car";
    }

    void print() override {
        mail::print();
        cout << "[car] driver_name: " << driver << endl;
    }
    
    ~car_mail() override {}
};

void obj_swap(object *&lhs, object *&rhs) {
    object *tmp = lhs;
    lhs = rhs;
    rhs = tmp;
}

#endif // SRC_HPP