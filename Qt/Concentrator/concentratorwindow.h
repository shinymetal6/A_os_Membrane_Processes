#ifndef CONCENTRATORWINDOW_H
#define CONCENTRATORWINDOW_H

#include <QMainWindow>
#include <QtSerialPort/QSerialPort>
#include <QtSerialPort/QSerialPortInfo>
#include <QFile>
#include <QCoreApplication>
#include <QTextStream>

QT_BEGIN_NAMESPACE
namespace Ui { class ConcentratorWindow; }
QT_END_NAMESPACE
#define WAIT_REPLY              1000
#define WAIT_REPLY_DOWNLOAD     30000
#define SPECIAL_STATE_NORMAL    0
#define SPECIAL_STATE_SPECIAL   1

class ConcentratorWindow : public QMainWindow
{
    Q_OBJECT

public:
    ConcentratorWindow(QWidget *parent = nullptr);
    ~ConcentratorWindow();

private slots:
    void on_Port_comboBox_currentTextChanged(const QString &arg1);

    void on_Exit_pushButton_clicked();

    void on_DownloadBinary_pushButton_clicked();

    void on_GetInfo_pushButton_clicked();

    void on_PowerON_pushButton_clicked();

    void on_Discovery_pushButton_clicked();

    void on_GetMap_pushButton_clicked();

    void on_GetData_pushButton_clicked();

    void on_Scan_pushButton_clicked();

    void on_StartProgramSensors_pushButton_clicked();

    void on_SelectFile_pushButton_clicked();

    void on_StoreToSensors_pushButton_clicked();

    void on_Special_pushButton_clicked();

    void on_SpecialString_pushButton_clicked();

    void on_RequestInfo_pushButton_clicked();

    void on_ReadParameters_pushButton_clicked();

    void on_WriteParameters_pushButton_clicked();

    void on_RequestVersionInfo_pushButton_clicked();

    void on_setScanTime_comboBox_currentTextChanged(const QString &arg1);

    void on_SelectAlgoCSVFile_pushButton_clicked();

private:
    Ui::ConcentratorWindow *ui;

    QByteArray serial_tx( QByteArray hex_line);
    void store_sensor_data(QByteArray reply , int dsc , int sensor);

    QSerialPort serial;
    int serial_started;
    int power_state;
    int special_state;
    int packets;
    int packets_errors;
    int wait_reply_var;
    QString folder;
    QString filename;
    QString fileversion;
    int timer0Id;
    int timerint;
    int scan_time;
    int scan_time_changed;
    QString csvk_filename;

    QFile CsvFile;
    QTextStream CsvFileStream;
    int cmd_counter1;
    int cmd_counter2;
    int cmd_counter3;
    int cmd_counter4;
    int concentrator_counter;


protected:
    void timerEvent(QTimerEvent *event);
};
#endif // CONCENTRATORWINDOW_H
