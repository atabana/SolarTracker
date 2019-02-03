#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::makeVoltagePlot();
    MainWindow::makeCurrentPlot();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::makeVoltagePlot()
{

    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:

    ui->voltagePlot->addGraph();
    ui->voltagePlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->voltagePlot->xAxis->setLabel("time");
    ui->voltagePlot->yAxis->setLabel("voltage");
    // set axes ranges, so we see all data:
    ui->voltagePlot->xAxis->setRange(0, 5);
    ui->voltagePlot->yAxis->setRange(0, 5);
    ui->voltagePlot->replot();

}

void MainWindow::makeCurrentPlot()
{

    QVector<double> x(101), y(101); // initialize with entries 0..100
    for (int i=0; i<101; ++i)
    {
      x[i] = i/50.0; // x goes from -1 to 1
      y[i] = x[i]*x[i]; // let's plot a quadratic function
    }
    // create graph and assign data to it:
    ui->chargePlot->addGraph();
    ui->chargePlot->graph(0)->setData(x, y);
    // give the axes some labels:
    ui->chargePlot->xAxis->setLabel("time");
    ui->chargePlot->yAxis->setLabel("charge");
    // set axes ranges, so we see all data:
    ui->chargePlot->xAxis->setRange(0, 5);
    ui->chargePlot->yAxis->setRange(0, 5);
    ui->chargePlot->replot();

}
