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

#include "messageexchange.hpp"

std::string MessageExchange::registerNewAgent(std::string prefix) {
  char buffer[32];
  sprintf(buffer, "%d", Rng::next());
  std::string randomComponent(buffer);
  std::string agentId = prefix+"_"+randomComponent;

  this->_agentLock.Lock();
  {
    (this->_subscriptionsByAgentId[agentId]) = std::map<std::string, bool>();
  }
  this->_agentLock.Unlock();
  this->_messageBoxLock.Lock();
  {
    this->_messageBox[agentId] = std::list<PubSubMsg>();
  }
  this->_messageBoxLock.Unlock();
  this->_orphanedMessagesForAgentLock.Lock();
  {
    this->_orphanedMessagesForAgent[agentId] = std::map<std::string, std::list<PubSubMsg> >();
  }
  this->_orphanedMessagesForAgentLock.Unlock();

  return agentId;
}

bool MessageExchange::hasAgent(std::string agentId) {
  bool result = false;
  this->_agentLock.Lock();
  {
    result = !(this->_subscriptionsByAgentId.find(agentId) == this->_subscriptionsByAgentId.end());
  }
  this->_agentLock.Unlock();
  return result;
}
bool MessageExchange::hasSubscription(std::string agentId) {
  bool result = false;
  _subscriptionLock.Lock();
  {
    result = !(this->_subscriptionsByEvent.find(agentId) == this->_subscriptionsByEvent.end());
  }
  _subscriptionLock.Unlock();
  return result;
}
bool MessageExchange::agentIsSubscribedTo(std::string agentId, std::string msgId)
{
  bool result = false;
  bool hasAgent = this->hasAgent(agentId);
  this->_subscriptionLock.Lock();
  {
    if (hasAgent && this->hasSubscription(msgId))
    {
      std::map<std::string, bool>* subscribingAgents = &(_subscriptionsByEvent[msgId]);
      result = !(subscribingAgents->find(agentId) == subscribingAgents->end());
    }
  }
  this->_subscriptionLock.Unlock();

  return result;
}

bool MessageExchange::hasOrphanedMsgId(std::string msgId) {
  bool result = false;
  _orphanedMessagesLock.Lock();
  {
    result = !(this->_orphanedMessages.find(msgId) == this->_orphanedMessages.end());
  }
  _orphanedMessagesLock.Unlock();
  return result;
}
bool MessageExchange::agentHasOrphanedMsgId(std::string agentId, std::string msgId) {
  bool result = false;
  _orphanedMessagesForAgentLock.Lock();
  {
    result = !(this->_orphanedMessagesForAgent[agentId].find(msgId) == this->_orphanedMessagesForAgent[agentId].end());
  }
  _orphanedMessagesForAgentLock.Unlock();
  return result;
}

void MessageExchange::addSubscription(std::string agentId, std::string subscriptionName)
{
  if (this->hasAgent(agentId))
  {
    if (!this->hasSubscription(subscriptionName)) {
      this->_subscriptionsByEvent[subscriptionName] = std::map<std::string, bool>();
    }
    _agentLock.Lock();
    {
      this->_subscriptionsByAgentId[agentId][subscriptionName] = true;
    }
    _agentLock.Unlock();
    _subscriptionLock.Lock();
    {
      this->_subscriptionsByEvent[subscriptionName][agentId] = true;
    }
    _subscriptionLock.Unlock();
    //printf("messaging: subscribing msgId %s for %s\n", subscriptionName.c_str(), agentId.c_str());
    if(this->hasOrphanedMsgId(subscriptionName))
    {
      std::list<PubSubMsg>* orphans;
      std::list<PubSubMsg> pendingMessages;
      this->_orphanedMessagesLock.Lock();
      {
        while(!this->_orphanedMessages[subscriptionName].empty())
        {
          orphans = &(this->_orphanedMessages[subscriptionName]);
          PubSubMsg msg = orphans->front();
          orphans->pop_front();
          pendingMessages.push_back(msg);
        }
      }
      this->_orphanedMessagesLock.Unlock();
      this->_messageBoxLock.Lock();
      {
        std::list<PubSubMsg>::iterator it;
        for(it = pendingMessages.begin(); it != pendingMessages.end(); it++)
        {
          PubSubMsg msg = *it;
          this->_messageBox[agentId].push_back(msg);
        }
      }
      this->_messageBoxLock.Unlock();
    }
    if (this->agentHasOrphanedMsgId(agentId, subscriptionName))
    {
      std::list<PubSubMsg>* orphans;
      std::list<PubSubMsg> pendingMessages;
      this->_orphanedMessagesForAgentLock.Lock();
      {
        while(!this->_orphanedMessagesForAgent[agentId][subscriptionName].empty())
        {
          orphans = &(this->_orphanedMessagesForAgent[agentId][subscriptionName]);
          PubSubMsg msg = orphans->front();
          orphans->pop_front();
          pendingMessages.push_back(msg);
        }
      }
      this->_orphanedMessagesForAgentLock.Unlock();
      this->_messageBoxLock.Lock();
      {
        std::list<PubSubMsg>::iterator it;
        for(it = pendingMessages.begin(); it != pendingMessages.end(); it++)
        {
          PubSubMsg msg = *it;
          this->_messageBox[agentId].push_back(msg);
        }
      }
      this->_messageBoxLock.Unlock();
    }
  }
}

