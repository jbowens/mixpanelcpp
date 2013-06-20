#include <iostream>
#include <mixpanel/eventbuffer.h>

int main(int argc, const char * argv[])
{
	Mixpanel::EventBuffer *eb = new Mixpanel::EventBuffer("421a131ea65e5dfb2f2f3cade8504774");
	
	// Construct the properties to be associated with an event
	Json::Reader reader;
	Json::Value properties;
	reader.parse("{\"distinct_id\":\"123123123123\",\"action\":\"play\"}", properties);
	
	// Asynchronously track an event named "event1" with the properties created above
	Mixpanel::MP_ErrorCode err;
	err = eb->TrackEvent("event1", properties);
	
	if (err == Mixpanel::MP_SUCCESS)
	{
		std::cout << "event enqueued" << std::endl;
	}
	else
	{
		std::cout << err << std::endl;
	}
	
	// Destructor of EventBuffer will join on a delivery thread until all messages are sent to Mixpanel
    delete eb;
	return 0;
}

