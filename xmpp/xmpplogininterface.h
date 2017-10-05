/*
 * Copyright (C) 2013 The X-Live Project
 *
 * @author Richard Luo, cdominant7@gmail.com
 * 
 * @date   2014-04-04 17:47:25
 * 
 ****************************************************************** 
 */

#ifndef _XMPPLOGININTERFACE_H
#define _XMPPLOGININTERFACE_H

namespace buzz {

class XmlElement;
class XmppEngineImpl;

class XmppLoginInterface {

  public:

    virtual bool IsDone() const = 0;

    virtual void OutgoingStanza(const XmlElement *element) = 0;

    virtual void IncomingStanza(const XmlElement *element, bool isStart) = 0;

    virtual ~XmppLoginInterface();

};

} // ns: buzz

#endif /* _XMPPLOGININTERFACE_H */



