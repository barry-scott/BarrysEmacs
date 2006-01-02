//
//    minibuf.h
//
class MiniBufferBody
{
public:
    MiniBufferBody();
    ~MiniBufferBody();

    MiniBufferBody( const MiniBufferBody &other );
    MiniBufferBody &operator=( const MiniBufferBody &other );

    // there are two types of body
    //
    void setMessageBody( const EmacsString &body_ );
    void setPromptBody( const EmacsString &body_ );
    void clearMessageBody();

    const EmacsString& getBody() const;
    bool haveBody() const;
    bool isPromptBody() const;

private:
    enum body_type_t { no_body, message_body, prompt_body } body_type;
    EmacsString body;
};
