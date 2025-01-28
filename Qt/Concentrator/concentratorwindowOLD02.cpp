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
    fi_loaded = 0;
    ui->PowerON_pushButton->setEnabled(false);
    ui->Power_label->setPixmap(redled);

    ui->data_frame->setEnabled(false);
    ui->program_frame->setEnabled(false);
    ui->selectfile_frame->setEnabled(false);
    ui->special_frame->setEnabled(false);
    ui->params_frame->setEnabled(false);
    ui->k_frame->setEnabled(false);
    ui->info_frame->setEnabled(false);

    wait_reply_var = WAIT_REPLY;
    timer0Id = 0;
    scan_time_changed = 0;
    cmd_counter1 = cmd_counter2 = cmd_counter3 = cmd_counter4 = 0;
    packets = 0;
    ui->PacketsReceived_lineEdit->setText(QString::number(packets));
    for(int i=0;i<32;i++)
        fi[i] = 0;
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

            QString result;
            result = QString::number(packets).rightJustified(8, '0');
            ui->RX_label->setPixmap(greenled);
        }
        if ( reply[1] == 'E')
        {
            QString result;
            result = QString::number(packets).rightJustified(8, '0');
            ui->RX_label->setPixmap(greenled);
            ui->Flashing_label->setPixmap(greenled);
            qDebug()<< "Finished ";
        }
        else if ( reply[1] == 'T')
        {
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
        packets++;
        ui->PacketsReceived_lineEdit->setText(QString::number(packets));
        //qDebug()<< packets;
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
            ui->program_frame->setEnabled(true);
        }
        else
        {
            ui->program_frame->setEnabled(false);
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
#ifdef W_ALL_ENABLED
        ui->program_frame->setEnabled(true);
        ui->selectfile_frame->setEnabled(true);
        ui->special_frame->setEnabled(true);
        ui->params_frame->setEnabled(true);
#endif
        ui->k_frame->setEnabled(true);
        //ui->info_frame->setEnabled(true);
    }
    else
    {
        ui->PowerON_pushButton->setText("Power ON");
        Command = "<O>";
        ui->Power_label->setPixmap(redled);
        ui->data_frame->setEnabled(false);
        ui->program_frame->setEnabled(false);
        ui->data_frame->setEnabled(false);
        ui->program_frame->setEnabled(false);
        ui->selectfile_frame->setEnabled(false);
        ui->special_frame->setEnabled(false);
        ui->params_frame->setEnabled(false);
        ui->k_frame->setEnabled(false);
        ui->info_frame->setEnabled(false);
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
        ui->k_frame->setEnabled(true);
    }
    else
    {
        QString ll = ui->setScanTime_comboBox->currentText();
        scan_time = ll.toInt();
        ui->setScanTime_comboBox->setEnabled(false);
        ui->sec_scan_label->setEnabled(false);
        ui->scantime_label->setEnabled(false);
        ui->k_frame->setEnabled(false);

        ui->Scan_pushButton->setText("Stop");
        cmd_counter1 = cmd_counter2 = cmd_counter3 = cmd_counter4 = 0;
        timer0Id = startTimer(scan_time*1000);
        toggle=0;
        ui->statusbar->showMessage("Running Scan @"+ui->setScanTime_comboBox->currentText()+" sec -");
    }
}


void ConcentratorWindow::on_StartProgramSensors_pushButton_clicked()
{
QPixmap redled (":/ledred.png");
QPixmap greenled(":/ledgreen.png");
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

    ui->SensorFlashing_label->setPixmap(redled);
    qApp->processEvents();

    Command = "<W "+line+" "+sensor+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
    }
    ui->statusbar->showMessage("Writing");
    QThread::msleep(2000);
    ui->statusbar->showMessage("Written");
    on_PowerON_pushButton_clicked();
    QThread::msleep(2000);
    on_PowerON_pushButton_clicked();
    ui->SensorFlashing_label->setPixmap(greenled);
    qApp->processEvents();

}

