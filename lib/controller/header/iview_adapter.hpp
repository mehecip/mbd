#include "node.hpp"

namespace mbd
{

class iview_adapter
{
	public:
    	virtual void to_model(node* n) = 0;
		virtual void to_view(node const* n, std::size_t tick_) = 0;

        ~iview_adapter() {};
};

}