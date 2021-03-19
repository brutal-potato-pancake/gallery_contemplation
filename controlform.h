#ifndef CONTROLFORM_H
#define CONTROLFORM_H

#include <QWidget>

namespace Ui {
    class ControlForm;
}

class MainWindow;
class ControlForm : public QWidget
{
    Q_OBJECT
    MainWindow* controlled_widget;
    double opacity = 1.0f;

public:
    explicit ControlForm(MainWindow *parent = nullptr);
    ~ControlForm();


private slots:

    void on_btn_exit_clicked();

    void on_btn_menu_clicked();

    void on_btn_forward_clicked();

    void on_btn_pause_clicked();

    void on_btn_back_clicked();

    void on_btn_info_clicked();


    void on_btn_opacity_clicked();

private:
    Ui::ControlForm *ui;
    void pause_on_go_back_because_img_soon_go_foward_when_delay_is_low();
};

#endif
