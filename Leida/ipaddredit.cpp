#include "IpAddrEdit.h"
#include <QRegExpValidator>
#include <QLabel>
#include "IpPartLineEdit.h"
#include "qjsondocument.h"


IpAddrEdit::IpAddrEdit(QWidget* pParent /* = 0 */)
    : QWidget(pParent)
{
    QFont font(".萍方-简", 10);
    ip_part1_ = new IpPartLineEdit(this);
    ip_part2_ = new IpPartLineEdit(this);
    ip_part3_ = new IpPartLineEdit(this);
    ip_part4_ = new IpPartLineEdit(this);

    labeldot1_ = new QLabel(this);
    labeldot2_ = new QLabel(this);
    labeldot3_ = new QLabel(this);

    ip_part1_->setGeometry(QRect(0,  0, 44, 28));
    ip_part2_->setGeometry(QRect(44, 0, 44, 28));
    ip_part3_->setGeometry(QRect(88, 0, 44, 28));
    ip_part4_->setGeometry(QRect(132,0, 44, 28));

    ip_part1_->setFont(font);
    ip_part2_->setFont(font);
    ip_part3_->setFont(font);
    ip_part4_->setFont(font);

    labeldot1_->setText(" .");
    labeldot1_->setFont(font);
    labeldot1_->setGeometry(QRect(41, 5, 6, 18));
    labeldot1_->setAlignment(Qt::AlignCenter);

    labeldot2_->setText(" .");
    labeldot2_->setFont(font);
    labeldot2_->setGeometry(QRect(85, 5, 6, 18));
    labeldot2_->setAlignment(Qt::AlignCenter);

    labeldot3_->setText(" .");
    labeldot3_->setFont(font);
    labeldot3_->setGeometry(QRect(129, 5, 6, 18));
    labeldot3_->setAlignment(Qt::AlignCenter);

    QWidget::setTabOrder(ip_part1_, ip_part2_);//定义切换焦点顺序
    QWidget::setTabOrder(ip_part2_, ip_part3_);
    QWidget::setTabOrder(ip_part3_, ip_part4_);

    ip_part1_->set_nexttab_edit(ip_part1_, ip_part2_);
    ip_part2_->set_nexttab_edit(ip_part2_, ip_part3_);
    ip_part3_->set_nexttab_edit(ip_part3_, ip_part4_);

    ip_part2_->set_prevtab_edit(ip_part2_, ip_part1_);
    ip_part3_->set_prevtab_edit(ip_part3_, ip_part2_);
    ip_part4_->set_prevtab_edit(ip_part4_, ip_part3_);

    connect(ip_part1_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part2_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part3_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));
    connect(ip_part4_, SIGNAL(textChanged(const QString&)), this, SLOT(textchangedslot(const QString&)));

    connect(ip_part1_, SIGNAL(textEdited(const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part2_, SIGNAL(textEdited(const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part3_, SIGNAL(textEdited(const QString&)), this, SLOT(texteditedslot(const QString&)));
    connect(ip_part4_, SIGNAL(textEdited(const QString&)), this, SLOT(texteditedslot(const QString&)));

}

IpAddrEdit::~IpAddrEdit()
{

}

void IpAddrEdit::textchangedslot(const QString& /*text*/)
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    QString ipaddr = QString("%1.%2.%3.%4")
                         .arg(ippart1)
                         .arg(ippart2)
                         .arg(ippart3)
                         .arg(ippart4);

    emit textchanged(ipaddr);
}

void IpAddrEdit::texteditedslot(const QString &/*text*/)
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    QString ipaddr = QString("%1.%2.%3.%4")
                         .arg(ippart1)
                         .arg(ippart2)
                         .arg(ippart3)
                         .arg(ippart4);

    emit textedited(ipaddr);
}

void IpAddrEdit::settext(const QString &text)
{
    QString ippart1, ippart2, ippart3, ippart4;
    QString qstring_validate = text;

    // IP地址验证
    QRegExp regexp("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
    QRegExpValidator regexp_validator(regexp, this);
    int nPos = 0;
    QValidator::State state = regexp_validator.validate(qstring_validate, nPos);
    // IP合法
    if (state == QValidator::Acceptable)
    {
        QStringList ippartlist = text.split(".");

        int strcount = ippartlist.size();
        int index = 0;
        if (index < strcount)
        {
            ippart1 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart2 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart3 = ippartlist.at(index);
        }
        if (++index < strcount)
        {
            ippart4 = ippartlist.at(index);
        }
    }

    ip_part1_->setText(ippart1);
    ip_part2_->setText(ippart2);
    ip_part3_->setText(ippart3);
    ip_part4_->setText(ippart4);
}

QString IpAddrEdit::text()
{
    QString ippart1, ippart2, ippart3, ippart4;
    ippart1 = ip_part1_->text();
    ippart2 = ip_part2_->text();
    ippart3 = ip_part3_->text();
    ippart4 = ip_part4_->text();

    return QString("%1.%2.%3.%4")
        .arg(ippart1)
        .arg(ippart2)
        .arg(ippart3)
        .arg(ippart4);
}

void IpAddrEdit::getIp(QString& ip1, QString& ip2, QString& ip3, QString& ip4)
{
    ip1 = ip_part1_->text();
    ip2 = ip_part2_->text();
    ip3 = ip_part3_->text();
    ip4 = ip_part4_->text();
}


void IpAddrEdit::setStyleSheet(const QString &styleSheet)
{
    ip_part1_->setStyleSheet(styleSheet);
    ip_part2_->setStyleSheet(styleSheet);
    ip_part3_->setStyleSheet(styleSheet);
    ip_part4_->setStyleSheet(styleSheet);
}
