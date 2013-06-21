#include <iostream>
#include <mixpanel/eventbuffer.h>

void TestMixpanel()
{
    Mixpanel::EventBuffer *eb = new Mixpanel::EventBuffer("<Project token here>");

    // Construct the properties to be associated with an event
    Json::Value properties = Json::Value(Json::objectValue);
    properties["distinct_id"] = "<some distinct id>";
    properties["action"] = "play";

    // Asynchronously track an event. Project token and time of event are automatically handled by TrackEvent.
    Mixpanel::MP_ErrorCode err;
    err = eb->TrackEvent("<some event name>", properties);

    if (err == Mixpanel::MP_SUCCESS)
    {
        std::cout << "event enqueued" << std::endl;
    }
    else
    {
        std::cout << "event not enqueued: " << err << std::endl;
    }

    // Destructor of EventBuffer will join on a delivery thread until all messages are sent to Mixpanel
    delete eb;
}

int main(int argc, const char * argv[])
{
    TestMixpanel();
    return 0;
}

