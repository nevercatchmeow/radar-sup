#include "IpPartLineEdit.h"
#include <QIntValidator>
#include <QKeyEvent>

IpPartLineEdit::IpPartLineEdit(QWidget *parent/* = 0*/)
    : QLineEdit(parent)
{
    next_tab_ = nullptr;
    prev_tab_ = nullptr;

    this->setMaxLength(3);
    this->setFrame(false);
    this->setAlignment(Qt::AlignCenter);

    QValidator *validator = new QIntValidator(0, 255, this);
    this->setValidator(validator);

    connect(this, SIGNAL(textEdited(const QString&)), this, SLOT(text_edited(const QString&)));
}

IpPartLineEdit::~IpPartLineEdit(void)
{
}

void IpPartLineEdit::focusInEvent(QFocusEvent *e)
{
    this->selectAll();
    QLineEdit::focusInEvent(e);
}

void IpPartLineEdit::keyPressEvent(QKeyEvent *event)
{

    if (event->key() == Qt::Key_Period)
    {
        QString current_tab = current_tab_->text();
        if (next_tab_  && (!current_tab.isEmpty()))
        {
            next_tab_->setFocus();
            next_tab_->selectAll();
        }
    }
    else if (event->key() == Qt::Key_Backspace)
    {
        QString current_tab = current_tab_->text();
        if (prev_tab_ && (current_tab.isEmpty()))
        {
            prev_tab_->setFocus();
            prev_tab_->selectAll();
        }
    }

    QLineEdit::keyPressEvent(event);
}

void IpPartLineEdit::text_edited(const QString& text)
{
    QIntValidator v(0, 255, this);
    QString ipaddr = text;
    int pos = 0;
    QValidator::State state = v.validate(ipaddr, pos);
    if (state == QValidator::Acceptable)
    {
        if (ipaddr.size() > 1)
        {
            if (ipaddr.size() == 2)
            {
                int ipnum = ipaddr.toInt();

                if (ipnum > 25)
                {
                    if (next_tab_)
                    {
                        next_tab_->setFocus();
                        next_tab_->selectAll();
                    }
                }
            }
            else
            {
                if (next_tab_)
                {
                    next_tab_->setFocus();
                    next_tab_->selectAll();
                }
            }
        }
    }
}
