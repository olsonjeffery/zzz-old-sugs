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

std::string MessageExchange::registerNewWorker(std::string prefix) {
  char buffer[32];
  sprintf(buffer, "%d", Rng::next());
  std::string randomComponent(buffer);
  std::string workerId = prefix+"_"+randomComponent;

  this->_workerLock.Lock();
  {
    (this->_subscriptionsByWorkerId[workerId]) = std::map<std::string, bool>();
  }
  this->_workerLock.Unlock();
  this->_messageBoxLock.Lock();
  {
    this->_messageBox[workerId] = std::list<PubSubMsg>();
  }
  this->_messageBoxLock.Unlock();
  this->_orphanedMessagesForWorkerLock.Lock();
  {
    this->_orphanedMessagesForWorker[workerId] = std::map<std::string, std::list<PubSubMsg> >();
  }
  this->_orphanedMessagesForWorkerLock.Unlock();

  return workerId;
}

bool MessageExchange::hasWorker(std::string workerId) {
  bool result = false;
  this->_workerLock.Lock();
  {
    result = !(this->_subscriptionsByWorkerId.find(workerId) == this->_subscriptionsByWorkerId.end());
  }
  this->_workerLock.Unlock();
  return result;
}
bool MessageExchange::hasSubscription(std::string workerId) {
  bool result = false;
  _subscriptionLock.Lock();
  {
    result = !(this->_subscriptionsByEvent.find(workerId) == this->_subscriptionsByEvent.end());
  }
  _subscriptionLock.Unlock();
  return result;
}
bool MessageExchange::workerIsSubscribedTo(std::string workerId, std::string msgId)
{
  bool result = false;
  bool hasWorker = this->hasWorker(workerId);
  this->_subscriptionLock.Lock();
  {
    if (hasWorker && this->hasSubscription(msgId))
    {
      std::map<std::string, bool>* subscribingWorkers = &(_subscriptionsByEvent[msgId]);
      result = !(subscribingWorkers->find(workerId) == subscribingWorkers->end());
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
bool MessageExchange::workerHasOrphanedMsgId(std::string workerId, std::string msgId) {
  bool result = false;
  _orphanedMessagesForWorkerLock.Lock();
  {
    result = !(this->_orphanedMessagesForWorker[workerId].find(msgId) == this->_orphanedMessagesForWorker[workerId].end());
  }
  _orphanedMessagesForWorkerLock.Unlock();
  return result;
}

void MessageExchange::addSubscription(std::string workerId, std::string subscriptionName)
{
  if (this->hasWorker(workerId))
  {
    if (!this->hasSubscription(subscriptionName)) {
      this->_subscriptionsByEvent[subscriptionName] = std::map<std::string, bool>();
    }
    _workerLock.Lock();
    {
      this->_subscriptionsByWorkerId[workerId][subscriptionName] = true;
    }
    _workerLock.Unlock();
    _subscriptionLock.Lock();
    {
      this->_subscriptionsByEvent[subscriptionName][workerId] = true;
    }
    _subscriptionLock.Unlock();
    //printf("messaging: subscribing msgId %s for %s\n", subscriptionName.c_str(), workerId.c_str());
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
          this->_messageBox[workerId].push_back(msg);
        }
      }
      this->_messageBoxLock.Unlock();
    }
    if (this->workerHasOrphanedMsgId(workerId, subscriptionName))
    {
      std::list<PubSubMsg>* orphans;
      std::list<PubSubMsg> pendingMessages;
      this->_orphanedMessagesForWorkerLock.Lock();
      {
        while(!this->_orphanedMessagesForWorker[workerId][subscriptionName].empty())
        {
          orphans = &(this->_orphanedMessagesForWorker[workerId][subscriptionName]);
          PubSubMsg msg = orphans->front();
          orphans->pop_front();
          pendingMessages.push_back(msg);
        }
      }
      this->_orphanedMessagesForWorkerLock.Unlock();
      this->_messageBoxLock.Lock();
      {
        std::list<PubSubMsg>::iterator it;
        for(it = pendingMessages.begin(); it != pendingMessages.end(); it++)
        {
          PubSubMsg msg = *it;
          this->_messageBox[workerId].push_back(msg);
        }
      }
      this->_messageBoxLock.Unlock();
    }
  }
}

