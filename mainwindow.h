#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>

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
    void writeSerial(QString);

    // sensors
    void updateSensor1(QString);
    void updateSensor2(QString);
    void updateSensor3(QString);
    void updateSensor4(QString);

    void on_pushButton_Up_clicked();

    void on_pushButton_Down_clicked();

    void on_pushButton_Left_clicked();

    void on_pushButton_Right_clicked();

    void on_pushButton_Reset_clicked();

    void on_pushButton_FullSweep_clicked();

    void on_pushButton_Track_clicked();

    void on_radioButton_Elevation_clicked();

    void on_radioButton_Azimuth_clicked();

    void realtimePlot_1();

    void realtimePlot_2();

    void on_radioButton_Azimuth_Elevation_clicked();

private:
    Ui::MainWindow *ui;

    QSerialPort *fpga;
    static const quint16 fpga_vendor_id = 1027;
    static const quint16 fpga_product_id = 24592;
    QByteArray serialData;
    QString serialBuffer;
    QString sensors_data_unparsed;
    QStringList sensors_data_list;
    double sensor_1_value;
    double sensor_2_value;
    double sensor_3_value;
    double sensor_4_value;
    double sensor_5_value;
    double sensor_6_value;
    bool fpga_is_available = false;
    QTimer timer_plot_1;
    QTimer timer_plot_2;
    bool clicked = false;

};

#endif // MAINWINDOW_H
