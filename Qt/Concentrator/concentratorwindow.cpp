#include "concentratorwindow.h"
#include "ui_concentratorwindow.h"
#include <QApplication>
#include <QDebug>
#include <QTextStream>
#include <QDir>
#include <QDateTime>
#include <QFile>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>

ConcentratorWindow::ConcentratorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConcentratorWindow)
{
    QPixmap redled (":/ledred.png");

    ui->setupUi(this);
    power_state = 0;
    ui->PowerON_pushButton->setEnabled(false);
    ui->Power_label->setPixmap(redled);
    ui->data_frame->setEnabled(false);
    ui->program_frame->setEnabled(false);
    wait_reply_var = WAIT_REPLY;
    timer0Id = 0;
}

ConcentratorWindow::~ConcentratorWindow()
{
    delete ui;
}

QByteArray ConcentratorWindow::serial_tx( QByteArray hex_line)
{
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");
QByteArray reply;
    if ( serial_started == 0 )
        return "1";
    serial.flush();
    serial.write(hex_line);
    if(serial.waitForReadyRead(wait_reply_var))
    {
        reply = serial.readAll();
        if ( reply[1] == 'Y')
        {
            packets++;
            QString result;
            result = QString::number(packets).rightJustified(8, '0');
            ui->RX_label->setPixmap(greenled);
        }
        if ( reply[1] == 'E')
        {
            packets++;
            QString result;
            result = QString::number(packets).rightJustified(8, '0');
            ui->RX_label->setPixmap(greenled);
            ui->Flashing_label->setPixmap(greenled);
            qDebug()<< "Finished ";
        }
        else if ( reply[1] == 'T')
        {
            packets++;
            QString result;
            result = QString::number(packets).rightJustified(8, '0');
            ui->RX_label->setPixmap(greenled);
            qDebug()<< "File name accepted " << hex_line;
        }
        else if ( reply[1] == 'N')
        {
            packets_errors++;
            ui->RX_label->setPixmap(redled);
            qDebug()<< "Error on " << hex_line;
        }
        else if ( reply[1] == 'W')
        {
            packets_errors++;
            ui->RX_label->setPixmap(redled);
            qDebug()<< "Write Error";
        }
        else
            ui->RX_label->setPixmap(redled);
        //qDebug()<< reply;
        return reply;
    }
    else
    {
        serial.flush();
        qDebug()<< "no reply @ " << hex_line;
    }
    qApp->processEvents();

    return "K";
}

void ConcentratorWindow::on_Port_comboBox_currentTextChanged(const QString &arg1)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    serial.close();
    serial_started = 0;
    serial.setPortName(arg1);
    if(serial.open(QIODevice::ReadWrite))
    {
        if(!serial.setBaudRate(QSerialPort::Baud115200))
        {
            ui->Comm_label->setPixmap(redled);
            qDebug()<< arg1 << " : " << serial.errorString();
            ui->statusbar->showMessage(arg1+" : "+serial.errorString());
            ui->PowerON_pushButton->setEnabled(false);
        }
        else
        {
            ui->Comm_label->setPixmap(greenled);
            serial_started = 1;
            qDebug()<< "Serial port opened";
            ui->statusbar->showMessage(arg1+" : Serial port opened");
            serial.setReadBufferSize (1024);
            ui->PowerON_pushButton->setEnabled(true);
        }
    }
    else
    {
        ui->Comm_label->setPixmap(redled);
        qDebug()<< arg1 << " : " << serial.errorString();
        ui->statusbar->showMessage(arg1+" : "+serial.errorString());
        ui->PowerON_pushButton->setEnabled(false);
    }
}

void ConcentratorWindow::on_Exit_pushButton_clicked()
{
    serial.close();
    close();
}

void ConcentratorWindow::on_DownloadBinary_pushButton_clicked()
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    QByteArray hex_line,reply,fsizeQ;
    folder =  "/Devel/Stm32_16.0_A_os_2024.09-rc/Membrane-2412171-00-Sensor/Debug/";
    filename = "Membrane-2412171-00-Sensor.hex";
    fileversion = "1.0.0rc0";
    ui->statusbar->showMessage("Downloading "+filename);

    ui->Flashing_label->setPixmap(redled);

    ui->FlashBinary_pushButton->setEnabled(false);
    ui->Program_pushButton->setEnabled(false);
    ui->Line_comboBox->setEnabled(false);
    ui->Sensor_comboBox->setEnabled(false);

    QFile file(folder+filename);
    int fsize,s_unit;

    serial.flush();

    if (!file.open(QIODevice::ReadOnly))
    {
        qDebug()<<"File not found";
        return ;
    }
    fsize = file.size();
    s_unit = fsize/100;

    fsizeQ.setNum(fsize);
    QByteArray Details = "File " + filename.toUtf8() +" Version "+fileversion.toUtf8() +" Size "+fsizeQ;
    qDebug()<<Details;
    QByteArray Title = ":T "+filename.toUtf8() +" "+fileversion.toUtf8() +" "+fsizeQ+"\r\n";
    while ( (reply = serial_tx(Title)) == "K" )
    {
        qDebug()<<reply;
    }

    while ( fsize)
    {
        hex_line = file.readLine();
        if ( (reply = serial_tx(hex_line)) != "1" )
        {
            //qDebug()<<hex_line;
        }
        fsize -= hex_line.length();
        ui->download_progressBar->setValue(100-fsize/s_unit);
    }

    file.close();
    ui->statusbar->showMessage(filename+" downloaded");
    ui->FlashBinary_pushButton->setEnabled(true);
    ui->Program_pushButton->setEnabled(true);
    ui->Line_comboBox->setEnabled(true);
    ui->Sensor_comboBox->setEnabled(true);
}

