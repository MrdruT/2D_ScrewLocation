﻿#ifndef DIALOGSHAPEMODEL_H
#define DIALOGSHAPEMODEL_H

#pragma execution_character_set("utf-8")
#include <QDialog>
#include <QStandardItemModel>
#include <QMessageBox>

#include "cpp/HalconCpp.h"
#include "halfunc.h"
#include "aqlog.h"
#include "cam.h"
#include "inifile.h"

namespace Ui {
class DialogShapeModel;
}

class DialogShapeModel : public QDialog
{
    Q_OBJECT

public:
    explicit DialogShapeModel(QWidget *parent = 0);
    ~DialogShapeModel();

    //初始化函数
    int start_param_init();
    int start_ui_init();

    //模板列表模块
    void refresh_list();
    void deleteFile();

    int draw_show();
    int save_templa_image();

    //相机初始化
    int cam_init(basler_cam* ptr_cam);

    //QMessage打印
    static void print_qmess(QString& content);

    //测试
    void test();

public slots:
    int  ClickButtonPicOne();
    int  ClickButtonCreateShapeModel();

private slots:
    void on_listView_activated(const QModelIndex &index);

    void on_pushButton_delete_clicked();

    void on_pushButton_name_clicked();

    void on_listView_doubleClicked(const QModelIndex &index);

    void on_pushButtonSnapOne_clicked();

    void on_combo_ShangStd_activated(int index);

    void on_combo_Type_activated(int index);

    void on_pushButton_confirm_clicked();

private:
    Ui::DialogShapeModel *ui;

    Hobject m_image;
    HTuple m_ModelID;
    Hlong m_win_id,image_width,image_height;
    double m_create_row,m_create_col,m_create_radius;

    QString m_path_exe;

    //创建模板所使用字段
    Hobject cpy_image, m_modelRegion, m_templateImage, m_ModelContours, m_TransContours;
    HTuple m_row, m_col, m_angle, m_score;
    HTuple  hv_RefColumn, hv_HomMat2D, hv_RefRow, hv_ModelRegionArea;
    //螺丝编号对应模板编号
    int screw_num=0;
    int screw_type=0;

    //list view列表所使用字段
    QStandardItemModel *standardItemModel;
    QStringList strList;
    QString m_fileName;
    int index_delete;

    //相机
    basler_cam* p_cam;

    //log打印
    aqlog m_log;
    //保存模板对应信息
    IniFile m_ini;

};

#endif // DIALOGSHAPEMODEL_H，



