#ifndef RXDEBUG_H
#define RXDEBUG_H

#define RX_EXCEPTION(msg) std::runtime_error{msg}
#ifdef _DEBUG
#define RX_ASSERT_MSG(x, msg) assert((x) && (msg))
#define RX_ASSERT(x) assert((x))
#else
#define RX_ASSERT_MSG(x, msg)
#define RX_ASSERT(x)
#endif

#endif