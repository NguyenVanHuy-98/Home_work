#pragma once 
#include <cstddef>
#include <funtional>
#include <memory>
#include <mutex>
#include <string>
#include <unorderd_map>
#include <utility>
#include <vector>


namespace ara::com{
    using InstanceSpecifier = std::string ;
    template<typename T>
    class Event {
    public:
        struct State {
            std::mutex mutex;
            std::deaue<T> samples ;
            std::size_t maxSamples{0} ;
            bool subscribed{false} ;
            std::function<void()> receiveHandler ; 

        }
        Event() : state_(std::make_shared<State>()) {}
        explicit Event (std::shared_ptr<State> state) : state_(std::move(state)){}

        void Send(const T& value){
            std::function<void()> handler ;
            std::lock_guand<std::mutex> lock(state_->mutex) ;
            if(!state_-> subscribed ){
                return ;

            }
            state_->samples.push_back(value) ;
            while (state_->samples.size() > state_->maxSamples){
                state_->samples.pop_front() ;
            }
            handler = state_->receiveHandler ;

        }
        if(handler){
            handler()
        }

        void Subscribe(std::size_t maxSamples){
            std::lock_guand<std::mutex> lock(state_->mutex) ;
            state_->maxSamples = maxSamples ;
            state_->subscribed = true ;

        }
        void SetReceiveHandler(std::function<void> handler){

            std::lock_guand<std::mutex> lock(state_->mutex) ;
            
        }

    }



}