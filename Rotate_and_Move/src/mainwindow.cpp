#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "openglrenderwidget.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    opengl_widget = new OpenGLRenderWidget();
    setCentralWidget(opengl_widget);
}

MainWindow::~MainWindow()
{
    delete ui;
    delete opengl_widget;
}
