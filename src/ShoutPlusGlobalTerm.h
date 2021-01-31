//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#if !defined(SHOUT_PLUS_GLOBAL_TERM_H)
#define SHOUT_PLUS_GLOBAL_TERM_H

#include "ShoutPlus.h"

class ShoutPlusGlobalTerm : public ShoutPlus {
protected:
  /** @brief Counter registers the number of check msg a parent has received from its children */
  unsigned checkCounter;
public:
  ShoutPlusGlobalTerm() : checkCounter(0) { }
  void handleMessage(omnetpp::cMessage*) override;
};

#endif // SHOUT_PLUS_GLOBAL_TERM_H
