#include "mainwindow.h"

#include "ConnectIndicator.h"
#include "AppDataManager.h"
#include "DatachannelThread.h"
#include "Utils.h"
#include <qtimer.h>
#include <QDebug>
#include <opencv2/imgcodecs.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    mUi.setupUi(this);

    mAppDataManager = std::make_shared<AppDataManager>();
    mConnectIndicator = std::make_shared<ConnectIndicator>(mAppDataManager, this);
    mDatachannelThread = std::make_shared<DatachannelThread>(mAppDataManager);
    mTimer = std::make_shared<QTimer>(this);

    mUi.horizontalLayout->addWidget(mConnectIndicator.get());
    mUi.horizontalLayout->setStretch(0, 1);
    mUi.horizontalLayout->setStretch(1, 10);
    mUi.horizontalLayout->setStretch(2, 1);


    mDatachannelThread->Start();

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
    mDatachannelThread->RestartIfNeed();
    mConnectIndicator->Update();

    std::string logData;
    mAppDataManager->GetLogData(logData);
    mUi.lblDebugLog->setText(QString::fromStdString(logData));

    std::vector<unsigned char> buffer;
    bool bufferUpdated = mAppDataManager->GetReceivedBuffer(buffer);
    if(bufferUpdated)
    {
        cv::Mat surveillanceFrame = cv::imdecode(buffer, cv::IMREAD_UNCHANGED);
        mUi.lblSurveillanceFrame->setPixmap(CvMatToQPixmap(surveillanceFrame));
    }



    mTimer->singleShot(10, this, &MainWindow::UpdateWindgets);
}

