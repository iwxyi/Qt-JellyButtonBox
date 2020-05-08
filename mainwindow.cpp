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
    QPoint center = mapFromGlobal(QCursor::pos());
    JellyButtonBox* jbb = new JellyButtonBox(this);
    jbb->exec(center, QPoint(center.x(), center.y()-80));
}
