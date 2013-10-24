#include <QCoreApplication>
#include "Server.h"

int main(int argc, char *argv[])
{
    int debug = 1;
    QString settings;
    QCoreApplication app(argc, argv);
    QStringList param(QCoreApplication::arguments());
    if(param.length()>1)
    {
        foreach(QString str,param)
        {

            if(str.toLower().contains("-v"))
            {
                QStringList temp = str.simplified().split("=");
                if(temp.length()>1)
                    debug = str.simplified().split("=").at(1).toInt();
            }
            else if(str.toLower().contains("-settings"))
            {
                QStringList temp = str.simplified().split("=");
                if(temp.length()>1)
                    settings = str.simplified().split("=").at(1);
            }
        }
    }
    bool init;
    Server *server = new Server(debug,settings, init);
    if(init)
        return app.exec();
    else
    {
        delete server;
        return -1;
    }
}