void ConcentratorWindow::timerEvent(QTimerEvent *event)
{
    QByteArray reply;
    QByteArray Command;
    QByteArray qline;
    QByteArray qsensor;
    int dsc,sensor;

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
        //qDebug()<<"***********************";
        wait_reply_var = 50;
        for(dsc=1;dsc<NUM_DSC+1;dsc++)
        {
            for(sensor=FIRST_WSENSOR;sensor<LAST_WSENSOR+2;sensor++)
            {
                QThread::msleep(30);
                qline.setNum(dsc);
                qsensor.setNum(sensor);
                Command = "<A "+qline+" "+qsensor+">";
                if ( (reply = serial_tx(Command)) != "1" )
                {
                    store_sensor_data(reply,dsc,sensor);
                }
            }
        }
    }
}

//QString dirPath= "/Devel/Membrane";
#ifdef Q_OS_WIN
QString dirPath= "c:/MembraneLog_3.2";
#else
QString dirPath= "/Devel/MembraneLog";
#endif
QString filename;

void ConcentratorWindow::store_sensor_data(QByteArray reply , int dsc , int sensor)
{
    QDateTime currentDateTime = QDateTime::currentDateTime();
    QDate currentDate = currentDateTime.date();
    QTime currentTime = currentDateTime.time();
    QString dayPath = currentDate.toString("ddMMyy");
    QString version = "1.0.0";
    QString folderpath;
    int type,readout,total_readout,scalefactor,dac,calibration,temp_micro;
    int offset;
    concentrator_counter = 1;
    QByteArray q_concentrator_counter;
    q_concentrator_counter.setNum(concentrator_counter);
    QString TopDir_dayPath = dirPath+"/"+currentDate.toString("yyMMdd")+"_CON"+q_concentrator_counter+"_iCON";
    QString File_dayPath = currentDate.toString("yyMMdd")+"_CON"+q_concentrator_counter+"_iCON";
    int cmd_counter;
    //dac = 2047;
    int offsets[8];
    float ftp1000_Tdata;
    int scale = 0;
    char    tval[32];

    QDir top_directory(dirPath);
    if ( !top_directory.exists())
    {
        qDebug()<< dirPath << " created";
        top_directory.mkpath(dirPath);
    }

    QDir top_directory_folderpath(TopDir_dayPath);
    if ( !top_directory_folderpath.exists())
    {
        qDebug()<< TopDir_dayPath << " created";
        top_directory_folderpath.mkpath(TopDir_dayPath);
    }


    QByteArray q_dsc;
    q_dsc.setNum(dsc);
    filename = TopDir_dayPath+"/"+currentDate.toString("yyMMdd")+"_CON"+q_concentrator_counter+"_DSC"+q_dsc+".csv";

    QFile file(filename);
    if ( ! file.exists())
    {
        qDebug()<< filename << " : File not present, created";
        CsvFile.setFileName(filename);
        CsvFile.open(QIODevice::Append | QIODevice::Text);
        CsvFileStream.setDevice(&CsvFile);
        CsvFileStream << "################################################################\n";
        CsvFileStream << "Concentrator version,v1.1-241212\n";
        CsvFileStream << "Sensors      version,v1.1-241212\n";
        CsvFileStream << "DSC Scan time (mSec),"<<ui->setScanTime_comboBox->currentText()<<"000\n";
        CsvFileStream << "-,-\n";
        CsvFileStream << "-,-\n";
        CsvFileStream << "################################################################\n";
        CsvFileStream << "Time stamp,Sequence number,Concentrator,DSC,Sensor,Scale,Readout,Total Readout,Total Noise,Temp Micro,Temp PT1000,DAC,Active,Status\n";
    }
    else
    {
        CsvFile.setFileName(filename);
        CsvFile.open(QIODevice::Append | QIODevice::Text);
        if (  scan_time_changed == 1 )
        {
            scan_time_changed = 0;
            //CsvFileStream << "################################################################\n";
            //CsvFileStream << "####  Scan time "<<ui->setScanTime_comboBox->currentText()<<" Sec ####\n";
            //CsvFileStream << "################################################################\n";
        }
        CsvFileStream.setDevice(&CsvFile);
    }

    const char* DataAsString = reply.constData();
    sscanf(DataAsString,"DSC %d Sensor  %d Type  %d Readout %d Scale Factor    %d DAC %d Calibration    %d Temperature %d",
           &dsc,&sensor,&type,&readout,&scalefactor,&dac,&calibration,&temp_micro);
    cmd_counter = 0;
    switch(dsc)
    {
    case 1 :
        cmd_counter = cmd_counter1; cmd_counter1++;break;
    case 2 :
        cmd_counter = cmd_counter2; cmd_counter2++;break;
    case 3 :
        cmd_counter = cmd_counter3; cmd_counter3++;break;
    case 4 :
        cmd_counter = cmd_counter4; cmd_counter4++;break;
    }
    switch(scalefactor)
    {
    case 1 : scale = 0; break;
    case 2 : scale = 1; break;
    case 4 : scale = 2; break;
    case 8 : scale = 3; break;
    case 16 : scale =4; break;
    case 32 : scale =5; break;
    case 64 : scale =6; break;
    default : scale = 0;
    }


    //#define DEBUG_APP_DSC1    1
    if ( type == 1 )
    {
        QString timestamp = currentDate.toString("dd/MM/yy")+" "+currentTime.toString("hh:mm:ss");
        offset = ui->Offset_lineEdit->text().toInt();
        sprintf(tval,"%2.1f",tdata_sensor[dsc]);
        for(int i=0;i<32;i++)
        {
            if ( tval[i] == 0 )
                break;
            if ( tval[i] == ',')
                tval[i] = '.';
        }

        if ( ui->useK_checkBox->isChecked() && (fi_loaded == 1 ))
        {
            offsets[0] = offset;
            for ( int n=1;n<8;n++)
            {
                offsets[n] = offsets[n-1]+2559*fi[n-1];
            }
            total_readout =   offsets[scale] +  readout * fi[scale];
#ifdef DEBUG_APP_DSC1
            if ( dsc == 1 )
#endif
                qDebug()<<"dsc" << dsc << "sensor "<< sensor << " scalefactor " << scale+1 << " readout " << readout*fi[scale] << "offsets" << offsets[scale] << " total_readout " << total_readout << " dac " << dac+1;
            CsvFileStream << timestamp << "," << cmd_counter << "," << concentrator_counter << "," << dsc << "," << sensor << "," << scale+1 << "," << readout << "," << total_readout << "," << calibration << "," << temp_micro << ","<< tval << dac+1 << ",Y,A\n";
        }
        else
        {
            total_readout = readout * (scale + 1);
#ifdef DEBUG_APP_DSC1
            if ( dsc == 1 )
#endif
                qDebug()<<"dsc" << dsc << "sensor "<< sensor << " scalefactor " << scale+1 << " readout " << readout << " total_readout " << total_readout;
            CsvFileStream << timestamp << "," << cmd_counter << "," << concentrator_counter << "," << dsc << "," << sensor << "," << scale+1 << "," << readout << "," << total_readout << "," << calibration << "," << temp_micro << ","<< tval << "," << dac+1 << ",Y,A\n";
        }
    }

    if ( type == 2 )
    {
        int tp1000_Tdata,up_Tdata;
        float deg1 = 3.9F;
        float deg0read = 1855.0F;
        const char* DataAsString = reply.constData();
        int pnum = sscanf(DataAsString,"DSC %d Sensor  %d Type  %d PT1000_TData %d uP_TData %d",
               &dsc,&sensor,&type,&tp1000_Tdata,&up_Tdata);
        if ( pnum == 5 )
        {
            ftp1000_Tdata = ((float )tp1000_Tdata - deg0read) / deg1;
            tdata_sensor[dsc] = ftp1000_Tdata/2;
        }
    }
    cmd_counter++;
    CsvFile.close();
}

