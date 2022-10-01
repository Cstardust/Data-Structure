#include"Timestamp.h"

Timestamp::Timestamp(int64_t s /* =0 */):mircoSecondsSinceEpoch_(s){}

Timestamp Timestamp::now()
{
    return Timestamp(time(nullptr));
}

string Timestamp::toString() const
{
    tm *t = localtime(&mircoSecondsSinceEpoch_);
    char buf[512];
    sprintf(buf,
            "%04d-%02d-%02d-%02d-%02d-%02d", 
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
    return string(buf);
}