#ifndef MIXPANEL_EVENTBUFFER_H
#define MIXPANEL_EVENTBUFFER_H

#include <json/json.h>
#include <thread>
#include "mp_concurrent_queue.h"


/*
 * Note: If building as a dll, define MIXPANEL_BUILDING_DLL.
 * If using the library as a dll, define MIXPANEL_USING_DLL.
 * Otherwise, leave both undefined.
 */
#ifdef MIXPANEL_BUILDING_DLL
#define IMPORT_EXPORT __declspec(dllexport)
#elif defined MIXPANEL_USING_DLL
#define IMPORT_EXPORT __declspec(dllimport)
#else
#define IMPORT_EXPORT
#endif

namespace Mixpanel
{
    typedef enum
    {
        MP_SUCCESS                  = 0,
        MP_INVALID_PROPERTIES       = 1 << 0,
        MP_DEAD_DELIVERY_THREAD     = 1 << 1,
        MP_MESSAGE_TOO_LONG         = 1 << 2
    } MP_ErrorCode;

    class IMPORT_EXPORT EventBuffer
    {
    public:
        /*
         * @param[in] projectToken - Token of the Mixpanel project to track events for
         */
        EventBuffer(const char* projectToken);

        ~EventBuffer();

        /*
         * Tracks a Mixpanel event. Note that this does not deliver messages
         * immediately as they are batched together and handled by a delivery thread.
         * @param[in] eventName - A human readable name for the event
         * @param[in] properties - A JSON object associating properties with the event.
         * @return - MP_ErrorCode specifying success enqueuing message or some type of failure.
         */
        MP_ErrorCode TrackEvent(const char* eventName, const Json::Value &properties);

        /*
         * Tracks a Mixpanel event. Note that this does not deliver messages
         * immediately as they are batched together and handled by a delivery thread.
         * @param[in] eventName - A human readable name for the event
         * @param[in] properties - A string representing a JSON object associating properties with the event
         * @return - MP_ErrorCode specifying success enqueuing message or some type of failure.
         */
        MP_ErrorCode TrackEvent(const char* eventName, const char* propsString);

    private:
        /*
         * Function called by the delivery thread responsible for batching
         * up events from trackEvent and sending them to Mixpanel
         */
        void CheckToDeliver();

        Concurrency::concurrent_queue<Json::Value> mToDeliver;
        std::string mProjectToken;
        bool mStillProcessing;
        std::thread mDeliveryThread;

    };

    static const int MAX_BATCH_SIZE = 30;
    static const int MAX_SINGLE_MESSAGE_SIZE = 16384;
}

#endif // MIXPANEL_EVENTBUFFER_H
