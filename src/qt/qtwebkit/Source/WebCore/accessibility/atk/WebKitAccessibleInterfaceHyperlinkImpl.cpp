/*
 * Copyright (C) 2010, 2011, 2012 Igalia S.L.
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

#include "config.h"
#include "WebKitAccessibleInterfaceHyperlinkImpl.h"

#if HAVE(ACCESSIBILITY)

#include "WebKitAccessibleHyperlink.h"

using namespace WebCore;

static GQuark hyperlinkObjectQuark = 0;

static AtkHyperlink* webkitAccessibleHyperlinkImplGetHyperlink(AtkHyperlinkImpl* hyperlink)
{
    AtkHyperlink* hyperlinkObject = ATK_HYPERLINK(g_object_get_qdata(G_OBJECT(hyperlink), hyperlinkObjectQuark));
    if (!hyperlinkObject) {
        hyperlinkObject = ATK_HYPERLINK(webkitAccessibleHyperlinkNew(hyperlink));
        g_object_set_qdata(G_OBJECT(hyperlink), hyperlinkObjectQuark, hyperlinkObject);
    }
    return hyperlinkObject;
}

void webkitAccessibleHyperlinkImplInterfaceInit(AtkHyperlinkImplIface* iface)
{
    iface->get_hyperlink = webkitAccessibleHyperlinkImplGetHyperlink;
    hyperlinkObjectQuark = g_quark_from_static_string("webkit-accessible-hyperlink-object");
}

#endif
