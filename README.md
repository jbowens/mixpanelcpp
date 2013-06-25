#Mixpanel C++ library

This is a barebones Mixpanel library for C++. It is currently only able
to track normal mixpanel events and is not setup for Mixpanel People.

The library has two dependencies: libcurl and jsoncpp, which are provided in
their respective subdirectories.

This library has been tested in Windows and OSX, and Visual Studio / Xcode
projects are provided. For OSX, the Running from Xcode should be enough
to build the project. For Windows, you first need to build jsoncpp
(you can use the provided build_jsoncpp.bat script).

## Usage
    #include <mixpanel/eventbuffer.h>
    
    void TrackMixpanel()
    {
        Mixpanel::EventBuffer eb = EventBuffer("MIXPANEL_PROJECT_TOKEN");
        Mixpanel::MP_ErrorCode err;
        // You can construct Json objects and pass them to the library
        Json::Vaue properties = Json::Value(Json::objectValue);
        properties["some_key"] = "some_value";
        err = eb->TrackEvent("some_event_name", properties);

        // Or you can just pass strings representing Json objects
        // err = eb->TrackEvent("some_other_event", "{\"distinct_id\" : \"some_id\"}");
        
        if (err == Mixpanel::MP_SUCCESS) 
        {
            // event has been queued and will be tracked
        }
    }
    


## Runnable sample

Example usage can be found in the mixpanelcppexample subdirectory with an
Xcode and Visual Studio project. The example can be easily run in Xcode just by running the mixpanelcppexample target. For the Visual Studio project, copy the lib (and dll if building dynamically) from mixpanelcpp/lib/{CONFIG} to mixpanelcppexample/bin/{CONFIG}. Also, you will need to copy over the libcurl dll's into the same directory.


