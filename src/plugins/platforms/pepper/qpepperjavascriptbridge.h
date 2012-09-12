#include <QtCore/QObject>

#include "qpepperinstance.h"

#include <ppapi/cpp/instance.h>

class QPepperJavascriptBridge : public QObject
{
public:
    QPepperJavascriptBridge(pp::Instance *instance);
    void evalSource(const QByteArray &code);
    void evalFile(const QString &fileName);
private:
    pp::Instance *m_instance;
};
