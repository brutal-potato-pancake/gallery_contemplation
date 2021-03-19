#include <QPainter>
#include "galleryparams.h"

void GalleryParams::set_x(int x)
{
    pos.setX(x);
    sender->set_pos(pos);
}

void GalleryParams::set_y(int y)
{
    pos.setY(y);
    sender->set_pos(pos);
}

void GalleryParams::update_source()
{
    if(sender) emit sender->params_accepted(this);
}

void ParamSender::set_pos(QPoint p)
{
    emit signal_set_pos(p);
}

QPixmap KeepAspectRatio::fit_img_to_widget(const QPixmap &source_img, QSize size_for_fit)
{
    return source_img.scaledToHeight(size_for_fit.height());
    return source_img.scaledToWidth(size_for_fit.width());
}

QPixmap KeepAspectRatio::resize(const QPixmap& source_pixmap)
{
    if(!w) return new_pixmap = source_pixmap;
    auto sz = w->size();
    if(sz.width() < 1 || sz.height() < 1) return new_pixmap = source_pixmap;

    double x = sz.width(), y =  sz.height();
    double aspect_ratio = y / x;
    switch (set_aspect_ratio) {
    case KeepAspectRatioVariant::dont_keep:
        break;
    case KeepAspectRatioVariant::resize_window:
        w->resize(w->size().width(), w->size().width() * aspect_ratio);
        break;
    case KeepAspectRatioVariant::add_margins:
        return new_pixmap = QPixmap(add_margins(source_pixmap));
    }
    return new_pixmap = QPixmap(source_pixmap.scaled(w->size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation));
}

QPixmap KeepAspectRatio::add_margins(const QPixmap &img, QSize new_size, C_Margins margins)
{
    if(margins == nullptr) return img;

    Margins::UseImg margins_usage;
    if(!*margins) margins_usage = Margins::margin_color_only;
    else margins_usage = margins->using_mg;
    auto equal_width_margins_sz = margins->type == FrameParams::Type::normal ? 0 : margins->equal_width_margins_sz;
    FrameParams frame(img.size(), new_size, equal_width_margins_sz);
    QPixmap ret = paint_img(frame, img);
    paint_with_color(frame, ret, margins->color);
    switch (margins_usage) {
    case Margins::margin_color_only:
        return paint_with_color(frame, ret, margins->color);
    case Margins::as_frame:
        return paint_frame(frame, ret, margins->img);
    case Margins::pave:
        return paint_with_pave(frame, ret, margins->img);
    }
    return ret;
}

QPixmap &KeepAspectRatio::add_margins(const QPixmap& source_pixmap)
{
    return new_pixmap = !*this ? source_pixmap : add_margins(source_pixmap, w->size(), &margins);
}

void KeepAspectRatio::paint_for_pave(QPixmap &ret, QRect rect, const QPixmap& for_pave)
{
    QPainter painter(&ret);
    int i = 0, j = 0;
    auto w = for_pave.size().width();
    auto h = for_pave.size().height();
    for(i = 0; i < rect.size().height() - h; i+= h)
    {
        for(j = 0; j < rect.size().width() - w; j+= w)
        {
            painter.drawPixmap(QPoint(i, j), for_pave);
        }
        painter.drawPixmap(QRect{QPoint{i, j}, QSize{rect.size().width() - j, h}}, for_pave, {{0, 0}, for_pave.size()});
    }
    for(j = 0; j < rect.size().width() - w; j+= w)
    {
        painter.drawPixmap(QRect{QPoint{i, j}, QSize{rect.size().width() - j, rect.size().height() - h}}, for_pave, {{0, 0}, for_pave.size()});
    }
}

QPixmap &KeepAspectRatio::paint_with_color(const FrameParams &frame, QPixmap &ret, QColor color)
{
    QPainter painter(&ret);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.fillRect(frame.down, color);
    painter.fillRect(frame.top, color);
    painter.fillRect(frame.left, color);
    painter.fillRect(frame.right, color);
    painter.end();
    return ret;
}

QPixmap &KeepAspectRatio::paint_with_pave(const FrameParams &frame, QPixmap &ret, const QPixmap &margins)
{
    decltype(margins) pix_for_pave = margins;
    if(frame.top.height() > 0.0f)
    {
        paint_for_pave(ret, frame.top, pix_for_pave);
        paint_for_pave(ret, frame.down, pix_for_pave);
    }
    else
    {
        paint_for_pave(ret, frame.left, pix_for_pave);
        paint_for_pave(ret, frame.right, pix_for_pave);
    }
    return ret;
}

QPixmap &KeepAspectRatio::paint_frame(const FrameParams &frame, QPixmap &ret, const QPixmap &margins)
{
    QPixmap scaled_margins = margins.scaled(ret.size(), Qt::IgnoreAspectRatio, Qt::SmoothTransformation);
    QPainter painter(&ret);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(frame.down, scaled_margins, frame.down);
    painter.drawPixmap(frame.top, scaled_margins, frame.top);
    painter.drawPixmap(frame.left, scaled_margins, frame.left);
    painter.drawPixmap(frame.right, scaled_margins, frame.right);
    return ret;
}

