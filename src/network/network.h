#ifndef NETWORK_H
#define NETWORK_H

#include <QNetworkAccessManager>
namespace Network {
inline QNetworkAccessManager &manager() {
    static QNetworkAccessManager m;
    return m;
}
} // namespace Network

#endif // NETWORK_H
