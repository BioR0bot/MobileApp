#pragma once

#include <QMainWindow>
#include "ui_mainwindow.h"


class AppDataManager;
class ConnectIndicator;
class DatachannelThread;
class mTimer;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void AdjustGeometry();
    void InitWidgets();

private:
    void UpdateWindgets();


private:
    Ui::MainWindow                     mUi;
    std::shared_ptr<AppDataManager>    mAppDataManager;
    std::shared_ptr<ConnectIndicator>  mConnectIndicator;
    std::shared_ptr<DatachannelThread> mDatachannelThread;
    std::shared_ptr<QTimer>            mTimer;

};