QPixmap KeepAspectRatio::paint_img(const FrameParams &frame, const QPixmap& src)
{
    QPixmap ret(frame.frame_sz);
    ret.fill();
    QPainter painter(&ret);
    painter.setRenderHint(QPainter::SmoothPixmapTransform);
    painter.drawPixmap(frame.img_rect, src, src.rect());
    painter.end();
    return ret;
}

GetFile::GetFile(QWidget *parent, const QString &caption, const QString &directory, const QString &filter) : file_dialog(parent, caption, directory, filter)
{
    file_dialog.setFileMode(QFileDialog::ExistingFiles);
}

QFile *GetFile::get_file_manual_delete_requared()
{
    destroy_file();
    if(is_exec_and_not_open)
        file_dialog.exec();
    else
        file_dialog.open();
    auto lst = file_dialog.selectedFiles();
    if(lst.isEmpty() )
    {
         return allow_nullptr_as_ret ? nullptr : new QFile();
    }
    auto ret = new QFile(lst[0]);
    if(!ret->open(QFile::ReadOnly))
    {
        return allow_nullptr_as_ret ? nullptr : new QFile();
    }
    return ret;
}

QIcon GetFile::get_icon()
{
    destroy_file();
    if(is_exec_and_not_open)
        file_dialog.exec();
    else
        file_dialog.open();
    auto lst = file_dialog.selectedFiles();
    if(lst.isEmpty() ) return QIcon();
    return QIcon(lst[0]);
}


const QFile* GetFile::get_file_and_delete_old()
{
    destroy_file();
    if(is_exec_and_not_open)
        file_dialog.exec();
    else
        file_dialog.open();
    auto lst = file_dialog.selectedFiles();
    if(lst.isEmpty() )
    {
         return allow_nullptr_as_ret ? nullptr : &void_file;
    }
    ret.reset(new QFile(lst[0]));
    if(!ret->open(QFile::ReadOnly))
    {
        return allow_nullptr_as_ret ? nullptr : &void_file;
    }
    return ret.get();
}

void FrameParams::calc_permanent_frame(QSize frame, uint permanent_frame_sz)
{
   img_rect = QRect( calc_img_in_frame_pos(frame, permanent_frame_sz), calc_img_in_frame_size(frame, permanent_frame_sz));
   calc_margins_by_img_rect(frame);
}

void FrameParams::calc_margins(QSize img_in_frame, QRect w_frame)
{
    auto a_ratio_img = double(img_in_frame.height()) / double(img_in_frame.width());
    double frame_w_x = w_frame.width(), frame_w_y = w_frame.height();
    auto a_ratio_widget = double(frame_w_y) / double(frame_w_x);
    if(a_ratio_widget == a_ratio_img)
    {
        img_rect = QRect(QPoint(0, 0), img_in_frame);
        return;
    }
    bool is_img_taller = a_ratio_img >  a_ratio_widget;
    if(is_img_taller)
    {
        img_rect = QRect(0, 0, w_frame.height() / a_ratio_img, w_frame.height());
        img_rect.moveLeft((w_frame.width() - img_rect.width()) /2);
    }
    if(!is_img_taller)
    {
        img_rect = QRect(0, 0, w_frame.width(), w_frame.width() * a_ratio_img);
        img_rect.moveTop((w_frame.height() - img_rect.height()) /2);
    }
    calc_margins_by_img_rect(w_frame.size());
}

void FrameParams::calc_margins_by_img_rect(QSize frame)
{
    top = QRect{{0, 0}, QSize{frame.width(), img_rect.y()}};
    left = QRect{{0, top.height()}, QSize{img_rect.x(), img_rect.height()}};
    right = QRect({img_rect.topRight().x() + 1, left.y()}, QSize{frame.width() - img_rect.bottomRight().x(), frame.height()});
    down = QRect({0, img_rect.bottomLeft().y() + 1}, QSize{frame.width(), frame.height() - img_rect.bottomLeft().y()});
}


QSize FrameParams::calc_img_in_frame_size(QSize frame, uint frame_sz)
{
    QSize ret;
    ret.setWidth(frame_sz < frame.width() / 2 ? frame.width() - frame_sz : frame.width() / 2);
    ret.setHeight(frame_sz < frame.height() / 2 ?frame.height() - frame_sz : frame.height() / 2);
    return ret;
}

QPoint FrameParams::calc_img_in_frame_pos(QSize frame, uint frame_sz)
{
    QPoint ret;
    ret.setX(frame_sz < frame.width() / 2 ? frame_sz : frame.width() / 2);
    ret.setY(frame_sz < frame.height() / 2 ? frame_sz : frame.height() / 2);
    return ret;
}

FrameParams::FrameParams(QSize img, QSize w_frame, uint permanent_frame_sz)
{
    if(permanent_frame_sz)
        calc_permanent_frame(w_frame, permanent_frame_sz);
    else
        calc_margins(img, QRect({0, 0}, w_frame));
    frame_sz = w_frame;
}
