/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: nil; c-basic-offset: 2 -*- */
/* vim: set ts=8 sts=2 et sw=2 tw=80: */
/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "mozilla/dom/WheelEvent.h"
#include "mozilla/MouseEvents.h"
#include "prtime.h"

namespace mozilla {
namespace dom {

WheelEvent::WheelEvent(EventTarget* aOwner,
                       nsPresContext* aPresContext,
                       WidgetWheelEvent* aWheelEvent)
  : MouseEvent(aOwner, aPresContext,
               aWheelEvent ? aWheelEvent :
                             new WidgetWheelEvent(false, eVoidEvent, nullptr))
  , mAppUnitsPerDevPixel(0)
{
  if (aWheelEvent) {
    mEventIsInternal = false;
    // If the delta mode is pixel, the WidgetWheelEvent's delta values are in
    // device pixels.  However, JS contents need the delta values in CSS pixels.
    // We should store the value of mAppUnitsPerDevPixel here because
    // it might be changed by changing zoom or something.
    if (aWheelEvent->deltaMode == nsIDOMWheelEvent::DOM_DELTA_PIXEL) {
      mAppUnitsPerDevPixel = aPresContext->AppUnitsPerDevPixel();
    }
  } else {
    mEventIsInternal = true;
    mEvent->mTime = PR_Now();
    mEvent->refPoint.x = mEvent->refPoint.y = 0;
    mEvent->AsWheelEvent()->inputSource = nsIDOMMouseEvent::MOZ_SOURCE_UNKNOWN;
  }
}

NS_IMPL_ADDREF_INHERITED(WheelEvent, MouseEvent)
NS_IMPL_RELEASE_INHERITED(WheelEvent, MouseEvent)

NS_INTERFACE_MAP_BEGIN(WheelEvent)
NS_INTERFACE_MAP_END_INHERITING(MouseEvent)

void
WheelEvent::InitWheelEvent(const nsAString& aType,
                           bool aCanBubble,
                           bool aCancelable,
                           nsGlobalWindow* aView,
                           int32_t aDetail,
                           int32_t aScreenX,
                           int32_t aScreenY,
                           int32_t aClientX,
                           int32_t aClientY, 
                           uint16_t aButton,
                           EventTarget* aRelatedTarget,
                           const nsAString& aModifiersList,
                           double aDeltaX,
                           double aDeltaY,
                           double aDeltaZ,
                           uint32_t aDeltaMode)
{
  MouseEvent::InitMouseEvent(aType, aCanBubble, aCancelable, aView, aDetail,
                             aScreenX, aScreenY, aClientX, aClientY, aButton,
                             aRelatedTarget, aModifiersList);

  WidgetWheelEvent* wheelEvent = mEvent->AsWheelEvent();
  wheelEvent->deltaX = aDeltaX;
  wheelEvent->deltaY = aDeltaY;
  wheelEvent->deltaZ = aDeltaZ;
  wheelEvent->deltaMode = aDeltaMode;
}

double
WheelEvent::DeltaX()
{
  if (!mAppUnitsPerDevPixel) {
    return mEvent->AsWheelEvent()->deltaX;
  }
  return mEvent->AsWheelEvent()->deltaX *
    mAppUnitsPerDevPixel / nsPresContext::AppUnitsPerCSSPixel();
}

double
WheelEvent::DeltaY()
{
  if (!mAppUnitsPerDevPixel) {
    return mEvent->AsWheelEvent()->deltaY;
  }
  return mEvent->AsWheelEvent()->deltaY *
    mAppUnitsPerDevPixel / nsPresContext::AppUnitsPerCSSPixel();
}

double
WheelEvent::DeltaZ()
{
  if (!mAppUnitsPerDevPixel) {
    return mEvent->AsWheelEvent()->deltaZ;
  }
  return mEvent->AsWheelEvent()->deltaZ *
    mAppUnitsPerDevPixel / nsPresContext::AppUnitsPerCSSPixel();
}

uint32_t
WheelEvent::DeltaMode()
{
  return mEvent->AsWheelEvent()->deltaMode;
}

already_AddRefed<WheelEvent>
WheelEvent::Constructor(const GlobalObject& aGlobal,
                        const nsAString& aType,
                        const WheelEventInit& aParam,
                        ErrorResult& aRv)
{
  nsCOMPtr<EventTarget> t = do_QueryInterface(aGlobal.GetAsSupports());
  RefPtr<WheelEvent> e = new WheelEvent(t, nullptr, nullptr);
  bool trusted = e->Init(t);
  e->InitWheelEvent(aType, aParam.mBubbles, aParam.mCancelable,
                    aParam.mView, aParam.mDetail,
                    aParam.mScreenX, aParam.mScreenY,
                    aParam.mClientX, aParam.mClientY,
                    aParam.mButton, aParam.mRelatedTarget,
                    EmptyString(), aParam.mDeltaX,
                    aParam.mDeltaY, aParam.mDeltaZ, aParam.mDeltaMode);
  e->InitializeExtraMouseEventDictionaryMembers(aParam);
  e->SetTrusted(trusted);
  return e.forget();
}

} // namespace dom
} // namespace mozilla

using namespace mozilla;
using namespace mozilla::dom;

already_AddRefed<WheelEvent>
NS_NewDOMWheelEvent(EventTarget* aOwner,
                    nsPresContext* aPresContext,
                    WidgetWheelEvent* aEvent)
{
  RefPtr<WheelEvent> it = new WheelEvent(aOwner, aPresContext, aEvent);
  return it.forget();
}
