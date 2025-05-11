#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "texturewidget.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    TextureWidget *texture_widget;
};
#endif // MAINWINDOW_H