void ConcentratorWindow::on_GetInfo_pushButton_clicked()
{
QByteArray reply;
QByteArray Command;
    Command = "<G "+ui->Getinfo_comboBox->currentText().toUtf8()+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
        ui->statusbar->showMessage(reply);
        if ( reply[5] != 'U')
        {
            ui->Program_pushButton->setEnabled(true);
            ui->FlashBinary_pushButton->setEnabled(true);
            ui->Line_comboBox->setEnabled(true);
            ui->Sensor_comboBox->setEnabled(true);
        }
        else
        {
            ui->Program_pushButton->setEnabled(false);
            ui->FlashBinary_pushButton->setEnabled(false);
            ui->Line_comboBox->setEnabled(false);
            ui->Sensor_comboBox->setEnabled(false);
        }
    }
}

void ConcentratorWindow::on_Program_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
        Command = "<W "+ui->Program_comboBox->currentText().toUtf8()+">";
        qDebug()<<Command;
        if ( (reply = serial_tx(Command)) != "1" )
        {
            qDebug()<<reply;
        }
}

void ConcentratorWindow::on_FlashBinary_pushButton_clicked()
{
QByteArray reply;
QByteArray Command;
QByteArray line;
QByteArray sensor;
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");

    wait_reply_var = WAIT_REPLY_DOWNLOAD;

    ui->SensorInFlash_label->setPixmap(redled);
    qApp->processEvents();
    line = ui->Line_comboBox->currentText().toUtf8();
    sensor = ui->Sensor_comboBox->currentText().toUtf8();
    if ( ui->Line_comboBox->currentText() == "All")
        line = "255";
    if ( ui->Sensor_comboBox->currentText() == "All")
        sensor = "255";

    Command = "<F "+line+" "+sensor+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
        ui->SensorInFlash_label->setPixmap(greenled);
        qApp->processEvents();
    }
    wait_reply_var = WAIT_REPLY;
}

void ConcentratorWindow::on_PowerON_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    if ( power_state == 0 )
    {
        Command = "<P>";
        ui->PowerON_pushButton->setText("Power OFF");
        ui->Power_label->setPixmap(greenled);
        ui->data_frame->setEnabled(true);
        ui->program_frame->setEnabled(true);
    }
    else
    {
        ui->PowerON_pushButton->setText("Power ON");
        Command = "<O>";
        ui->Power_label->setPixmap(redled);
        ui->data_frame->setEnabled(false);
        ui->program_frame->setEnabled(false);
    }

    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        power_state ++;
        power_state &= 1;

        qDebug()<<reply;
    }
}

void ConcentratorWindow::on_Discovery_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    Command = "<Z>";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
}

void ConcentratorWindow::on_GetMap_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;

    line = ui->GetMapLine_comboBox->currentText().toUtf8();
    Command = "<M "+line+">";
    qDebug()<<Command;

    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
}

void ConcentratorWindow::on_GetData_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;
    QByteArray sensor;
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

        line = ui->GetDataLine_comboBox->currentText().toUtf8();
        sensor = ui->GetDataSensor_comboBox->currentText().toUtf8();

        Command = "<A "+line+" "+sensor+">";
        //qDebug()<<Command;
        if ( (reply = serial_tx(Command)) != "1" )
        {
            qDebug()<<reply;
            ui->SensorInFlash_label->setPixmap(greenled);
            qApp->processEvents();
        }
        wait_reply_var = WAIT_REPLY;
}

int toggle=0;

void ConcentratorWindow::on_Scan_pushButton_clicked()
{


    if ( ui->Scan_pushButton->text() == "Stop")
    {
        ui->Scan_pushButton->setText("Scan");
        killTimer(timer0Id);
        ui->statusbar->showMessage("Stopped");
        ui->setScanTime_comboBox->setEnabled(true);
        ui->sec_scan_label->setEnabled(true);
        ui->scantime_label->setEnabled(true);
    }
    else
    {
        QString ll = ui->setScanTime_comboBox->currentText();
        int llint = ll.toInt();
        ui->setScanTime_comboBox->setEnabled(false);
        ui->sec_scan_label->setEnabled(false);
        ui->scantime_label->setEnabled(false);

        ui->Scan_pushButton->setText("Stop");
        timer0Id = startTimer(llint*1000);
        toggle=0;
        ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec -");
    }
}

void ConcentratorWindow::timerEvent(QTimerEvent *event)
{
    QByteArray reply;
    QByteArray Command;
    if ( event->timerId() == timer0Id )
    {
        toggle ++;
        toggle &= 3;
        if ( toggle == 0 )
            ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec -");
        else if ( toggle == 1 )
            ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec \\");
        else if ( toggle == 2 )
            ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec |");
        else if ( toggle == 3 )
            ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec /");

        //qDebug()<<"Timer";

        Command = "<S>";
        //qDebug()<<Command;

        if ( (reply = serial_tx(Command)) != "1" )
        {
            //qDebug()<<reply;
        }
        Command = "<A 1 5>";
        if ( (reply = serial_tx(Command)) != "1" )
        {
            qDebug()<<reply;
        }


    }
}
