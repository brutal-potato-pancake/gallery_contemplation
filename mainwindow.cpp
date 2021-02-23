#include <QDebug>
#include <QMouseEvent>
#include <QMenu>
#include <QFileDialog>
#include <QVariant>
#include <algorithm>
#include <QSettings>

#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    curr_dir.setPath(QDir::home().path());
    settings = new QSettings("see_gallery.cfg", QSettings::Format::IniFormat, this);
    //settings->setIniCodec("UTF-8");
    if(!settings->contains("dir"))
        set_dir(QDir::home().absoluteFilePath("Изображения"));

    auto dir_name = settings->value("dir").toString();
    curr_dir.setPath(dir_name);
    curr_dir.refresh();
    setWindowTitle(curr_dir.dirName());
    refresh_delay();
    rnd.seed(QRandomGenerator::system()->generate());
    set_menu();

    if(settings->contains("pos") && settings->contains("size"))
    {
        auto point = settings->value("pos").toPoint();
        auto sz = settings->value("size").toSize();
        this->setGeometry(point.x(), point.y(), sz.width(), sz.height());
    }
    timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(show_next_by_timer()));
    params.delay_as_100_ms = def_image_switch_delay / 1000;
    restore_params();
    timer->start(params.delay_as_100_ms * 100);
    QTimer::singleShot(1, this, SLOT(show_next_img())); //deferred  call, since image became badly loaded when this func called from MainWindow constructor
    params.sender = new ParamSender;
    connect(params.sender, SIGNAL(signal_set_pos(QPoint)), this, SLOT(set_pos(QPoint)));
    connect(params.sender, SIGNAL(params_accepted(GalleryParams*)), this, SLOT(update_params(GalleryParams*)));
    control_form = new ControlForm(this);
    control_form->show();
}

MainWindow::~MainWindow()
{
    settings->setValue("pos", QVariant(this->pos()));
    settings->setValue("size", QVariant(this->size()));
    delete ui;
}

void MainWindow::show_next_img()
{
    QStringList files = curr_dir.entryList(params.file_filter);
    if(files.size() < 1) return;
    load_img(next_img());
}

void MainWindow::load_img(const QString& file_name)
{
    if(file_name.isEmpty())
    {
        ui->lbl_main_hint->show();
        return;
    }
    ui->lbl_main_hint->hide();
    curr_img.load(file_name);
    if(curr_img.isNull()) qDebug()<<file_name;    

    //change_window_size(new_sz);
    background = QPixmap(curr_img);
    set_background();
}

void MainWindow::show_next_by_timer()
{
    if(params.is_manually) return;
    show_next_img();
}

void MainWindow::show_prev_img()
{
    if(params.order != NextImgOrder::random)
    {
        bool reverse = params.is_reverse;
        params.is_reverse = !reverse;
        show_next_img();
        params.is_reverse = reverse;
        return;
    }
}

void MainWindow::call_menu(QPoint where)
{
    menu.exec(where);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if(event->type() != QMouseEvent::MouseButtonPress) return;
    call_menu();
}

void MainWindow::change_window_size(int x, int y)
{
    //ToDo: make this method odinary func and place it in common.h
    work_around_bug__after_resize_fullscreen_or_maximized_window_drops_its_windowstate_to_0_but_actually_remains_in_last_mode(x, y);
}

void MainWindow::change_window_size(QSize sz)
{
    change_window_size(sz.width(), sz.height());
}

bool MainWindow::work_around_bug__after_resize_fullscreen_or_maximized_window_drops_its_windowstate_to_0_but_actually_remains_in_last_mode(int x, int y)
{
    if(this->windowState() != Qt::WindowNoState)
    {
        return false;
    }
    this->resize(x, y);
    return true;
}

void MainWindow::change_dir()
{
    set_dir(QFileDialog::getExistingDirectory(this, QString(), curr_dir.path(), QFileDialog::DontConfirmOverwrite));
}

void MainWindow::show_settings()
{
    params.pos = geometry().topLeft();
    OptionsDialog::exec(params);
}

void MainWindow::switch_title()
{
    bool is_window_frameless = this->windowFlags().testFlag(Qt::FramelessWindowHint);
    if(is_window_frameless)
        setWindowFlags(windowFlags() & ~Qt::FramelessWindowHint);
    else
        setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    show();
}

void MainWindow::set_pos(QPoint p)
{
    auto rect = geometry();
    rect.moveTo(p);
    setGeometry(rect);
}

void MainWindow::update_params(GalleryParams* new_params)
{
    params = *new_params;
    set_pos(params.pos);
    set_delay();
    set_order();
    if(params.set_aspect_ratio)
        params.curr_width = this->size().width();
    this->setWindowOpacity(params.img_opacity);
    this->update();
}

void MainWindow::set_pause(bool on)
{
    params.is_manually = on;
}

bool MainWindow::get_pause()
{
    return params.manually();
}

void MainWindow::show_info()
{

}

