#pragma once

namespace nike
{
class shoe
{
public:
   shoe() = default;
   virtual ~shoe() = default;
   virtual void do_it() = 0;

private:
   shoe(const shoe&) = delete;
   shoe& operator=(const shoe&) = delete;
};
}
