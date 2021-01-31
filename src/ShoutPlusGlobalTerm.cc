#include "ShoutPlusGlobalTerm.h"

Define_Module(ShoutPlusGlobalTerm);

void ShoutPlusGlobalTerm::handleMessage(omnetpp::cMessage* recvMsg) {
  if (status == Status::INITIATOR) {
    if (recvMsg->getKind() == MsgKind::TIMER) { //A1
      root = true;
      msg = new omnetpp::cMessage("query", MsgKind::QUERY);
      localBroadcast(msg);
      status = Status::ACTIVE;
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else if(status == Status::IDLE) { // A2
    if (recvMsg->getKind() == MsgKind::QUERY) {
      parent = recvMsg->getArrivalGate()->getIndex();
      msg = new omnetpp::cMessage("yes", MsgKind::YES);
      send(msg, "port$o", parent);
      counter++;
      if (gateSize("port$o") == counter) status = Status::DONE;
      else {
        localFlooding(recvMsg);
        status = Status::ACTIVE;
      }
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else if(status == Status::ACTIVE) {
    int senderPort = recvMsg->getArrivalGate()->getIndex();
    counter++;
    if (recvMsg->getKind() == MsgKind::YES) {
      children.push_back(senderPort);
      delete recvMsg;
    }
    else if (recvMsg->getKind() == MsgKind::QUERY) {
      delete recvMsg;
      if (counter == gateSize("port$o")) 
        if (children.empty()) {
          msg = new omnetpp::cMessage("check", MsgKind::CHECK);
          send(msg, "port$o", parent);
        }
    }
    else if (recvMsg->getKind() == MsgKind::CHECK) {
      checkCounter++;
      delete recvMsg;
      if (checkCounter == children.size()) {
        if (!root) {
          msg = new omnetpp::cMessage("check", MsgKind::CHECK);
          send(msg, "port$o", parent);
        }
        else {
          msg = new omnetpp::cMessage("end", MsgKind::TERMINATE);
          localMulticast(msg, children);
          status = Status::DONE;
        }
      }
    }
    else if (recvMsg->getKind() == MsgKind::TERMINATE) {
      localMulticast(recvMsg, children);
      if (children.empty())
        delete recvMsg;
      status = Status::DONE;
    }
    else
      omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
  }
  else
    omnetpp::cRuntimeError("Invalid event to status: %s", status.str());
}