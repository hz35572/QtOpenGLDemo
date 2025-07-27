#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opengl_widget = new QOpenGLRenderWidget();
    setCentralWidget(opengl_widget);
}

MainWindow::~MainWindow()
{
    delete ui;
}
