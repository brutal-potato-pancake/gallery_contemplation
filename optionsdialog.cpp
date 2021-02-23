#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->btn_order->addItem("🕗", static_cast<int>(NextImgOrder::time));
    ui->btn_order->addItem("A...z", static_cast<int>(NextImgOrder::a_z));
    ui->btn_order->addItem("A...z 🧠", static_cast<int>(NextImgOrder::a_z_intellectual));
    ui->btn_order->addItem("🎲", static_cast<int>(NextImgOrder::random));
    ui->btn_delay_time->setMinimum(double(GalleryParams::min_delay) / 10);
}

OptionsDialog::~OptionsDialog()
{
    delete ui;
}

void OptionsDialog::set_params()
{
   ui->btn_sz_x->setValue(params.pos.x());
   ui->btn_sz_y->setValue(params.pos.y());
   ui->btn_cycle->setChecked(params.cycled);
   ui->btn_cycle->setChecked(params.manually());
   ui->btn_order->setCurrentIndex(static_cast<int>(params.order));
   ui->btn_delay_time->setValue(double(params.delay_as_100_ms) / 10);
   if(!ui->btn_keep_aspect_ratio->isChecked()) params.curr_width = 0.0f;
   else if(!params.dont_keep_aspect_ratio()) params.set_aspect_ratio = true;

}

void OptionsDialog::on_btn_ok_cancel_accepted()
{
    params.cycled = ui->btn_cycle->isChecked();
    params.delay_as_100_ms = ui->btn_delay_time->value() * 10;
    params.is_manually = ui->btn_manually->isChecked();
    params.order = static_cast<NextImgOrder>(ui->btn_order->currentData().toInt());
    params.is_reverse = ui->btn_reverse_order->isChecked();
    params.pos = QPoint(ui->btn_sz_x->value(), ui->btn_sz_y->value());
    params.img_opacity = transparency_to_opacity(ui->btn_main_window_opacity->value());
    params.update_source();
    close();
}

void OptionsDialog::on_btn_ok_cancel_helpRequested()
{

}

void OptionsDialog::on_btn_ok_cancel_rejected()
{
    close();
}

void OptionsDialog::on_btn_delay_time_valueChanged(double arg1)
{
    auto delay_as_100_ms = arg1 * 10;
    ui->btn_manually->setEnabled(delay_as_100_ms >= params.min_delay);
    if(delay_as_100_ms < params.min_delay)
    {
        ui->btn_manually->setChecked(true);
    }
}

void OptionsDialog::on_btn_sz_y_valueChanged(int arg1)
{
    params.set_y(arg1);
}

void OptionsDialog::on_btn_sz_x_valueChanged(int arg1)
{
    params.set_x(arg1);
}

void OptionsDialog::on_btn_cycle_toggled(bool checked)
{

}

void OptionsDialog::on_btn_order_currentIndexChanged(int index)
{

}

OptionsDialog* OptionsDialog::exec(const GalleryParams & _params, bool just_get)
{
    static OptionsDialog* dlg = new OptionsDialog();
    if(!just_get)
    {
        dlg->params = _params;
        dlg->set_params();
        dlg->show();
    }
    return dlg;
}

void OptionsDialog::on_label_4_linkHovered(const QString &link)
{

}

void OptionsDialog::on_btn_keep_aspect_ratio_toggled(bool checked)
{

}

void OptionsDialog::on_btn_sit_in_tray_toggled(bool checked)
{
   ui->btn_allow_only_1->setChecked(checked);

}

void OptionsDialog::on_btn_allow_only_1_toggled(bool checked)
{
   if(!checked) ui->btn_sit_in_tray->setChecked(false);
}

void OptionsDialog::on_btn_main_window_opacity_valueChanged(int arg1)
{

}
