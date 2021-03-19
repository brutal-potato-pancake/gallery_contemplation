#ifndef INTELLECTUALFILENAMECOMPARSION_H
#define INTELLECTUALFILENAMECOMPARSION_H

#include <QString>
#include <QList>
#include <QDateTime>


class SpecialStrComp
{
public:
    bool left_in_priority = true;
    bool from_start_only = true;
    bool from_end_only = false;
    bool reverse_order = false;
    bool whole_dir_only = false;
    QStringList consider_as_alphabet;
    struct Params
    {
        int a_beg;
        int b_beg;
        bool is_equal;
    };

    virtual bool is_comparable(QString& a, QString& b, Params& p) = 0;
    virtual bool compare(QString& a, QString& b, Params& p) = 0;
    virtual ~SpecialStrComp() = default;
};

class AlNumCmp : public SpecialStrComp
{
public:
    bool detect_float = false;
    bool detect_signed = false;
    bool check_space_in_signed = false;
    bool detect_hex = false;
    bool detect_hex_prefix = false;
    int max_numbers = 20;
    int max_numbers_for_float = 9;
    inline bool actual_compare(QString& a, QString& b, SpecialStrComp::Params& p);
    virtual bool is_comparable(QString& a, QString& b, SpecialStrComp::Params& p) override;
    virtual bool compare(QString& a, QString& b, SpecialStrComp::Params& p) override;
};

class IgnoreSymbols : public SpecialStrComp
{

};

class DateTimeCmp: public SpecialStrComp
{
public:
};


class EMailCmp: public SpecialStrComp
{
public:
};

class ExtensionCmp: public SpecialStrComp
{
public:
    int first_from_right = 0;
    int last_from_right = -1;
    bool sort = true;
};

class WordSequenseDetector : public SpecialStrComp
{
public:
    QStringList lst;
};

class ExtensionTypesDetector : public SpecialStrComp
{
public:
    QList<QStringList> lst;

};


struct IntellectualFileNameComparsionParams
{
    AlNumCmp al_num_cmp;
    QList<SpecialStrComp*> compare;
    bool check_std_filename_extensions = true;
    bool ignore_case = false;
    bool ignore_all_extensions = false;
    bool just_alphabet = false;
    bool alnum_only = false;
    QStringList std_extensions = {"mp4", "mp3", "7z", "md5", "md3"};
    bool exec(QString& a, QString& b);
    static void remove_all_extension(QString& str);
    static IntellectualFileNameComparsionParams me;
};


inline bool intellectual_names_comparsion(const QString& a, const QString& b)
{
    auto& params = IntellectualFileNameComparsionParams::me;
    if(a.isEmpty() && !b.isEmpty()) return true;
    if(b.isEmpty()) return false;
    if(params.just_alphabet && !params.ignore_all_extensions && !params.ignore_case)
        return a < b;
    QString a1 = a, b1 = b;
    if(params.ignore_case) a1 = a.toLower(), b1 = b.toLower();
    if(params.ignore_all_extensions)
    {
        IntellectualFileNameComparsionParams::remove_all_extension(a1);
        IntellectualFileNameComparsionParams::remove_all_extension(b1);
    }
    if(params.just_alphabet) return a1 < b1;
    if(params.alnum_only)
    {
        SpecialStrComp::Params strs_start = {0, 0, false};
        return params.al_num_cmp.actual_compare(a1, b1, strs_start);
    }
    return params.exec(a1, b1);
}

#endif // INTELLECTUALFILENAMECOMPARSION_H
