#ifndef RXAPI_H
#define RXAPI_H

#ifdef rdxengine_EXPORTS
#define RX_API __declspec(dllexport)
#else
#define RX_API __declspec(dllimport)
#endif

#endif