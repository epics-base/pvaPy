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
