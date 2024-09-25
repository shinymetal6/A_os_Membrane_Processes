/********************************************************************************
** Form generated from reading UI file 'concentratorwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CONCENTRATORWINDOW_H
#define UI_CONCENTRATORWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_ConcentratorWindow
{
public:
    QWidget *centralwidget;
    QComboBox *Port_comboBox;
    QLabel *Comm_label;
    QLabel *RX_label;
    QLineEdit *PacketsReceived_lineEdit;
    QLabel *label;
    QLabel *RX_label_2;
    QPushButton *Exit_pushButton;
    QPushButton *DownloadBinary_pushButton;
    QProgressBar *download_progressBar;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *ConcentratorWindow)
    {
        if (ConcentratorWindow->objectName().isEmpty())
            ConcentratorWindow->setObjectName(QString::fromUtf8("ConcentratorWindow"));
        ConcentratorWindow->resize(800, 600);
        centralwidget = new QWidget(ConcentratorWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        Port_comboBox = new QComboBox(centralwidget);
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->addItem(QString());
        Port_comboBox->setObjectName(QString::fromUtf8("Port_comboBox"));
        Port_comboBox->setGeometry(QRect(30, 30, 111, 22));
        Comm_label = new QLabel(centralwidget);
        Comm_label->setObjectName(QString::fromUtf8("Comm_label"));
        Comm_label->setGeometry(QRect(170, 30, 22, 22));
        Comm_label->setPixmap(QPixmap(QString::fromUtf8(":/ledred.png")));
        Comm_label->setScaledContents(true);
        RX_label = new QLabel(centralwidget);
        RX_label->setObjectName(QString::fromUtf8("RX_label"));
        RX_label->setEnabled(true);
        RX_label->setGeometry(QRect(520, 30, 22, 22));
        RX_label->setPixmap(QPixmap(QString::fromUtf8(":/ledred.png")));
        RX_label->setScaledContents(true);
        PacketsReceived_lineEdit = new QLineEdit(centralwidget);
        PacketsReceived_lineEdit->setObjectName(QString::fromUtf8("PacketsReceived_lineEdit"));
        PacketsReceived_lineEdit->setGeometry(QRect(410, 30, 91, 22));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        label->setGeometry(QRect(260, 30, 141, 16));
        RX_label_2 = new QLabel(centralwidget);
        RX_label_2->setObjectName(QString::fromUtf8("RX_label_2"));
        RX_label_2->setEnabled(true);
        RX_label_2->setGeometry(QRect(910, 230, 22, 22));
        RX_label_2->setPixmap(QPixmap(QString::fromUtf8(":/ledred.png")));
        RX_label_2->setScaledContents(true);
        Exit_pushButton = new QPushButton(centralwidget);
        Exit_pushButton->setObjectName(QString::fromUtf8("Exit_pushButton"));
        Exit_pushButton->setGeometry(QRect(40, 470, 100, 27));
        DownloadBinary_pushButton = new QPushButton(centralwidget);
        DownloadBinary_pushButton->setObjectName(QString::fromUtf8("DownloadBinary_pushButton"));
        DownloadBinary_pushButton->setGeometry(QRect(210, 470, 151, 27));
        download_progressBar = new QProgressBar(centralwidget);
        download_progressBar->setObjectName(QString::fromUtf8("download_progressBar"));
        download_progressBar->setGeometry(QRect(210, 500, 471, 23));
        download_progressBar->setValue(0);
        ConcentratorWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(ConcentratorWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 800, 24));
        ConcentratorWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(ConcentratorWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        ConcentratorWindow->setStatusBar(statusbar);

        retranslateUi(ConcentratorWindow);

        QMetaObject::connectSlotsByName(ConcentratorWindow);
    } // setupUi

    void retranslateUi(QMainWindow *ConcentratorWindow)
    {
        ConcentratorWindow->setWindowTitle(QApplication::translate("ConcentratorWindow", "ConcentratorWindow", nullptr));
        Port_comboBox->setItemText(0, QApplication::translate("ConcentratorWindow", "Invalid", nullptr));
        Port_comboBox->setItemText(1, QApplication::translate("ConcentratorWindow", "/dev/ttyACM0", nullptr));
        Port_comboBox->setItemText(2, QApplication::translate("ConcentratorWindow", "/dev/ttyACM1", nullptr));
        Port_comboBox->setItemText(3, QApplication::translate("ConcentratorWindow", "COM4", nullptr));
        Port_comboBox->setItemText(4, QApplication::translate("ConcentratorWindow", "COM5", nullptr));
        Port_comboBox->setItemText(5, QApplication::translate("ConcentratorWindow", "COM6", nullptr));
        Port_comboBox->setItemText(6, QApplication::translate("ConcentratorWindow", "COM7", nullptr));
        Port_comboBox->setItemText(7, QApplication::translate("ConcentratorWindow", "COM8", nullptr));
        Port_comboBox->setItemText(8, QApplication::translate("ConcentratorWindow", "COM9", nullptr));
        Port_comboBox->setItemText(9, QApplication::translate("ConcentratorWindow", "COM10", nullptr));

        Port_comboBox->setCurrentText(QApplication::translate("ConcentratorWindow", "Invalid", nullptr));
        Comm_label->setText(QString());
        RX_label->setText(QString());
        label->setText(QApplication::translate("ConcentratorWindow", "Packets received", nullptr));
        RX_label_2->setText(QString());
        Exit_pushButton->setText(QApplication::translate("ConcentratorWindow", "Exit", nullptr));
        DownloadBinary_pushButton->setText(QApplication::translate("ConcentratorWindow", "Download Binary", nullptr));
    } // retranslateUi

};

namespace Ui {
    class ConcentratorWindow: public Ui_ConcentratorWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CONCENTRATORWINDOW_H