void MessageExchange::publish(std::string senderWorkerId, std::string msgId, std::string jsonData) {
  if (this->hasSubscription(msgId)) {
    std::map<std::string, bool>* workers = &(_subscriptionsByEvent[msgId]);
    std::map<std::string, bool>::iterator it;
    this->_messageBoxLock.Lock();
    {
      for(it = workers->begin(); it != workers->end(); it++) {
        PubSubMsg msg(senderWorkerId, msgId, jsonData);
        std::string workerId = it->first;
        this->_messageBox[workerId].push_back(msg);
      }
    }
    this->_messageBoxLock.Unlock();
  }
  else {
    this->pushToOrphanQueue(senderWorkerId, msgId, jsonData);
  }
}
void MessageExchange::publish(std::string targetWorkerId, std::string senderWorkerId, std::string msgId, std::string jsonData) {
  if (this->workerIsSubscribedTo(targetWorkerId, msgId)) {
    this->_messageBoxLock.Lock();
    {
      PubSubMsg msg(senderWorkerId, msgId, jsonData);
      _messageBox[targetWorkerId].push_back(msg);
    }
    this->_messageBoxLock.Unlock();
  }
  else {
    // worker isn't subscribed to msg.. fail?
    //printf("worker %s isn't subscribed to %s .. bailing!\n", targetWorkerId.c_str(), msgId.c_str());
    //exit(EXIT_FAILURE);
    this->pushToOrphanQueueForWorker(targetWorkerId, senderWorkerId, msgId, jsonData);
  }
}

void MessageExchange::pushToOrphanQueue(std::string senderWorkerId, std::string msgId, std::string jsonData) {
  bool orphanedMsgIdExists = this->hasOrphanedMsgId(msgId);
  this->_orphanedMessagesLock.Lock();
  {
    if (!orphanedMsgIdExists) {
      this->_orphanedMessages[msgId] = std::list<PubSubMsg>();
    }
    PubSubMsg msg(senderWorkerId, msgId, jsonData);
    this->_orphanedMessages[msgId].push_back(msg);
  }
  this->_orphanedMessagesLock.Unlock();
}

void MessageExchange::pushToOrphanQueueForWorker(std::string targetWorkerId, std::string senderWorkerId, std::string msgId, std::string jsonData) {
  bool orphanedMsgIdExists = this->workerHasOrphanedMsgId(targetWorkerId, msgId);
  this->_orphanedMessagesForWorkerLock.Lock();
  {
    if (!orphanedMsgIdExists) {
      this->_orphanedMessagesForWorker[targetWorkerId][msgId] = std::list<PubSubMsg>();
    }
    PubSubMsg msg(senderWorkerId, msgId, jsonData);
    this->_orphanedMessagesForWorker[targetWorkerId][msgId].push_back(msg);
  }
  this->_orphanedMessagesForWorkerLock.Unlock();
}

bool MessageExchange::messagesPendingFor(std::string workerId)
{
  bool result = false;
  bool hasWorker = this->hasWorker(workerId);
  if (hasWorker) {
    this->_messageBoxLock.Lock();
    {
      result = !(this->_messageBox[workerId].empty());
    }
    this->_messageBoxLock.Unlock();
  }
  return result;
}

PubSubMsg MessageExchange::unshiftNextMsgFor(std::string workerId)
{
  PubSubMsg msg("","","");
  bool messagesPending = this->messagesPendingFor(workerId);
  if (messagesPending) {
    this->_messageBoxLock.Lock();
    {
      msg = this->_messageBox[workerId].front();
      this->_messageBox[workerId].pop_front();
    }
    this->_messageBoxLock.Unlock();
  }
  return msg;
}
