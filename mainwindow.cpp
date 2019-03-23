#include "mainwindow.h"
#include "ui_mainwindow.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    MainWindow::makeVoltagePlot();
    MainWindow::makeCurrentPlot();
    MainWindow::printPortsInfo();
    MainWindow::setupSerial();

}

MainWindow::~MainWindow()
{
    if(fpga->isOpen()){
        fpga->close(); //    Close the serial port if it's open.
    }
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
    ui->voltagePlot->xAxis->setRange(0, 20);
    ui->voltagePlot->yAxis->setRange(0, 20);
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

/*
 *  Testing code, prints the description, vendor id, and product id of all ports.
 *  Used to determine the values for the FPGA.
 *
 *
 *
 **/
void MainWindow::printPortsInfo()
{

    qDebug() << "Number of ports: " << QSerialPortInfo::availablePorts().length() << "\n";

    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        qDebug() << "***************************************************\n";
        qDebug() << "Description: " << serialPortInfo.description() << "\n";
        qDebug() << "Has vendor id?: " << serialPortInfo.hasVendorIdentifier() << "\n";
        qDebug() << "Vendor ID: " << serialPortInfo.vendorIdentifier() << "\n";
        qDebug() << "Has product id?: " << serialPortInfo.hasProductIdentifier() << "\n";
        qDebug() << "Product ID: " << serialPortInfo.productIdentifier() << "\n";
        qDebug() << "***************************************************\n";
    }
}

void MainWindow::setupSerial()
{
    ui->lcdSensor_1->display("-------");
    ui->lcdSensor_2->display("-------");
    ui->lcdSensor_3->display("-------");
    ui->lcdSensor_4->display("-------");

    fpga = new QSerialPort(this);
    serialBuffer = "";
    parsed_data = "";
    sensor_1_value = 0.0;
    sensor_2_value = 0.0;
    sensor_3_value = 0.0;
    sensor_4_value = 0.0;


    /*
     *   Identify the port the arduino uno is on.
     */
    bool fpga_is_available = false;
    QString fpga_port_name;
    //
    //  For each available serial port
    foreach(const QSerialPortInfo &serialPortInfo, QSerialPortInfo::availablePorts()){
        //  check if the serialport has both a product identifier and a vendor identifier
        if(serialPortInfo.hasProductIdentifier() && serialPortInfo.hasVendorIdentifier()){
            //  check if the product ID and the vendor ID match those of the fpga
            if((serialPortInfo.productIdentifier() == fpga_product_id)
                    && (serialPortInfo.vendorIdentifier() == fpga_vendor_id)){
                fpga_is_available = true; //    arduino uno is available on this port
                fpga_port_name = serialPortInfo.portName();
            }
        }
    }

    /*
     *  Open and configure the fpga port if available
     */
    if(fpga_is_available){
        qDebug() << "Found the arduino port...\n";
        fpga->setPortName(fpga_port_name);
        fpga->open(QSerialPort::ReadOnly);
        fpga->setBaudRate(QSerialPort::Baud9600);
        fpga->setDataBits(QSerialPort::Data8);
        fpga->setFlowControl(QSerialPort::NoFlowControl);
        fpga->setParity(QSerialPort::NoParity);
        fpga->setStopBits(QSerialPort::OneStop);
        QObject::connect(fpga, SIGNAL(readyRead()), this, SLOT(readSerial()));
    }else{
        qDebug() << "Couldn't find the correct port for the fpga.\n";
        QMessageBox::information(this, "Serial Port Error", "Couldn't open serial port to fpga.");
    }

}

void MainWindow::readSerial()
{
    /*
     * readyRead() doesn't guarantee that the entire message will be received all at once.
     * The message can arrive split into parts.  Need to buffer the serial data and then parse for the temperature value.
     *
     */
    QStringList buffer_split = serialBuffer.split(","); //  split the serialBuffer string, parsing with ',' as the separator

    //  Check to see if there less than 3 tokens in buffer_split.
    //  If there are at least 3 then this means there were 2 commas,
    //  means there is a parsed temperature value as the second token (between 2 commas)
    if(buffer_split.length() < 3){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = fpga->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        // the second element of buffer_split is parsed correctly, update the temperature value on
        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        parsed_data = buffer_split[1];
        sensor_1_value = (9/5.0) * (parsed_data.toDouble()) + 32; // convert to fahrenheit
        qDebug() << "Temperature: " << sensor_1_value << "\n";
        parsed_data = QString::number(sensor_1_value, 'g', 4); // format precision of temperature_value to 4 digits or fewer
        MainWindow::updateSensor1(parsed_data);
    }

}

void MainWindow::updateSensor1(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdSensor_1->display(sensor_reading);
}

void MainWindow::updateSensor2(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdSensor_2->display(sensor_reading);
}

void MainWindow::updateSensor3(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdSensor_3->display(sensor_reading);
}

void MainWindow::updateSensor4(QString sensor_reading)
{
    //  update the value displayed on the lcdNumber
    ui->lcdSensor_4->display(sensor_reading);
}
