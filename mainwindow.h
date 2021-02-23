#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QImage>
#include <QRandomGenerator>
#include <QSettings>
#include <QTimer>
#include <QEvent>
#include <QMenu>
#include <QFileInfo>
#include <functional>
#include <QDateTime>
#include <QMouseEvent>

#include "galleryparams.h"
#include "optionsdialog.h"
#include "controlform.h"



QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class EvToSignal : public QObject
{
    Q_OBJECT
public:
    QObject* from;
    int type;
    EvToSignal(QObject* _from, int _type) : QObject(_from)
    {
        from = _from;
        from->installEventFilter(this);
        type = _type;
    }
    bool eventFilter(QObject *object, QEvent *event)
    {
        if (object == from && type == event->type()) {
            last_event = event;
            emit me();
        }
        return false;
    }

    idtype_t t;
    QEvent* last_event = nullptr;

signals:
    void me();
};

template <class Z, class T, class Functor>
T search_next(const Z& list, const T& what, Functor compare, bool cycle = false)
{
    if(list.empty()) return T();
    T const* next = nullptr;
    bool prev_is_exists = what.exists();
    if(prev_is_exists)
        for(auto& i: list)
        {
            if(compare(what, i) && (!next || compare(i, *next)))
                next = &i;
        }
    if(next != nullptr) return *next;
    if(prev_is_exists && !cycle) return what;

    T const* max = &list.first();
    T const* min = max;
    for(auto& i: list) if(compare(i, *min)) min = &i;
    return *min;
}


inline bool intellectual_names_comparsion(const QString& a, const QString& b)
{
    return true;
}



class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static constexpr int def_image_switch_delay = 3000;
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    QPixmap curr_img;
    QDir curr_dir;

    template<class Func>
    QFileInfo get_next(Func func)
    {
        return search_next(curr_dir.entryInfoList(params.file_filter), last_file_info, func, params.cycled);
    }

    template<class Func>
    QString get_next__wrapper(Func func)
    {
        auto ret = get_next(func);
        if(!ret.exists()) return QString();
        last_file_info = ret;
        return last_file_info.filePath();
    }

    void refresh_delay();
    void set_delay();
    void set_dir(QString new_dir);
    void set_menu();
    void set_order();
    void restore_params();
    void check_bool_option(const QString &name, bool& option);
    QString next_img();
    QString next_img_az();
    QString next_img_az_intellectual();
    QString next_img_time();
    virtual void mousePressEvent(QMouseEvent *event) override;
    void change_window_size(int x, int y);
    void change_window_size(QSize sz);
    bool work_around_bug__after_resize_fullscreen_or_maximized_window_drops_its_windowstate_to_0_but_actually_remains_in_last_mode(int x, int y);
    QSize set_and_apply_aspect_ratio();
    void resizeEvent(QResizeEvent *event) override;
    void set_background();


public slots:
    void show_next_img();
    void load_img(const QString &file_name);
    void show_next_by_timer();
    void show_prev_img();
    void call_menu(QPoint where = QCursor::pos());
    void change_dir();
    void show_settings();
    void switch_title();
    void set_pos(QPoint);
    void update_params(GalleryParams*);
    void set_pause(bool on = true);
    bool get_pause();
    void show_info();

    void switch_fullscreen();
private slots:
    void update_size_while_keep_aspect_ratio();

private:
    Ui::MainWindow *ui;
    bool want_update_size_while_keep_aspect_ratio = false;
    QRandomGenerator rnd;
    GalleryParams params;
    QTimer* timer;
    QTimer timer_update_sz;
    QSettings* settings;
    QFileInfo last_file_info;
    QPixmap background;
    ControlForm* control_form;
    QMenu menu;
};
#endif // MAINWINDOW_H