void MessageExchange::publish(std::string senderAgentId, std::string msgId, std::string jsonData) {
  if (this->hasSubscription(msgId)) {
    std::map<std::string, bool>* agents = &(_subscriptionsByEvent[msgId]);
    std::map<std::string, bool>::iterator it;
    this->_messageBoxLock.Lock();
    {
      for(it = agents->begin(); it != agents->end(); it++) {
        PubSubMsg msg(senderAgentId, msgId, jsonData);
        std::string agentId = it->first;
        this->_messageBox[agentId].push_back(msg);
      }
    }
    this->_messageBoxLock.Unlock();
  }
  else {
    this->pushToOrphanQueue(senderAgentId, msgId, jsonData);
  }
}
void MessageExchange::publish(std::string targetAgentId, std::string senderAgentId, std::string msgId, std::string jsonData) {
  if (this->agentIsSubscribedTo(targetAgentId, msgId)) {
    this->_messageBoxLock.Lock();
    {
      PubSubMsg msg(senderAgentId, msgId, jsonData);
      _messageBox[targetAgentId].push_back(msg);
    }
    this->_messageBoxLock.Unlock();
  }
  else {
    // agent isn't subscribed to msg.. fail?
    //printf("agent %s isn't subscribed to %s .. bailing!\n", targetAgentId.c_str(), msgId.c_str());
    //exit(EXIT_FAILURE);
    this->pushToOrphanQueueForAgent(targetAgentId, senderAgentId, msgId, jsonData);
  }
}

void MessageExchange::pushToOrphanQueue(std::string senderAgentId, std::string msgId, std::string jsonData) {
  bool orphanedMsgIdExists = this->hasOrphanedMsgId(msgId);
  this->_orphanedMessagesLock.Lock();
  {
    if (!orphanedMsgIdExists) {
      this->_orphanedMessages[msgId] = std::list<PubSubMsg>();
    }
    PubSubMsg msg(senderAgentId, msgId, jsonData);
    this->_orphanedMessages[msgId].push_back(msg);
  }
  this->_orphanedMessagesLock.Unlock();
}

void MessageExchange::pushToOrphanQueueForAgent(std::string targetAgentId, std::string senderAgentId, std::string msgId, std::string jsonData) {
  bool orphanedMsgIdExists = this->agentHasOrphanedMsgId(targetAgentId, msgId);
  this->_orphanedMessagesForAgentLock.Lock();
  {
    if (!orphanedMsgIdExists) {
      this->_orphanedMessagesForAgent[targetAgentId][msgId] = std::list<PubSubMsg>();
    }
    PubSubMsg msg(senderAgentId, msgId, jsonData);
    this->_orphanedMessagesForAgent[targetAgentId][msgId].push_back(msg);
  }
  this->_orphanedMessagesForAgentLock.Unlock();
}

bool MessageExchange::messagesPendingFor(std::string agentId)
{
  bool result = false;
  bool hasAgent = this->hasAgent(agentId);
  if (hasAgent) {
    this->_messageBoxLock.Lock();
    {
      result = !(this->_messageBox[agentId].empty());
    }
    this->_messageBoxLock.Unlock();
  }
  return result;
}

PubSubMsg MessageExchange::unshiftNextMsgFor(std::string agentId)
{
  PubSubMsg msg("","","");
  bool messagesPending = this->messagesPendingFor(agentId);
  if (messagesPending) {
    this->_messageBoxLock.Lock();
    {
      msg = this->_messageBox[agentId].front();
      this->_messageBox[agentId].pop_front();
    }
    this->_messageBoxLock.Unlock();
  }
  return msg;
}
