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
#include <QFileDialog>
#include <QCoreApplication>
#include <QTextStream>
#include <QThread>

ConcentratorWindow::ConcentratorWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::ConcentratorWindow)
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    ui->setupUi(this);
    power_state = 0;
    special_state = 0;
    ui->PowerON_pushButton->setEnabled(false);
    ui->Power_label->setPixmap(redled);
    ui->data_frame->setEnabled(false);
    ui->program_frame->setEnabled(false);
    ui->StoreToSensors_pushButton->setEnabled(false);
    ui->StartProgramSensors_pushButton->setEnabled(false);
    ui->Special_label->setPixmap(greenled);
    ui->Special_pushButton->setEnabled(false);
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


void ConcentratorWindow::on_GetInfo_pushButton_clicked()
{
QByteArray reply;
QByteArray Command;
    Command = "<G 0>";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
        ui->statusbar->showMessage(reply);
        if ( reply != "Read flash error")
        {
            ui->StoreToSensors_pushButton->setEnabled(true);
            ui->StartProgramSensors_pushButton->setEnabled(true);
            ui->Line_comboBox->setEnabled(true);
            ui->Sensor_comboBox->setEnabled(true);
        }
        else
        {
            ui->StoreToSensors_pushButton->setEnabled(false);
            ui->StartProgramSensors_pushButton->setEnabled(false);
            ui->Line_comboBox->setEnabled(false);
            ui->Sensor_comboBox->setEnabled(false);
        }
    }
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
        ui->Special_pushButton->setEnabled(true);
    }
    else
    {
        ui->PowerON_pushButton->setText("Power ON");
        Command = "<O>";
        ui->Power_label->setPixmap(redled);
        ui->data_frame->setEnabled(false);
        ui->program_frame->setEnabled(false);
        ui->Special_pushButton->setEnabled(false);
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


void ConcentratorWindow::on_StartProgramSensors_pushButton_clicked()
{
QByteArray reply;
QByteArray Command;
QByteArray line;
QByteArray sensor;

    line = ui->ProgramLine_comboBox->currentText().toUtf8();
    sensor = ui->ProgramSensor_comboBox->currentText().toUtf8();
    if ( ui->ProgramLine_comboBox->currentText() == "All")
        line = "255";
    if ( ui->ProgramSensor_comboBox->currentText() == "All")
        sensor = "255";

    Command = "<W "+line+" "+sensor+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
}

void ConcentratorWindow::timerEvent(QTimerEvent *event)
{
    QByteArray reply;
    QByteArray Command;
    int i,j;
    QByteArray qline;
    QByteArray qsensor;
    int dsc,sensor,type,readout,scalefactor,dac,calibration,temperature;

    if ( event->timerId() == timer0Id )
    {
        Command = "<S>";
        serial_tx(Command);

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
        qDebug()<<"***********************";
        wait_reply_var = 50;
        for(i=1;i<5;i++)
        {
            for(j=1;j<9;j++)
            {
                //QThread::msleep(10);
                qline.setNum(i);
                qsensor.setNum(j);
                Command = "<A "+qline+" "+qsensor+">";
                if ( (reply = serial_tx(Command)) != "1" )
                {
                    //qDebug()<<reply;
                    const char* DataAsString = reply.constData();
                    //"DSC 1 Sensor  5 Type  1 Readout 2374 Scale Factor    1 DAC 2047 Calibration    4 Temperature 32"
                    sscanf(DataAsString,"DSC %d Sensor  %d Type  %d Readout %d Scale Factor    %d DAC %d Calibration    %d Temperature %d",
                           &dsc,&sensor,&type,&readout,&scalefactor,&dac,&calibration,&temperature);
                    //qDebug()<<reply;
                    if ( type == 1 )
                    {
                        //qDebug()<<dsc<<" "<<sensor<<" "<<type<<" "<<readout<<" "<<scalefactor<<" "<<dac<<" "<<calibration<<" "<<temperature;
                        qDebug()<<reply;
                    }
                    if ( type == 2 )
                    {
                        //qDebug()<<dsc<<" "<<sensor<<" "<<type<<" "<<readout<<" "<<scalefactor;
                        qDebug()<<reply;
                    }
                }
            }
        }
        qDebug()<<"***********************";

    }
}


void ConcentratorWindow::on_DownloadBinary_pushButton_clicked()
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    QByteArray hex_line,reply,fsizeQ;
    fileversion = "1.0.0rc0";
    ui->statusbar->showMessage("Downloading "+filename);

    ui->Flashing_label->setPixmap(redled);

    ui->StoreToSensors_pushButton->setEnabled(false);
    ui->StartProgramSensors_pushButton->setEnabled(false);
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
    QFileInfo fi(filename);
    QString base = fi .baseName();
    QByteArray Title = ":T "+base.toUtf8() +" "+fileversion.toUtf8() +" "+fsizeQ+"\r\n";
    qDebug()<<Title;
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
    ui->StoreToSensors_pushButton->setEnabled(true);
    ui->StartProgramSensors_pushButton->setEnabled(true);
    ui->Line_comboBox->setEnabled(true);
    ui->Sensor_comboBox->setEnabled(true);
}

