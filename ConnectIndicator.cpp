#include "ConnectIndicator.h"
#include "AppDataManager.h"
#include <qmovie.h>

ConnectIndicator::ConnectIndicator(std::shared_ptr<AppDataManager>& appDataManager, QWidget *parent) :
    QWidget(parent),
    mStatus(Status::Undefined),
    mAppDataManager(appDataManager)
{
    mUi.setupUi(this);
    mConnectWaitMovie = std::make_shared<QMovie>("assets:/connect_wait.gif");
    mConnectSuccessPixmap = QPixmap("assets:/connect_succeed.jpg");
}

void ConnectIndicator::AdjustGeometry()
{
    int lblConnectStatusSide = std::min(mUi.horizontalLayout->geometry().width(), mUi.horizontalLayout->geometry().height());

    mUi.lblConnectStatus->setFixedWidth(lblConnectStatusSide);
    mUi.lblConnectStatus->setFixedHeight(lblConnectStatusSide);
    mConnectWaitMovie->setScaledSize(mUi.lblConnectStatus->size());
    mConnectSuccessPixmap = mConnectSuccessPixmap.scaled(mUi.lblConnectStatus->size(), Qt::AspectRatioMode::KeepAspectRatio, Qt::TransformationMode::SmoothTransformation);
}

void ConnectIndicator::Update()
{
    if(mAppDataManager->GetIsConnectToRobot())
    {
        ShowSucceed();
    }
    else
    {
        ShowWait();
    }
}

void ConnectIndicator::ShowSucceed()
{
    if(mStatus == Status::Succeed)
    {
        return;
    }

    mConnectWaitMovie->stop();
    mUi.lblConnectStatus->setPixmap(mConnectSuccessPixmap);
    mStatus = Status::Succeed;
}

void ConnectIndicator::ShowWait()
{
    if(mStatus == Status::Wait)
    {
        return;
    }

    mUi.lblConnectStatus->setMovie(mConnectWaitMovie.get());
    mConnectWaitMovie->start();
    mStatus = Status::Wait;
}
