#include <QListWidget>
#include <QGraphicsOpacityEffect>
#include <QApplication>

#include "controlform.h"
#include "ui_controlform.h"
#include "mainwindow.h"


ControlForm::ControlForm(MainWindow *parent) :
    QWidget(parent),
    ui(new Ui::ControlForm)
{
    ui->setupUi(this);
    controlled_widget = parent;
}

ControlForm::~ControlForm()
{
    delete ui;
}


void ControlForm::on_btn_exit_clicked()
{
    QApplication::exit();
}

void ControlForm::on_btn_menu_clicked()
{
    controlled_widget->call_menu(QWidget::mapToGlobal(ui->btn_menu->geometry().bottomLeft()));
}

void ControlForm::on_btn_forward_clicked()
{
    controlled_widget->show_next_img();
}

void ControlForm::on_btn_pause_clicked()
{
    controlled_widget->set_pause(!controlled_widget->get_pause());
}

void ControlForm::on_btn_back_clicked()
{
    controlled_widget->show_prev_img();
    pause_on_go_back_because_img_soon_go_foward_when_delay_is_low();
}

void ControlForm::on_btn_info_clicked()
{
    controlled_widget->show_info();
}

void ControlForm::pause_on_go_back_because_img_soon_go_foward_when_delay_is_low()
{
    controlled_widget->set_pause(true);
}


void ControlForm::on_btn_opacity_clicked()
{
    QList<QWidget*> btns_list = {ui->btn_info, ui->btn_back, ui->btn_pause, ui->btn_forward, ui->btn_fullscreen, ui->btn_opacity, ui->btn_menu, ui->btn_exit};
    opacity -= 0.3;
    if(opacity < 0.3) opacity = 1.0f;

    for(auto i: btns_list)
    {
        QGraphicsOpacityEffect* opacity_effect = new QGraphicsOpacityEffect; //effect works only on 1 widget, don't ask me, why. Acc. to  Qt 6.0 docs: QWidget::setGraphicsEffect
        opacity_effect->setOpacity(opacity);
        i->setGraphicsEffect(opacity_effect);
        i->setAutoFillBackground(true);
        i->update();
    }
}
