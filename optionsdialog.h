#ifndef OPTIONSDIALOG_H
#define OPTIONSDIALOG_H
#include <algorithm>
#include <QDialog>
#include <QColorDialog>
#include <QFileDialog>
#include <QMap>

#include "galleryparams.h"

namespace Ui {
class OptionsDialog;
}

inline int transparency_to_opacity(int transparency_0_to_99)
{
    transparency_0_to_99 = std::min(transparency_0_to_99, 99);
    transparency_0_to_99 = std::max(transparency_0_to_99, 0);
    return 100 - transparency_0_to_99;
}

class OptionsDialog final: public QDialog
{
    Q_OBJECT

public:
    explicit OptionsDialog(QWidget *parent = nullptr);
    ~OptionsDialog();
    QColorDialog dlg_strips_color;
    GetFile get_img_for_strips;
    QFileDialog  dlg_strips_img;

    void set_params();
    static OptionsDialog *exec(const GalleryParams &params, bool just_get = false);
private slots:
    void on_btn_ok_cancel_accepted();

    void on_btn_ok_cancel_helpRequested();

    void on_btn_ok_cancel_rejected();

    void on_btn_delay_time_valueChanged(double arg1);

    void on_btn_sz_y_valueChanged(int arg1);

    void on_btn_sz_x_valueChanged(int arg1);

    void on_btn_cycle_toggled(bool checked);
    void on_btn_order_currentIndexChanged(int index);



    void on_label_4_linkHovered(const QString &link);

    void on_btn_keep_aspect_ratio_toggled(bool checked);

    void on_btn_sit_in_tray_toggled(bool checked);

    void on_btn_allow_only_1_toggled(bool checked);

    void on_btn_main_window_opacity_valueChanged(int arg1);

    void on_btn_select_img_for_strips_clicked();

    void on_btn_select_color_for_strips_clicked();

    void on_btn_no_aspect_ratio_toggled(bool);

    void on_btn_keep_aspect_ratio_by_resize_toggled(bool);

    void on_btn_keep_aspect_ratio_by_strips_toggled(bool);


    void on_btn_strips_inframe_mode_toggled(bool);

    void on_btn_true_frame_toggled(bool checked);

private:
    Ui::OptionsDialog *ui;
    GalleryParams params;
    QString image;

signals:
};

#endif
