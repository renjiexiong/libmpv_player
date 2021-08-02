#include "widget.h"
#include "ui_widget.h"
#include <QFileDialog>
#include <QDebug>
Widget::Widget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Widget)
{
    ui->setupUi(this);
    videoPlayer = new mpv_windows_player(this);
    connect(videoPlayer, &mpv_windows_player::changeDuration, this, [=](double duration)
    {
        qDebug()<<"duration"<<duration<<endl;
        ui->horizontalSlider->setRange(0,duration);
    });
    connect(videoPlayer, &mpv_windows_player::changeCurrent, this, [=](double pos)
    {
        //qDebug()<<"pos"<<pos<<endl;
        ui->horizontalSlider->setValue(pos);
    });

}

Widget::~Widget()
{
    delete ui;
}

void Widget::on_pushButton_clicked()
{
    QString file = QFileDialog::getOpenFileName(this, "Open file");
    if(!videoPlayer)
    {
        videoPlayer = new mpv_windows_player();
    }
    videoPlayer->play(ui->label->winId(),file);
}

void Widget::on_pushButton_2_clicked()
{
    static bool flag = false;
    flag = ! flag;
    videoPlayer->pause(flag);
}

void Widget::on_pushButton_3_clicked()
{
    //videoPlayer->setCurrent(0);
    videoPlayer->stop();
}

void Widget::on_pushButton_4_clicked()
{
    if(videoPlayer)
    {
        videoPlayer->play(ui->label->winId(),"");
        videoPlayer = nullptr;
    }

}
