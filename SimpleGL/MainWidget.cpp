#include "MainWidget.h"

MainWidget::MainWidget(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    // ÐÅºÅ²Û
    connect(ui.btn_filter_inverse, &QPushButton::clicked, ui.paintingWidget, &PaintingWidget::setFilterInverse);
    connect(ui.btn_filter_none, &QPushButton::clicked, ui.paintingWidget, &PaintingWidget::setFilterNone);
    connect(ui.paintingWidget, &PaintingWidget::onCollision, this, &MainWidget::showCollisionInfo);
    connect(ui.paintingWidget, &PaintingWidget::showFPS, this, &MainWidget::showFPS);

    // ×´Ì¬À¸
    m_label_collision = new QLabel("");
    m_label_collision->setStyleSheet("color: rgb(255, 0, 0);");
    m_label_collision->setMinimumWidth(150);
    ui.statusBar->addWidget(m_label_collision);

    m_fps = new QLabel("");
    m_fps->setStyleSheet("color: rgb(0, 0, 0);");
    m_fps->setMinimumWidth(150);
    ui.statusBar->addWidget(m_fps);
}

void MainWidget::setScene(const std::string& filename)
{
    ui.paintingWidget->setSceneFile(filename);
}

void MainWidget::setUseRT(bool use, int thread_num)
{
    ui.paintingWidget->setUseRT(use, thread_num);
}

void MainWidget::showCollisionInfo(const QString& target_name)
{
    m_label_collision->setText(QString::fromLocal8Bit("ÄãÅöµ½ÁË: ") + target_name);
}

void MainWidget::showFPS(int fps)
{
    m_fps->setText(QString::number(fps) + "\tfps");
}
