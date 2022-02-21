#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_MainWidget.h"

class MainWidget : public QMainWindow
{
    Q_OBJECT

public:
    MainWidget(QWidget *parent = Q_NULLPTR);
    void setScene(const std::string& filename);
    void setUseRT(bool use, int thread_num);

private:
    Ui::MainWidget ui;

    // ��ײ��ʾ
    QLabel* m_label_collision;

    // ֡����ʾ
    QLabel* m_fps;

public slots:
    void showCollisionInfo(const QString& target_name);
    void showFPS(int fps);
};
