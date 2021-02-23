#ifndef GALLERYPARAMS_H
#define GALLERYPARAMS_H

#include <QObject>
#include <QPoint>

class GalleryParams;
class ParamSender : public QObject
{
    Q_OBJECT
public:
    inline void set_pos(QPoint);
signals:
    void signal_set_pos(QPoint);
    void params_accepted(GalleryParams*);
};

enum class NextImgOrder
{
    time, a_z, a_z_intellectual, random

};

class GalleryParams
{

public:
    bool cycled = true;
    ParamSender* sender = nullptr;
    int delay_as_100_ms = 30;
    static constexpr int min_delay = 2;
    NextImgOrder order = NextImgOrder::random;
    bool is_reverse = false;
    bool is_manually = false;
    double curr_width = 0;
    bool set_aspect_ratio = false;
    QPoint pos;
    void set_x(int x);
    void set_y(int y);
    bool manually() const
    {
        return is_manually || delay_as_100_ms < min_delay;
    }
    bool dont_keep_aspect_ratio()
    {
        return curr_width < 0.01f;
    }
    void update_source();
    QStringList file_filter = {"*.png", "*.jpg", "*.bmp", "*.gif", "*.JPG"};
    int img_opacity = 100;
};

#endif // GALLERYPARAMS_H
