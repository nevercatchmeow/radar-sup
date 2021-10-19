#pragma once
#include <QLineEdit>
class QWidget;
class QFocusEvent;
class QKeyEvent;

class IpPartLineEdit : public QLineEdit
{
    Q_OBJECT
public:
    IpPartLineEdit(QWidget *parent = nullptr);
    ~IpPartLineEdit(void);

    void set_prevtab_edit(QLineEdit *currenttab, QLineEdit *prevtab) { current_tab_ = currenttab, prev_tab_ = prevtab; }
    void set_nexttab_edit(QLineEdit *currenttab, QLineEdit *nexttab) {current_tab_ = currenttab, next_tab_ = nexttab; }

    //void

protected:
    virtual void focusInEvent(QFocusEvent *e);
    virtual    void keyPressEvent(QKeyEvent *event);

private slots:
    void text_edited(const QString& text);

private:
    QLineEdit *current_tab_, *prev_tab_, *next_tab_;
};
