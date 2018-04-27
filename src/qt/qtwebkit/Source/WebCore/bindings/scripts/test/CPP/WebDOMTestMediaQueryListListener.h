/*
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 * Copyright (C) 2004, 2005, 2006, 2007, 2008, 2009 Apple Inc. All rights reserved.
 * Copyright (C) 2006 Samuel Weinig <sam.weinig@gmail.com>
 * Copyright (C) Research In Motion Limited 2010. All rights reserved.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#ifndef WebDOMTestMediaQueryListListener_h
#define WebDOMTestMediaQueryListListener_h

#include <WebDOMObject.h>
#include <WebDOMString.h>

namespace WebCore {
class TestMediaQueryListListener;
};

class WebDOMMediaQueryListListener;

class WebDOMTestMediaQueryListListener : public WebDOMObject {
public:
    WebDOMTestMediaQueryListListener();
    explicit WebDOMTestMediaQueryListListener(WebCore::TestMediaQueryListListener*);
    WebDOMTestMediaQueryListListener(const WebDOMTestMediaQueryListListener&);
    WebDOMTestMediaQueryListListener& operator=(const WebDOMTestMediaQueryListListener&);
    virtual ~WebDOMTestMediaQueryListListener();

    void method(const WebDOMMediaQueryListListener& listener);

    WebCore::TestMediaQueryListListener* impl() const;

protected:
    struct WebDOMTestMediaQueryListListenerPrivate;
    WebDOMTestMediaQueryListListenerPrivate* m_impl;
};

WebCore::TestMediaQueryListListener* toWebCore(const WebDOMTestMediaQueryListListener&);
WebDOMTestMediaQueryListListener toWebKit(WebCore::TestMediaQueryListListener*);

#endif
