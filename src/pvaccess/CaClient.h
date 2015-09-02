// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef CA_CLIENT_H
#define CA_CLIENT_H

/**
 * Channel access client.
 */
class CaClient 
{
public:
    CaClient();
    virtual ~CaClient();
private:
    // Use this class to make sure client factory is always
    // properly started/stopped.
    class ClientFactoryController {
    public:
        ClientFactoryController();
        ~ClientFactoryController();
    };
    static ClientFactoryController clientFactoryController;
};

#endif
