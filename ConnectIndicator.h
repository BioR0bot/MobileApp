#pragma once

#include "ui_ConnectIndicator.h"
#include <memory>


class QMovie;
class AppDataManager;

class ConnectIndicator : public QWidget
{
public:
    ConnectIndicator(std::shared_ptr<AppDataManager>& appDataManager, QWidget *parent = nullptr);
    void AdjustGeometry();
    void Update();

private:
    void ShowSucceed();
    void ShowWait();

private:
    Ui::Form                mUi;
    std::shared_ptr<QMovie> mConnectWaitMovie;
    QPixmap                 mConnectSuccessPixmap;

    enum Status
    {
        Undefined = 0,
        Succeed,
        Wait
    };
    Status                  mStatus;

    std::shared_ptr<AppDataManager>   mAppDataManager;

};
