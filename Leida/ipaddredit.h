#include <QWidget>

class QLineEdit;
class QLabel;
class IpPartLineEdit;

class IpAddrEdit : public QWidget
{
    Q_OBJECT
public:
    IpAddrEdit(QWidget* pParent = nullptr);
    ~IpAddrEdit();

    void settext(const QString &text);
    QString text();
    void getIp(QString& ip1, QString& ip2, QString& ip3, QString& ip4);
    void setStyleSheet(const QString &styleSheet);

signals:
    void textchanged(const QString& text);
    void textedited(const QString &text);

private slots:
    void textchangedslot(const QString& text);
    void texteditedslot(const QString &text);

private:
    IpPartLineEdit *ip_part1_;
    IpPartLineEdit *ip_part2_;
    IpPartLineEdit *ip_part3_;
    //QLineEdit *ip_part4_;
    IpPartLineEdit *ip_part4_;

    QLabel *labeldot1_;
    QLabel *labeldot2_;
    QLabel *labeldot3_;
};
