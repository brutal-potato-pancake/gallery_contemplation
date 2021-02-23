#include "intellectualfilenamecomparsion.h"

IntellectualFileNameComparsionParams IntellectualFileNameComparsionParams::me;

bool IntellectualFileNameComparsionParams::exec(QString &a, QString &b)
{
    SpecialStrComp::Params params;
    for(auto& i: compare)
    {
        if(!i->is_comparable(a, b, params)) continue;
        auto is_less = i->compare(a, b, params);
        if(params.is_equal) continue;
        return is_less;
    }
    return a < b;
}

void IntellectualFileNameComparsionParams::remove_all_extension(QString &str)
{

}

bool AlNumCmp::actual_compare(QString &a, QString &b, Params &p)
{

}

bool AlNumCmp::is_comparable(QString &a, QString &b, SpecialStrComp::Params &p)
{

}

bool AlNumCmp::compare(QString &a, QString &b, Params &p)
{
    return actual_compare(a, b, p);
}


