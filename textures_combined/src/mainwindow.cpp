#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // 创建QOpenGLWidget实例
    texture_widget = new TextureWidget();

    // 将QOpenGLWidget设置为主窗口的中央部件
    setCentralWidget(texture_widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
