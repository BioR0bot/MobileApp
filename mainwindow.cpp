#include "mainwindow.h"

#include "ConnectIndicator.h"
#include "AppDataManager.h"
#include "DatachannelThread.h"
#include <qtimer.h>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mUi.setupUi(this);

    mAppDataManager = std::make_shared<AppDataManager>();
    mConnectIndicator = std::make_shared<ConnectIndicator>(mAppDataManager, this);
    mDatachannelThread = std::make_shared<DatachannelThread>(mAppDataManager);
    mTimer = std::make_shared<QTimer>(this);

    mUi.horizontalLayout->addWidget(mConnectIndicator.get());
    mUi.horizontalLayout->setStretch(0, 10);
    mUi.horizontalLayout->setStretch(1, 1);

    mTimer->singleShot(10, this, &MainWindow::UpdateWindgets);
}

MainWindow::~MainWindow()
{

}

void MainWindow::AdjustGeometry()
{
    mConnectIndicator->AdjustGeometry();
}

void MainWindow::InitWidgets()
{
    mConnectIndicator->Update();
}

void MainWindow::UpdateWindgets()
{
    mConnectIndicator->Update();



//    mDatachannelThread->RestartIfNeed();


//    std::string logData;
//    mAppDataManager->GetLogData(logData);
//    ui->lblLogOutput->setText(QString::fromStdString(logData));

    mTimer->singleShot(10, this, &MainWindow::UpdateWindgets);
}

