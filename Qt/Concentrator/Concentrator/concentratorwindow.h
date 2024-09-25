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
#define WAIT_REPLY          1000
#define WAIT_REPLY_DOWNLOAD 30000

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

    void on_Program_pushButton_clicked();

    void on_FlashBinary_pushButton_clicked();

    void on_PowerON_pushButton_clicked();

    void on_Discovery_pushButton_clicked();

    void on_GetMap_pushButton_clicked();

    void on_GetData_pushButton_clicked();

    void on_Scan_pushButton_clicked();

private:
    Ui::ConcentratorWindow *ui;

    QByteArray serial_tx( QByteArray hex_line);

    QSerialPort serial;
    int serial_started;
    int power_state;
    int packets;
    int packets_errors;
    int wait_reply_var;
    QString folder;
    QString filename;
    QString fileversion;
    int timer0Id;
    int timerint;
protected:
    void timerEvent(QTimerEvent *event);
};
#endif // CONCENTRATORWINDOW_H
