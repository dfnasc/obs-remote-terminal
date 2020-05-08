#ifndef COMMAND_H
#define COMMAND_H

#include <QString>
#include <QStringList>
#include <QObject>

class Command : public QObject
{
    Q_OBJECT
public:
    Command(const QString& name, QStringList args);

    QStringList& args();
    QString name() const;

    bool validateArgumentNumber(int expectedArgs) const;

    static Command* parse(const QString& text);
private:
    QString m_name;
    QStringList m_args;
};

#endif // COMMAND_H