void ConcentratorWindow::on_DownloadBinary_pushButton_clicked()
{
    QPixmap redled (":/ledred.png");
    QPixmap greenled(":/ledgreen.png");

    QByteArray hex_line,reply,fsizeQ;
    fileversion = "1.0.0rc0";
    ui->statusbar->showMessage("Downloading "+filename);

    ui->Flashing_label->setPixmap(redled);

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

    QString base = fi.completeBaseName() + "." +fi.completeSuffix();

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
    ui->program_frame->setEnabled(true);
    ui->statusbar->showMessage(filename+" downloaded");
}

void ConcentratorWindow::on_SelectFile_pushButton_clicked()
{
    filename = QFileDialog::getOpenFileName();
    QFileInfo fi(filename);
    QString base = fi.completeBaseName() + "." +fi.completeSuffix();
    ui->label_FILE->setText(base);

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
            ui->statusbar->showMessage("Transfer done");
            ui->StartProgramSensors_pushButton->setEnabled(true);
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
    int recvline,ThresholdLow,ThresholdHigh,Hysteresis_K,HardLimitLow,HardLimitHigh,Sines,pnum;
    line = ui->ParamLineRead_comboBox->currentText().toUtf8();
    Command = "<Q "+line+">";
    qDebug()<<Command;
    if ( (reply = serial_tx(Command)) != "1" )
    {
        qDebug()<<reply;
        const char* DataAsString = reply.constData();
        pnum = sscanf(DataAsString,"Q %d %d %d %d %d %d %d",
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

void ConcentratorWindow::on_RequestVersionInfo_pushButton_clicked()
{
    QByteArray reply;
    QByteArray Command;
    QByteArray line;
    QByteArray sensor;

        line = ui->GetVersionInfoLine_comboBox->currentText().toUtf8();
        sensor = ui->GetVersionInfoSensor_comboBox->currentText().toUtf8();

        Command = "<J "+line+" "+sensor+">";
        if ( (reply = serial_tx(Command)) != "1" )
        {
            qDebug()<<reply;
        }
}

void ConcentratorWindow::on_setScanTime_comboBox_currentTextChanged(const QString &arg1)
{
    if ( scan_time != arg1.toInt())
    {
        scan_time = arg1.toInt();
        scan_time_changed = 1;
    }
}


void ConcentratorWindow::on_SelectAlgoCSVFile_pushButton_clicked()
{
    QString filters = "CSV files (*.csv)";
#ifdef Q_OS_WIN
    csvk_filename = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "c:/MembraneData",filters);
#else
    csvk_filename = QFileDialog::getOpenFileName(this, tr("Open CSV File"), "/Devel/MembraneData",filters);
#endif
    QFileInfo ficsv(csvk_filename);
    QString base = ficsv.completeBaseName() + "." +ficsv.completeSuffix();
    ui->label_CSVFILE->setText(base);
    QFile file(csvk_filename);

    if (!file.open(QIODevice::ReadOnly))
        qDebug()<<"File not found";
    else
    {
        int i=0;
        while(!file.atEnd())
        {
            QString line = file.readLine();
            QStringList cols = line.split(",");
            fi[i] = cols.at(0).toFloat();
            i++;
            if ( i >= 14 )
                qDebug()<<"Error " << i;

        }
        file.close();
        qDebug() <<  "###########";
        for(i=0;i<14;i++)
            qDebug()<<fi[i];
        ui->K1_lineEdit->setText(QString::number(fi[0], 'f',8));
        ui->K2_lineEdit->setText(QString::number(fi[1], 'f',8));
        ui->K3_lineEdit->setText(QString::number(fi[2], 'f',8));
        ui->K4_lineEdit->setText(QString::number(fi[3], 'f',8));
        ui->K5_lineEdit->setText(QString::number(fi[4], 'f',8));
        ui->K6_lineEdit->setText(QString::number(fi[5], 'f',8));
        ui->K7_lineEdit->setText(QString::number(fi[6], 'f',8));
        ui->K8_lineEdit->setText(QString::number(fi[7], 'f',8));
        ui->K9_lineEdit->setText(QString::number(fi[8], 'f',8));
        ui->K10_lineEdit->setText(QString::number(fi[9], 'f',8));
        ui->K11_lineEdit->setText(QString::number(fi[10], 'f',8));
        ui->K12_lineEdit->setText(QString::number(fi[11], 'f',8));
        ui->K13_lineEdit->setText(QString::number(fi[12], 'f',8));
        ui->K14_lineEdit->setText(QString::number(fi[13], 'f',8));
        fi_loaded = 1;
    }


}

