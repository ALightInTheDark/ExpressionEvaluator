#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "ExpressionEvaluator.h"
#include <exception>
using std::exception;
#include<fstream>
using std::fstream, std::ios;
#include<iostream>
using std::cout, std::endl;
#include<string>
using std::string, std::to_string;
#include<vector>
using std::vector;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowTitle(tr("表达式计算器"));
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cal_pushButton_clicked()
{
    QString qs_exp = ui -> get_exp_textEdit -> toPlainText().replace("\n",""); // 注意去掉换行符
    string exp = qs_exp.toStdString();

    QString qs_processed_exp;
    double result; QString qs_result;
    try
    {
        result = evaluator.evaluate(exp);

        qs_processed_exp = evaluator.get_preprocessed_expression().data();
        qs_result = QString::fromStdString(to_string(result));
    }
    catch(const ExpressionException& e)
    {
        if(e.error_index())
        {
            qs_processed_exp = evaluator.get_preprocessed_expression().data();
            size_t err_index = e.error_index().value();

            QString str1 = qs_processed_exp.mid(0, err_index);
            QString str2 = qs_processed_exp.mid(err_index, 1);
            QString str3 = qs_processed_exp.mid(err_index + 1);

            QString str4 = QString("<font style='font-size:42px; background-color:white; color:blue'>%1").arg(str1);
            QString str5 = QString("<font style='font-size:42px; background-color:white; color:red'>%1").arg(str2);
            QString str6 = QString("<font style='font-size:42px; background-color:white; color:blue'>%1").arg(str3);

            qs_processed_exp = str4 + str5 +str6;
            qs_result = QString("<font style='font-size:36px; background-color:white; color:red'>%1%2, %3").arg("错误位置", to_string(err_index).data(), e.what());
        }
        else
        {
            qs_processed_exp = evaluator.get_preprocessed_expression().data();
            qs_result = QString("<font style='font-size:36px; background-color:white; color:red'>%1").arg(e.what());
        }
    }
    catch(const exception& e)
    {
        qs_processed_exp = evaluator.get_preprocessed_expression().data();
        qs_result = QString("<font style='font-size:36px; background-color:white; color:red'>%1").arg(e.what());
    }

    ui->show_pretreated_Label->setText(qs_processed_exp);
    ui->show_result_Label->setText(qs_result);
}

void MainWindow::on_action_edit_triggered()
{
    QString curPath = QDir::currentPath();
    FileName = QFileDialog::getOpenFileName(this, "选择一个txt文件",curPath, "*.txt");
    if(FileName.isEmpty())
    {
        QMessageBox::information(NULL, "文件操作失败", "选择文件失败", QMessageBox::Ok);
        return;
    }
    QProcess* process = new QProcess();
    process->start("notepad.exe", QStringList(FileName));
    // 不可以delete process

    edited = true;
}


/* windows记事本使用了一个非常怪异的行为来保存UTF-8编码的文件，
 * 它自作聪明地在每个文件开头添加了0xefbbbf（十六进制）的字符，
 * 导致我们会遇到很多不可思议的问题。
 * 文件读写函数从文件的第二行开始读取数据，
 * 避免读到的数据中有记事本自行添加的字符。
*/
void MainWindow::on_action_output_triggered()
{
    if(!edited)
    {
        QMessageBox::information(NULL, "文件操作失败", "您还未编辑文件", QMessageBox::Ok);
        return;
    }

    fstream file_stream;
    file_stream.open(FileName.toStdString(), ios::out | ios::in | ios::app);
    if(!file_stream)
    {
        QMessageBox::information(NULL, "文件操作失败", "无法打开文件",QMessageBox::Ok);
        return;
    }
    file_stream.peek();
    if(file_stream.eof())
    {
        QMessageBox::information(NULL, "文件操作失败", "文件为空",QMessageBox::Ok);
        return;
    }

    vector<string> expressions;
    string s;
    getline(file_stream, s); // 跳过第一行
    while(!file_stream.eof())
    {
        getline(file_stream, s);
        if(s.empty()){ continue; } // 避免读到空行
        expressions.push_back(std::move(s));
    }

    file_stream.clear(); // 流读取到文件末尾,状态为EOF; 重设流的状态后,才能继续使用流

    file_stream << endl;

    string result;
    for(string& exp : expressions)
    {
        try
        {
            result = to_string( evaluator.evaluate(exp) );
        }
        catch(const ExpressionException& e)
        {
            result = e.what();
        }

        file_stream << result << "是表达式" << exp << "的计算结果。" << endl;
    }

    file_stream.close();

    QProcess* process = new QProcess();
    process->start("notepad.exe", QStringList(FileName));
    // 不可以delete process
}

