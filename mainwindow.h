#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void makeVoltagePlot();
    void makeCurrentPlot();
private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
