#include "command.h"

Command::Command(const QString& name, QStringList args)
 : QObject(), m_name(name), m_args(args) {

}

QStringList& Command::args() {
    return m_args;
}

QString Command::name() const {
    return m_name;
}

Command* Command::parse(const QString& text) {

    QStringList args;
    QString tmp = text;

    tmp = tmp.replace(QRegExp("\r\n"), "");
    tmp = tmp.replace(QRegExp("\\s\\+"), "");

    args = tmp.split(" ");

    if (args.length() == 0) {
        return nullptr;
    }

    return new Command(args.first(), args);
}

bool Command::validateArgumentNumber(int expectedArgs) const {
    return m_args.length() == expectedArgs+1;
}