void MainWindow::on_actionQT_triggered()
{
    QString info = "QT6.2.2\n本程序使用c++标准库中的字符串、容器、文件IO操作，未使用QT提供的对应类库。";
    QMessageBox::information(NULL, "QT版本", info, QMessageBox::Ok);
}

void MainWindow::on_action_log_triggered()
{
    QString info =
R"(2022/1/19
使用c++17标准重构项目;代码更加规范。
2020/8/18
在图形界面中添加文件操作，用户可以用记事本编辑选定的文件。
记事本会自行在文件第一行添加字符以标识编码格式，故程序不读入文件的第一行。
程序将文件中所有表达式计算出结果，追加在文件尾部，弹出记事本让用户查看结果。
重写了图形界面的布局，添加了spacers。
2020/8/17
完成了图形界面，在错误指示栏中用高亮来显示表达式中错误的字符；添加了菜单栏。
2020/8/14
完善了异常处理，补充了/的被除数不能为0、表达式中无数字、表达式中为多个括号括起来的数字，缺少运算符的情况。
2020/8/13
分析、考虑输入的表达式中可能出现的各种错误，为表达式计算函数添加完备的异常处理，并指出出错位置。
2020/8/12
基本完成了ExpressionEvaluator类的编写，实现了文件操作。
2020/8/11
实现代码
2020/8/10
选择算法：基于中缀形式的表达式求值算法。因为它更符合人的思维习惯，且有优秀的时间复杂度、空间复杂度。
)";

    QMessageBox::information(NULL, "工作日志", info,QMessageBox::Ok);
}

void MainWindow::on_action_about_triggered()
{
    QMessageBox::information(NULL, "关于", "数据结构综合训练作品\n微光\n2020.8.18",QMessageBox::Ok);
}

void MainWindow::on_action_use_triggered()
{
    QString info =
R"(============特性============
- 支持任意层嵌套括号的表达式求值，支持浮点数运算
- 完备的异常处理，能够判断表达式是否非法、操作数是否非法，并指出出错位置
- 表达式末尾可有=，可无=
- 支持英文字符大小写混用
- 负数可加括号，可不加括号
- QT实现的图形界面

============支持的运算符============
- 二元运算符：+、-、x(*)、/、%、^
- 需要左操作数的一元运算符：！
- 需要右操作数的一元运算符: sin(可简写为s)、cos(c)、tan(t)、lg(g)、ln(n)、sqrt(q)、-(取反)
- 常量：pi, e)";

    QMessageBox::information(NULL, "支持的运算", info,QMessageBox::Ok);
}

void MainWindow::on_action_fileIO_triggered()
{
    QString info =
R"(点击文件操作-编辑文件菜单项，选择您要编辑的txt文件，程序会弹出写字板让您编辑，每行输入一个表达式。
关闭并保存文件，点击文件操作-输出结果到文件尾，程序将把文件中所有表达式的计算结果追加到文件尾，而后弹出记事本让您查看结果。)";

    QMessageBox::information(NULL, "文件操作", info,QMessageBox::Ok);
}

