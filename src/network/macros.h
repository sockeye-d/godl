//
// Created by fish on 5/31/25.
//

#ifndef MACROS_H
#define MACROS_H

#ifdef GITHUB_TOKEN
#define AUTH(request) (request.setRawHeader("Authorization"_ba, u"Bearer %1"_s.arg(GITHUB_TOKEN).toUtf8()))
#else
#define auth(request)
#endif

#endif //MACROS_H