void ConcentratorWindow::on_SelectFile_pushButton_clicked()
{
    filename = QFileDialog::getOpenFileName();
    ui->label_FILE->setText(filename);

}

void ConcentratorWindow::on_StoreToSensors_pushButton_clicked()
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

void ConcentratorWindow::on_Special_pushButton_clicked()
{
QByteArray reply;
QByteArray Command;
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");

    if ( special_state == SPECIAL_STATE_NORMAL )
    {
        Command = "<x 1>";
        ui->Special_label->setPixmap(redled);
        ui->Special_pushButton->setText("Normal");
    }
    else
    {
        Command = "<x 0>";
        ui->Special_pushButton->setText("Special");
        ui->Special_label->setPixmap(greenled);
    }

    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        special_state ++;
        special_state &= 1;

        qDebug()<<reply;
    }
}

void ConcentratorWindow::on_SpecialString_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    Command = "<x 2 "+ui->NewSensorAddress_comboBox->currentText().toUtf8()+" "+ui->Name_plainTextEdit->toPlainText().toUtf8()+" "+ui->Version_plainTextEdit->toPlainText().toUtf8()+" >";
    //qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
}

void ConcentratorWindow::on_RequestInfo_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;
    QByteArray sensor;

        line = ui->GetInfoLine_comboBox->currentText().toUtf8();
        sensor = ui->GetInfoSensor_comboBox->currentText().toUtf8();

        Command = "<I "+line+" "+sensor+">";
        if ( (reply = serial_tx(Command)) != "1" )
        {
            qDebug()<<reply;
        }
}

void ConcentratorWindow::on_ReadParameters_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;
    int recvline,recvsensor,ThresholdLow,ThresholdHigh,Hysteresis_K,HardLimitLow,HardLimitHigh,Sines,pnum;
    line = ui->ParamLineRead_comboBox->currentText().toUtf8();
    Command = "<Q "+line+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
        const char* DataAsString = reply.constData();
        pnum = sscanf(DataAsString,"Q %d %d %d %d %d %d %d %d",
               &recvline,&ThresholdLow,&ThresholdHigh,&Hysteresis_K,&HardLimitLow,&HardLimitHigh,&Sines);
        if ( pnum == 7)
        {
            ui->ThresholdLow_lineEdit->setText(QString::number(ThresholdLow));
            ui->ThresholdHigh_lineEdit->setText(QString::number(ThresholdHigh));
            ui->Hysteresis_K_lineEdit->setText(QString::number(Hysteresis_K));
            ui->HardLimitLow_lineEdit->setText(QString::number(HardLimitLow));
            ui->HardLimitHigh_lineEdit->setText(QString::number(HardLimitHigh));
            ui->Sines_lineEdit->setText(QString::number(Sines));
        }
    }
}

void ConcentratorWindow::on_WriteParameters_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;

    line = ui->ParamLineRead_comboBox->currentText().toUtf8();
    ui->ThresholdLow_lineEdit->setValidator( new QIntValidator(0, 1024, this) );
    ui->ThresholdHigh_lineEdit->setValidator( new QIntValidator(2048, 4095, this) );
    ui->Hysteresis_K_lineEdit->setValidator( new QIntValidator(0, 512, this) );
    ui->HardLimitLow_lineEdit->setValidator( new QIntValidator(0, 512, this) );
    ui->HardLimitHigh_lineEdit->setValidator( new QIntValidator(2048, 4095, this) );
    ui->Sines_lineEdit->setValidator( new QIntValidator(2, 8, this) );
    Command = "<K "+line+" "+
            ui->ThresholdLow_lineEdit->text().toUtf8()+" "+
            ui->ThresholdHigh_lineEdit->text().toUtf8()+" "+
            ui->Hysteresis_K_lineEdit->text().toUtf8()+" "+
            ui->HardLimitLow_lineEdit->text().toUtf8()+" "+
            ui->HardLimitHigh_lineEdit->text().toUtf8()+" "+
            ui->Sines_lineEdit->text().toUtf8()+" "+
            ">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
}
