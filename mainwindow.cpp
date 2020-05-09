#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButton_clicked()
{
    // 要弹出的中心点
    QPoint center = ui->pushButton->geometry().center();
    JellyButtonBox* jbb = new JellyButtonBox(this);
    QList<QPixmap> icons = {QPixmap(":/icons/icon1"), QPixmap(":/icons/icon2"), QPixmap(":/icons/icon3")};
    jbb->setButtons(icons);
    jbb->exec(center, QPoint(center.x(), center.y()-64));
}
