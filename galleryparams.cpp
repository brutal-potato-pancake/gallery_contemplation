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
