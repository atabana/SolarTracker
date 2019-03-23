#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>

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
    // plots
    void makeVoltagePlot();
    void makeCurrentPlot();

    // serial
    void printPortsInfo();
    void setupSerial();
    void readSerial();

    // sensors
    void updateSensor1(QString);
    void updateSensor2(QString);
    void updateSensor3(QString);
    void updateSensor4(QString);

private:
    Ui::MainWindow *ui;

    QSerialPort *fpga;
    static const quint16 fpga_vendor_id = 9025;
    static const quint16 fpga_product_id = 67;
    QByteArray serialData;
    QString serialBuffer;
    QString parsed_data;
    double sensor_1_value;
    double sensor_2_value;
    double sensor_3_value;
    double sensor_4_value;

};

#endif // MAINWINDOW_H
