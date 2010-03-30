/*
 * Copyright (c) 1996-2009 Barton P. Miller
 * 
 * We provide the Paradyn Parallel Performance Tools (below
 * described as "Paradyn") on an AS IS basis, and do not warrant its
 * validity or performance.  We reserve the right to update, modify,
 * or discontinue this software at any time.  We shall have no
 * obligation to supply such updates or modifications or any other
 * form of support to you.
 * 
 * By your use of Paradyn, you understand and agree that we (or any
 * other person or entity with proprietary rights in Paradyn) are
 * under no obligation to provide either maintenance services,
 * update services, notices of latent defects, or correction of
 * defects for Paradyn.
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#ifndef SW_INTERNAL_H_
#define SW_INTERNAL_H_

#include <set>
#include "common/h/addrRange.h"
#include "stackwalk/h/framestepper.h"

namespace Dyninst {
namespace Stackwalker {

struct ltstepper
{
   bool operator()(const FrameStepper *a, const FrameStepper *b) const
   {
      return a->getPriority() < b->getPriority();
   }
};

typedef std::set<FrameStepper *, ltstepper> StepperSet;

class AddrRangeStepper : public addrRange
{
public:
   Address start;
   Address end;
   StepperSet steppers;

   AddrRangeStepper() { }
   AddrRangeStepper(Address s, Address e) { start = s; end = e; }

   virtual Address get_address() const { return start; } 
   virtual unsigned long get_size() const { return end - start; }
   virtual ~AddrRangeStepper() { }
};

class FrameFuncStepperImpl : public FrameStepper
{
 private:
   FrameStepper *parent;
   FrameFuncHelper *helper;
 public:
   FrameFuncStepperImpl(Walker *w, 
                        FrameStepper *parent_,
                        FrameFuncHelper *helper_);
   virtual ~FrameFuncStepperImpl();
   virtual gcframe_ret_t getCallerFrame(const Frame &in, 
                                        Frame &out);
   static gcframe_ret_t getBasicCallerFrame(const Frame &in, 
					    Frame &out);
   virtual unsigned getPriority() const;
};

class BottomOfStackStepperImpl : public FrameStepper {
private:
   BottomOfStackStepper *parent;
   std::vector<std::pair<Address, Address> > ra_stack_tops;
   std::vector<std::pair<Address, Address> > sp_stack_tops;
   bool libc_init;
   bool aout_init;
   bool libthread_init;
   void initialize();
public:
   BottomOfStackStepperImpl(Walker *w, BottomOfStackStepper *parent);
   virtual gcframe_ret_t getCallerFrame(const Frame &in, Frame &out);
   virtual unsigned getPriority() const;
   virtual void registerStepperGroup(StepperGroup *group);
   virtual void newLibraryNotification(LibAddrPair *la, lib_change_t change);
   virtual ~BottomOfStackStepperImpl();  
};

}
}

#endif
