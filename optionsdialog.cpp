#include <QIcon>
#include <QColorDialog>

#include "optionsdialog.h"
#include "ui_optionsdialog.h"

OptionsDialog::OptionsDialog(QWidget *parent) :
    QDialog(parent),
    dlg_strips_color(Qt::black, this),
    get_img_for_strips(this, tr("Choose image"), "~/", params.image_files_str()),
    dlg_strips_img(this, tr("Image for margins"), "~/", params.image_files_str()),
    ui(new Ui::OptionsDialog)
{
    ui->setupUi(this);
    ui->btn_order->addItem("🕗", static_cast<int>(NextImgOrder::time));
    ui->btn_order->addItem("A...z", static_cast<int>(NextImgOrder::a_z));
    ui->btn_order->addItem("A...z 🧠", static_cast<int>(NextImgOrder::a_z_intellectual));
    ui->btn_order->addItem("🎲", static_cast<int>(NextImgOrder::random));
    ui->btn_delay_time->setMinimum(double(GalleryParams::min_delay) / 10);
    dlg_strips_color.setOption(QColorDialog::ShowAlphaChannel);
    dlg_strips_color.setOption(QColorDialog::DontUseNativeDialog);
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
   switch(params.aspect_ratio.set_aspect_ratio)
   {
   case KeepAspectRatioVariant::dont_keep : ui->btn_no_aspect_ratio->setChecked(true); break;
   case KeepAspectRatioVariant::add_margins : ui->btn_keep_aspect_ratio_by_strips->setChecked(true); break;
   case KeepAspectRatioVariant::resize_window : ui->btn_keep_aspect_ratio_by_resize->setChecked(true); break;
   }
   ui->btn_strips_inframe_mode->setChecked(params.aspect_ratio.margins.using_mg == Margins::pave);
   ui->btn_true_frame_sz->setValue(params.aspect_ratio.margins.equal_width_margins_sz);

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
    KeepAspectRatioVariant a_ratio = KeepAspectRatioVariant::dont_keep;
    if(ui->btn_keep_aspect_ratio_by_strips->isChecked()) a_ratio = KeepAspectRatioVariant::add_margins;
    if(ui->btn_keep_aspect_ratio_by_resize->isChecked()) a_ratio = KeepAspectRatioVariant::resize_window;
    params.aspect_ratio.set_aspect_ratio = a_ratio;
    params.aspect_ratio.margins.equal_width_margins_sz = ui->btn_true_frame_sz->value();
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

void OptionsDialog::on_btn_select_img_for_strips_clicked()
{
    auto pallete = ui->btn_select_img_for_strips->palette();
    pallete.setColor(QPalette::Button, pallete.color(QPalette::Button));
    ui->btn_select_color_for_strips->setPalette(pallete);
    QIcon ico(get_img_for_strips.get_icon());
    if(ico.isNull()) return;
    ui->btn_select_img_for_strips->setIcon(ico);
    on_btn_strips_inframe_mode_toggled(false);
}

void OptionsDialog::on_btn_select_color_for_strips_clicked()
{
    ui->btn_select_img_for_strips->setIcon(QIcon());
    dlg_strips_color.exec();
    auto pallete = ui->btn_select_color_for_strips->palette();
    auto color = dlg_strips_color.selectedColor();
    params.aspect_ratio.margins.using_mg = Margins::margin_color_only;
    pallete.setColor(QPalette::Button, color);
    ui->btn_select_color_for_strips->setPalette(pallete);
    params.aspect_ratio.margins.color = color;
    QPalette palette = ui->lbl_strips->palette();
    palette.setColor(ui->lbl_strips->foregroundRole(), color);
    ui->lbl_strips->setPalette(palette);

}

void OptionsDialog::on_btn_no_aspect_ratio_toggled(bool)
{
    ui->lt_strips_options->setVisible(false);
}

void OptionsDialog::on_btn_keep_aspect_ratio_by_resize_toggled(bool)
{
   ui->lt_strips_options->setVisible(false);
}

void OptionsDialog::on_btn_keep_aspect_ratio_by_strips_toggled(bool)
{
    ui->lt_strips_options->setVisible(true);
}


void OptionsDialog::on_btn_strips_inframe_mode_toggled(bool)
{
    params.aspect_ratio.margins.using_mg = ui->btn_strips_inframe_mode->isChecked() ?
                Margins::as_frame : Margins::pave;
}

void OptionsDialog::on_btn_true_frame_toggled(bool)
{
    params.aspect_ratio.margins.equal_width_margins_sz = ui->btn_true_frame_sz->value();
    ui->lt_strips_options->setVisible(true);
}
