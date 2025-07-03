#ifndef NETWORKRESPONSECODE_H
#define NETWORKRESPONSECODE_H

#include <QMap>
#include <QObject>
#include <QtQml/qqmlregistration.h>

using namespace Qt::StringLiterals;

class NetworkResponseCode : public QObject
{
    Q_OBJECT

    QMap<int, QString> m_names = QMap<int, QString>({{1, u"ConnectionRefusedError"_s},
                                                     {2, u"RemoteHostClosedError"_s},
                                                     {3, u"HostNotFoundError"_s},
                                                     {4, u"TimeoutError"_s},
                                                     {5, u"OperationCanceledError"_s},
                                                     {6, u"SslHandshakeFailedError"_s},
                                                     {7, u"TemporaryNetworkFailureError"_s},
                                                     {8, u"NetworkSessionFailedError"_s},
                                                     {9, u"BackgroundRequestNotAllowedError"_s},
                                                     {10, u"TooManyRedirectsError"_s},
                                                     {11, u"InsecureRedirectError"_s},
                                                     {101, u"ProxyConnectionRefusedError"_s},
                                                     {102, u"ProxyConnectionClosedError"_s},
                                                     {103, u"ProxyNotFoundError"_s},
                                                     {104, u"ProxyTimeoutError"_s},
                                                     {105, u"ProxyAuthenticationRequiredError"_s},
                                                     {201, u"ContentAccessDenied"_s},
                                                     {202, u"ContentOperationNotPermittedError"_s},
                                                     {203, u"ContentNotFoundError"_s},
                                                     {204, u"AuthenticationRequiredError"_s},
                                                     {205, u"ContentReSendError"_s},
                                                     {206, u"ContentConflictError"_s},
                                                     {207, u"ContentGoneError"_s},
                                                     {401, u"InternalServerError"_s},
                                                     {402, u"OperationNotImplementedError"_s},
                                                     {403, u"ServiceUnavailableError"_s},
                                                     {301, u"ProtocolUnknownError"_s},
                                                     {302, u"ProtocolInvalidOperationError"_s},
                                                     {99, u"UnknownNetworkError"_s},
                                                     {199, u"UnknownProxyError"_s},
                                                     {299, u"UnknownContentError"_s},
                                                     {399, u"ProtocolFailure"_s},
                                                     {499, u"UnknownServerError"_s}});

public:
    static NetworkResponseCode *instance()
    {
        static auto instance = new NetworkResponseCode();
        return instance;
    }

    explicit NetworkResponseCode(QObject *parent = nullptr);
    Q_INVOKABLE QString error(int error) const { return m_names.value(error); }
};

#endif // NETWORKRESPONSECODE_H
