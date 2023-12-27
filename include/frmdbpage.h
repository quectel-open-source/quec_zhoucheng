#ifndef FRMDBPAGE_H
#define FRMDBPAGE_H

#include <QWidget>
#include <commonZack.h>
#include <QDateTimeEdit>

class DbPage;

namespace Ui {
class frmDbPage;
}

class frmDbPage : public QWidget
{
    Q_OBJECT

public:
    explicit frmDbPage(QWidget *parent = 0);
    ~frmDbPage();

private:
    Ui::frmDbPage *ui;

    QList<QString> columnNames; //字段名集合
    QList<int> columnWidths;    //字段宽度集合
    DbPage *dbPage;             //数据库翻页类

    QString tableName;          //表名称
    QString countName;          //统计行数字段名称

    QComboBox* _comboBox;
    QLineEdit* _lineEdit;
    QWidget* _widgetEdit;
    QDateTimeEdit* _timeEdit_1;
    QDateTimeEdit* _timeEdit_2;
    QPushButton* _buttonSearch;
    QPushButton* _buttonResume;

    QList<QString> _colNameList;
    QList<QString> _colNameList_CN;
    QList<QString> _searchNames;
    QList<QString> _searchNames_value;
private:
    void initForm();
    void init_UI();

private slots:
    void on_btnSelect_clicked();
    void on_buttonSearch_clicked();
    void on_comboBox_changed(int idx);
};

#endif // FRMDBPAGE_H
