#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QMessageBox>
#include<QFileDialog>
#include<QProcess>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
private:
	Ui::MainWindow *ui;
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QString FileName = "";
    bool edited = false;

private slots:
    void on_cal_pushButton_clicked();

    void on_action_log_triggered();

    void on_action_about_triggered();

    void on_action_use_triggered();

    void on_action_fileIO_triggered();

    void on_action_edit_triggered();

    void on_action_output_triggered();

    void on_actionQT_triggered();


};
#endif // MAINWINDOW_H