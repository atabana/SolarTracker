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
    //
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
    ui->lcdSensor_1->display("----");
    ui->lcdSensor_2->display("----");
    ui->lcdSensor_3->display("----");
    ui->lcdSensor_4->display("----");
    fpga = new QSerialPort(this);
    serialBuffer = "";
    sensors_data_unparsed = "";
    sensor_1_value = 0.0;
    sensor_2_value = 0.0;
    sensor_3_value = 0.0;
    sensor_4_value = 0.0;
    sensor_5_value = 0.0;
    sensor_6_value = 0.0;

    /*
     *   Identify the port the fpga is on.
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
        qDebug() << "Found the fpga port...\n";
        fpga->setPortName(fpga_port_name);
        fpga->open(QSerialPort::ReadWrite);
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
     * The message can arrive split into parts.  Need to buffer the serial data and then parse for the values.
     *
     */
    QStringList buffer_split = serialBuffer.split("$"); //  split the serialBuffer string, parsing with '$' as the separator

    //  Check to see if there less than 3 tokens in buffer_split.
    //  If there are at least 3 then this means there were 2 @,
    //  means there is a parsed valuse as the second token (between 2 @)
    if(buffer_split.length() < 3){
        // no parsed value yet so continue accumulating bytes from serial in the buffer.
        serialData = fpga->readAll();
        serialBuffer = serialBuffer + QString::fromStdString(serialData.toStdString());
        serialData.clear();
    }else{
        // the second element of buffer_split is parsed correctly, update the value on
        serialBuffer = "";
        qDebug() << buffer_split << "\n";
        sensors_data_unparsed = buffer_split[1];
        sensors_data_list = sensors_data_unparsed.split("-");

        sensor_1_value = sensors_data_list[0].toDouble();
        sensor_2_value = sensors_data_list[1].toDouble();
        sensor_3_value = sensors_data_list[2].toDouble();
        sensor_4_value = sensors_data_list[3].toDouble();
        sensor_5_value = sensors_data_list[4].toDouble();
        sensor_6_value = sensors_data_list[5].toDouble();

        qDebug() << "Sensor 1: " << sensor_1_value << "\n";
        qDebug() << "Sensor 2: " << sensor_2_value << "\n";
        qDebug() << "Sensor 3: " << sensor_3_value << "\n";
        qDebug() << "Sensor 4: " << sensor_4_value << "\n";
        qDebug() << "Sensor 5: " << sensor_5_value << "\n";
        qDebug() << "Sensor 6: " << sensor_6_value << "\n";

        // QString::number(sensor_1_value, 'g', 4); // format precision of value to 4 digits or fewer
        MainWindow::updateSensor1(QString::number(sensor_1_value, 'g', 4));
        MainWindow::updateSensor2(QString::number(sensor_2_value, 'g', 4));
        MainWindow::updateSensor3(QString::number(sensor_3_value, 'g', 4));
        MainWindow::updateSensor4(QString::number(sensor_4_value, 'g', 4));
        MainWindow::updateSensor5(QString::number(sensor_5_value, 'g', 4));
        MainWindow::updateSensor6(QString::number(sensor_6_value, 'g', 4));
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


//TODO
void MainWindow::updateSensor5(QString sensor_5_reading)
{

}
//TODO
void MainWindow::updateSensor6(QString sensor_6_reading)
{

}


void MainWindow::writeSerial(QString command)
{
    if(fpga->isOpen()){
        fpga->write(command.toUtf8());
    }
    else{
        QMessageBox::information(this, "Serial Port Error", "Couldn't send because serial port is not open.");
    }
}



void MainWindow::on_pushButton_Up_clicked()
{
    //QMessageBox::information(this, "Up", "Up Clicked!");
    writeSerial("/U/");

}

void MainWindow::on_pushButton_Down_clicked()
{
    //QMessageBox::information(this, "Down", "Down Clicked!");
    writeSerial("/D/");
}

void MainWindow::on_pushButton_Left_clicked()
{
    //QMessageBox::information(this, "Left", "Left Clicked!");
    writeSerial("/L/");
}

void MainWindow::on_pushButton_Right_clicked()
{
    //QMessageBox::information(this, "Right", "Right Clicked!");
    writeSerial("/R/");
}

void MainWindow::on_pushButton_Reset_clicked()
{
    //QMessageBox::information(this, "Reset", "Reset Clicked!");
    writeSerial("/X/");
}

void MainWindow::on_pushButton_FullSweep_clicked()
{
    //QMessageBox::information(this, "FullSweep", "Full Sweep Clicked!");
    writeSerial("/F/");
}

void MainWindow::on_pushButton_Track_clicked()
{
    //QMessageBox::information(this, "Track", "Track Clicked!");
    writeSerial("/T/");
}

void MainWindow::on_radioButton_Elevation_clicked()
{
    //QMessageBox::information(this, "Elevation", "Elevation Clicked!");
    ui->pushButton_Up->setEnabled(true);
    ui->pushButton_Down->setEnabled(true);
    ui->pushButton_Left->setEnabled(false);
    ui->pushButton_Right->setEnabled(false);
}

void MainWindow::on_radioButton_Azimuth_clicked()
{
    //QMessageBox::information(this, "Azimuth", "Azimuth Clicked!");
    ui->pushButton_Up->setEnabled(false);
    ui->pushButton_Down->setEnabled(false);
    ui->pushButton_Left->setEnabled(true);
    ui->pushButton_Right->setEnabled(true);
}
