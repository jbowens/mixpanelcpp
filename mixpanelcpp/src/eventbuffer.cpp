#include "eventbuffer.h"
#include "base64.h"
#include <iostream>
#include <curl/curl.h>
#include <ctime>
#include <sstream>
#include <assert.h>

namespace Mixpanel
{
    EventBuffer::EventBuffer(const char* projectToken)
    :   mStillProcessing(true)
    ,   mDeliveryThread(std::thread(&EventBuffer::CheckToDeliver, this))
    {
        mProjectToken = projectToken;
    }


    EventBuffer::~EventBuffer()
    {
        mStillProcessing = false;
        mDeliveryThread.join();
    }


    void
    EventBuffer::CheckToDeliver()
    {
        const char* mixpanelUrl = "http://api.mixpanel.com/track";
        CURL *curl = curl_easy_init();

        // If can't initialize curl, exit out and mark mStillProcessing as false
        // so calls to TrackEvent fail out.
        if (curl == nullptr)
        {
            mStillProcessing = false;
            return;
        }

        Json::Value messagesJsonArray = Json::Value(Json::arrayValue);
        // keep running until mToDeliver is empty and we stop processing events
        while (mStillProcessing || !mToDeliver.empty())
        {
            Json::Value message;
            while (messagesJsonArray.size() < MAX_BATCH_SIZE && mToDeliver.try_pop(message))
            {
                messagesJsonArray.append(message);
            }

            if (messagesJsonArray.size() > 0)
            {
                // encode the message into base64
                Json::FastWriter writer;
                const std::string messageString = writer.write(messagesJsonArray);
                std::string encodedEvent =
                    base64_encode(reinterpret_cast<const unsigned char*>(messageString.c_str()), (unsigned int)messageString.length());
                std::string mixpanelPostData = "data=" + encodedEvent;

                CURLcode res;
                curl_easy_setopt(curl, CURLOPT_URL, mixpanelUrl);
                curl_easy_setopt(curl, CURLOPT_POSTFIELDS, mixpanelPostData.c_str());
                curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1); // prevent signals from timeout
                curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);
                curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

                res = curl_easy_perform(curl);

                // if curl succeeds or we have stopped processing, clear messagesJsonArray
                // otherwise (in failure case), don't clear out and keep trying.
                if (res == CURLE_OK || !mStillProcessing)
                {
                    messagesJsonArray.clear();
                }
            }

            if (mStillProcessing)
            {
                std::this_thread::sleep_for(std::chrono::seconds(5));
            }
        }

        curl_easy_cleanup(curl);
    }


    MP_ErrorCode
    EventBuffer::TrackEvent(const char* eventName, const Json::Value &properties)
    {
        assert(eventName);

        // Note: the reason a reference to properties is passed in and then a copy is made/used
        // instead of directly passing a copy of properties is to deal with the problem where
        // the caller allocates and the callee deallocates, causing issues for debug builds.
        Json::Value propsCopy = Json::Value(properties);

        if (!propsCopy.isObject())
        {
            return MP_INVALID_PROPERTIES;
        }

        // add time/token properties if not yet present
        if (!propsCopy.isMember("time"))
        {
            std::stringstream ss;
            std::time_t curTime = std::time(nullptr);
            ss << curTime;
            propsCopy["time"] = Json::Value(ss.str());
        }

        if (!propsCopy.isMember("token"))
        {
            propsCopy["token"] = Json::Value(mProjectToken);
        }

        // construct actual JSON message to be sent
        Json::Value message;
        message["event"] = eventName;
        message["properties"] = propsCopy;

        // make sure is a valid length
        Json::FastWriter writer;
        const std::string messageString = writer.write(message);
        if (messageString.length() > MAX_SINGLE_MESSAGE_SIZE)
        {
            return MP_MESSAGE_TOO_LONG;
        }

        // make sure the delivery thread is up
        if (!mStillProcessing)
        {
            return MP_DEAD_DELIVERY_THREAD;
        }

        mToDeliver.push(message);
        return MP_SUCCESS;
    }
}