QSize MainWindow::set_and_apply_aspect_ratio()
{
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    set_background();
    timer->singleShot(1, this, SLOT(update_size_while_keep_aspect_ratio()));
    QMainWindow::resizeEvent(event);
}

void MainWindow::set_background()
{
    QPixmap resized_background(background);
    resized_background = resized_background.scaled(size(), Qt::IgnoreAspectRatio);
    QPalette new_pallete = palette();
    new_pallete.setBrush(QPalette::Window, resized_background);
    setPalette(new_pallete);
    update();
}

void MainWindow::switch_fullscreen()
{
    auto flag = Qt::WindowFullScreen;
    bool is_flag_active = this->windowState().testFlag(flag);
    auto curr_win_state = this->windowState();
    if(is_flag_active)
        setWindowState(curr_win_state & ~flag);
    else
        setWindowState(curr_win_state | flag);
}

void MainWindow::update_size_while_keep_aspect_ratio()
{
    static auto dt = QDateTime::currentMSecsSinceEpoch();
    auto now = QDateTime::currentMSecsSinceEpoch();
    if(now < dt + 150) return;
    dt = now;
    if(params.dont_keep_aspect_ratio()) return;
    auto a_ratio = background.size().height() / background.size().width();
    change_window_size(this->size().width(), this->size().width() * a_ratio);
}


void MainWindow::refresh_delay()
{
    params.delay_as_100_ms = settings->value("delay").toInt();
}

void MainWindow::set_delay()
{
    settings->setValue("delay", params.delay_as_100_ms);
    settings->setValue("manually", params.is_manually);
    if(!params.manually())
        timer->start(params.delay_as_100_ms * 100);
    else
        timer->stop();
}

void MainWindow::set_dir(QString new_dir)
{
    if(new_dir.isEmpty()) return;
    curr_dir.setPath(new_dir);
    settings->setValue("dir", curr_dir.absolutePath());
    setWindowTitle(curr_dir.dirName());
}

void MainWindow::set_menu()
{
    connect(menu.addAction("🗀"), SIGNAL(triggered()), this, SLOT(change_dir()));
    connect(menu.addAction("Fullscreen"), SIGNAL(triggered()), this, SLOT(switch_fullscreen()));
    connect(menu.addAction("👒 On/Off"), SIGNAL(triggered()), this, SLOT(switch_title()));
    connect(menu.addAction("🔧"), SIGNAL(triggered()), this, SLOT(show_settings()));
    connect(menu.addAction("❌"), SIGNAL(triggered()), this, SLOT(close()));
    QFont font;
    font.setPointSize(20);
    menu.setFont(font);
}

void MainWindow::set_order()
{
    settings->setValue("reverse", params.is_reverse);
    settings->setValue("cycled", params.cycled);
    settings->setValue("order", static_cast<int>(params.order));
}

void MainWindow::restore_params()
{
    check_bool_option("reverse", params.is_reverse);
    check_bool_option("manually", params.is_manually);
    check_bool_option("cycled", params.cycled);
    params.delay_as_100_ms = settings->value("delay").toInt();
    params.delay_as_100_ms = std::max(params.delay_as_100_ms, 2);

    {
        bool ok = false;
        auto var = settings->value("order");
        auto x = var.toInt(&ok);
        if(var.isValid() && !var.isNull())
            if(x >= 0 && ok && x <= static_cast<int>(NextImgOrder::random))
                params.order = static_cast<NextImgOrder>(x);
    }
}

void MainWindow::check_bool_option(const QString &name, bool& option)
{
    auto var = settings->value(name);
    if(var.isValid() && !var.isNull())
        option = var.toBool();
}

QString MainWindow::next_img()
{

    switch (params.order)
    {
    case NextImgOrder::a_z:
        return next_img_az();
    case NextImgOrder::a_z_intellectual:
        return next_img_az_intellectual();
    case NextImgOrder::time:
        return next_img_az();
    case NextImgOrder::random:
        {
            QStringList files = curr_dir.entryList(params.file_filter);
            if(files.isEmpty()) return "";
            auto file = curr_dir.absoluteFilePath(files[rnd.bounded(files.size())]);
            last_file_info = QFileInfo(file);
            return file;
        }
    }
    return "";
}

QString MainWindow::next_img_az()
{
    return !params.is_reverse ?
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return a.fileName() < b.fileName();}) :
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return a.fileName() > b.fileName();}) ;
}

QString MainWindow::next_img_az_intellectual()
{
    return !params.is_reverse ?
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return intellectual_names_comparsion(a.fileName(), b.fileName());}) :
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return intellectual_names_comparsion(b.fileName(), a.fileName());}) ;
}

QString MainWindow::next_img_time()
{
    return !params.is_reverse ?
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return a.birthTime() < b.birthTime();}) :
                get_next__wrapper([](const QFileInfo& a, const QFileInfo& b){return a.birthTime() > b.birthTime();}) ;
}

