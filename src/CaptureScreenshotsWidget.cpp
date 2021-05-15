#include "CaptureScreenshotsWidget.h"

#include <qpushbutton.h>
#include <qgridlayout.h>
#include <qlabel.h>
#include <qlineedit.h>

CaptureScreenshotsWidget::CaptureScreenshotsWidget(QWidget* parent)
    : QWidget(parent)
{
    setWindowTitle(tr("Capture Screenshots"));

    QPushButton* browseButtonCameras = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonCameras, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseCameraFiles);
    lineCameras = new QLineEdit(this);

    QPushButton* browseButtonDir = new QPushButton(tr("&Browse..."), this);
    connect(browseButtonDir, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::browseMainDir);
    lineMainDir = new QLineEdit(this);

    QPushButton* clearButton = new QPushButton(tr("&Clear"), this);
    connect(clearButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::clearParameters);

    QPushButton* startButton = new QPushButton(tr("&Start capturing..."), this);
    connect(startButton, &QAbstractButton::clicked, this, &CaptureScreenshotsWidget::startCapturing);
    
    QGridLayout* mainLayout = new QGridLayout(this);
    mainLayout->addWidget(new QLabel(tr("Select the cameras files that will be used.")), 0, 0, 1, 3);
    mainLayout->addWidget(new QLabel(tr("Cameras:")), 1, 0);
    mainLayout->addWidget(lineCameras, 1, 1);
    mainLayout->addWidget(browseButtonCameras, 1, 2);

    mainLayout->addWidget(new QLabel(tr("Select the cameras files that will be used.")), 2, 0, 1, 3);
    mainLayout->addWidget(new QLabel(tr("Main dir:")), 3, 0);
    mainLayout->addWidget(lineMainDir, 3, 1);
    mainLayout->addWidget(browseButtonDir, 3, 2);

    mainLayout->addWidget(clearButton, 4, 1, Qt::AlignRight | Qt::AlignBottom);
    mainLayout->addWidget(startButton, 4, 2, Qt::AlignRight | Qt::AlignBottom);
    this->setLayout(mainLayout);
}

void CaptureScreenshotsWidget::browseCameraFiles()
{

}

void CaptureScreenshotsWidget::browseMainDir()
{
}

void CaptureScreenshotsWidget::startCapturing()
{
}

void CaptureScreenshotsWidget::clearParameters()
{
}
