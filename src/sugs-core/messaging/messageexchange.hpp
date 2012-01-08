/* Copyright 2011 Jeffery Olson <olson.jeffery@gmail.com>. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification, are
 * permitted provided that the following conditions are met:
 *
 *  1. Redistributions of source code must retain the above copyright notice, this list of
 *     conditions and the following disclaimer.
 *
 *  2. Redistributions in binary form must reproduce the above copyright notice, this list
 *     of conditions and the following disclaimer in the documentation and/or other materials
 *     provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> ``AS IS'' AND ANY
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL
 * JEFFERY OLSON <OLSON.JEFFERY@GMAIL.COM> OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * The views and conclusions contained in the software and documentation are those of the
 * authors and should not be interpreted as representing official policies, either expressed
 * or implied, of Jeffery Olson <olson.jeffery@gmail.com>.
 *
 */

#ifndef __inprocmessenger_hpp__
#define __inprocmessenger_hpp__

#include <string>
#include <map>
#include <jsapi.h>
#include <SFML/System.hpp>

#include "../common.hpp"
#include "pubsub.hpp"

class MessageExchange
{
  public:
    MessageExchange()
    {
    }

    std::string registerNewAgent(std::string prefix);
    bool hasAgent(std::string workerId);
    bool workerIsSubscribedTo(std::string workerId, std::string msgId);
    bool hasSubscription(std::string subscriptionName);
    void addSubscription(std::string workerId, std::string subscriptionName);
    void publish(std::string senderAgentId, std::string msgId, std::string jsonData);
    void publish(std::string targetAgentId, std::string senderAgentId, std::string msgId, std::string jsonData);

    bool messagesPendingFor(std::string workerId);
    PubSubMsg unshiftNextMsgFor(std::string workerId);
  private:
    void pushToOrphanQueue(std::string sendingAgentName, std::string subscriptionName, std::string jsonData);
    void pushToOrphanQueueForAgent(std::string targetAgentName, std::string sendingAgentNAme, std::string msgId, std::string jsonData);
    bool hasOrphanedMsgId(std::string msgId);
    bool workerHasOrphanedMsgId(std::string workerId, std::string msgId);
    std::map<std::string, std::map<std::string, bool> > _subscriptionsByEvent;
    sf::Mutex _subscriptionLock;
    std::map<std::string, std::map<std::string, bool> > _subscriptionsByAgentId;
    sf::Mutex _workerLock;
    std::map<std::string, std::list<PubSubMsg> > _messageBox;
    sf::Mutex _messageBoxLock;
    std::map<std::string, std::list<PubSubMsg> > _orphanedMessages;
    sf::Mutex _orphanedMessagesLock;
    std::map<std::string, std::map<std::string, std::list<PubSubMsg> > > _orphanedMessagesForAgent;
    sf::Mutex _orphanedMessagesForAgentLock;
};
#endif
