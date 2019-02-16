#include <exception>

#define EXCEPTION(name) class name final : public std::exception { public: using std::exception::exception; };
