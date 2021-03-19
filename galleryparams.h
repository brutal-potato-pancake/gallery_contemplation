#ifndef GALLERYPARAMS_H
#define GALLERYPARAMS_H

#include <QObject>
#include <QPoint>
#include <QPixmap>
#include <QWidget>
#include <QFileDialog>
#include <memory>
#include <QTranslator>
#include <QIcon>
#include <functional>

class GetFile
{
    std::unique_ptr<QFile> ret;
    QFile* old_file();
    QFile void_file;
public:
    QFileDialog file_dialog;
    GetFile(QWidget* parent = nullptr, const QString &caption = QString(),
            const QString &directory = QString(),
            const QString &filter = QString());
    const QFile* get_file(){return get_file_and_delete_old();}
    const QFile* get_file_and_delete_old();
    inline QFile* get_file_manual_delete_requared();
    bool allow_nullptr_as_ret = true;
    bool control_life_time_of_returned_file = true;
    bool is_exec_and_not_open = true;
    void destroy_file(){ret.reset(nullptr);}
    QIcon get_icon();
    QFileDialog& operator->()
    {
        return file_dialog;
    }
    virtual ~GetFile() = default;
};

class GalleryParams;
class ParamSender : public QObject
{
    Q_OBJECT
public:
    inline void set_pos(QPoint);
    virtual ~ParamSender() = default;
signals:
    void signal_set_pos(QPoint);
    void params_accepted(GalleryParams*);

};

enum class NextImgOrder
{
    time, a_z, a_z_intellectual, random

};

enum class KeepAspectRatioVariant
{
    dont_keep, resize_window, add_margins
};

class FrameParams
{
public:
    enum class Type
    {
        no, normal, equal_width, png
    };
    QRect top, down, left, right;
    QRect img_rect;
    QSize frame_sz;
    void calc_permanent_frame(QSize frame, uint permanent_frame_sz = 0);
    void calc_margins(QSize img_in_frame, QRect w_frame);
    void calc_margins_by_img_rect(QSize frame);

    QSize calc_img_in_frame_size(QSize frame, uint frame_sz);
    QPoint calc_img_in_frame_pos(QSize frame, uint frame_sz);
    FrameParams(QSize img, QSize w_frame, uint permanent_frame_sz = 0);
};


struct Margins final
{
    QColor color = Qt::black;
    QPixmap img;
    uint equal_width_margins_sz = 0;
    FrameParams::Type type = FrameParams::Type::normal;
    enum UseImg
    {
        margin_color_only, as_frame, pave
    } using_mg = margin_color_only;
    bool operator!() const
    {
        return img.isNull() || using_mg != margin_color_only;
    }
};


class KeepAspectRatio final
{
    bool check_applicable(Margins::UseImg margins_type, void* func);
    typedef  QPixmap& (*paint_func)( const FrameParams&, QPixmap &);

public:
    QWidget* w = nullptr;
    KeepAspectRatioVariant set_aspect_ratio = KeepAspectRatioVariant::dont_keep;

    QPixmap new_pixmap;
    Margins margins;
    typedef const Margins* const C_Margins;
    static QPixmap fit_img_to_widget(const QPixmap& source_img, QSize size_for_fit);
    QPixmap resize(const QPixmap& source_pix_map);
    static QPixmap add_margins(const QPixmap& img, QSize new_size, C_Margins margins = nullptr);
    QPixmap& add_margins(const QPixmap& source_pixmap);
    static void paint_for_pave(QPixmap& ret, QRect rect, const QPixmap& for_pave);

    bool operator!()
    {
        return w == nullptr || set_aspect_ratio == KeepAspectRatioVariant::dont_keep;
    }
    static QPixmap paint_img(const FrameParams& frame, const QPixmap &src);
    static QPixmap& paint_with_color(const FrameParams& frame, QPixmap &ret, QColor color);
    static QPixmap& paint_with_pave(const FrameParams& frame, QPixmap &ret, const QPixmap &margins);
    static QPixmap& paint_frame(const FrameParams& frame, QPixmap &ret, const QPixmap &margins);
};


class GalleryParams final
{

public:
    bool cycled = true;
    ParamSender* sender = nullptr;
    int delay_as_100_ms = 30;
    static constexpr int min_delay = 2;
    NextImgOrder order = NextImgOrder::random;
    bool is_reverse = false;
    bool is_manually = false;
    KeepAspectRatio aspect_ratio;
    QPoint pos;
    void set_x(int x);
    void set_y(int y);
    bool manually() const
    {
        return is_manually || delay_as_100_ms < min_delay;
    }
    void update_source();
    QStringList file_filter = {"*.png", "*.jpg", "*.bmp", "*.gif", "*.JPG"};
    int img_opacity = 100;
    static const QString& image_files_str()
    {
        static QString ret = QWidget::tr("Images(*.png *.jpg *.jpeg *.bmp *.svg)");
        return ret;
    }
};

#endif
