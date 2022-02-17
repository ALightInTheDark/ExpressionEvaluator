#include "mainwindow.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}

/*
1.1+2.2*(3.3-4.5)/5.5
sin(46*pi/180)*sin(72*pi/180)+sin(18*pi/180)*sin(44*pi/180)
sqrt(((((3!-tane)+1)^2-1)/12))^2
ln(sqrt(2-1)*sqrt(2+1)+2)+ln(2)
2*sin(2*3-2)+2*e^(3*3+2)
36^2/6^2+(4^4/4^3)*2+7^2*2^2-8^5/8^3!
1/12+1/16+4/3+1/6+1+1/3+1/2+9/4+4/6+36/8+11+4/3+9/4
((81/24)+(243/120)+(sin(729)/tan(720)+lg(2187/5040))!)--100
(-1)!
-1!
()
()()
(()
3(3)
3!3
(3)!(3)
3.141.5+9
sin3PI
PI3+4
*/
