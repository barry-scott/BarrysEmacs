//
//
//    emobject.h
//
//    Copyright (c) 1995 Barry A. Scott
//
//

//
//
//    EmacsObject is used as a base class for all Emacs objects.
//    It is required to support memory useage diagnostics and
//    the save emacs environment feature.
//
//
class EmacsObject
{
protected:
    EmacsObject();
public:
    virtual ~EmacsObject();

#if DBG_ALLOC_CHECK
    void *operator new(size_t size, const char *fileName, int lineNumber);
#ifdef __has_array_new__
    void *operator new [](size_t size, const char *fileName, int lineNumber);
#endif
#endif

    static void *operator new(size_t size);
    static void operator delete(void *p);

#ifdef __has_array_new__
    static void *operator new [](size_t size);
    static void operator delete [](void *p);
#endif

    static void *operator new( size_t, void *p ) { return p; }
#ifdef _MSVC
    static void operator delete( void *, void * ) { }
#endif

    virtual const char *objectName() const = 0;
#ifdef SAVE_ENVIRONMENT
    virtual void SaveEnvironment() = 0;        // method that supports save environment
#endif
#if DBG_ALLOC_CHECK
    bool isHeapObject() const;
    void objectAllocatedBy( const char *&file, int &line, const EmacsObject *&object ) const;
#endif
private:
    EmacsObject( const EmacsObject &src );        // no implementation
    void operator=(const EmacsObject &src);        // no implementation
    static int NextObjectNumber;
    int ObjectNumber;
};

#if DBG_ALLOC_CHECK
#define EMACS_NEW new(THIS_FILE, __LINE__)
#else
#define EMACS_NEW new
#endif

#ifdef SAVE_ENVIRONMENT
#define EMACS_OBJECT_FUNCTIONS( classname ) \
     virtual const char *objectName() const { return #classname; } \
    virtual void SaveEnvironment();
#else
#define EMACS_OBJECT_FUNCTIONS( classname ) \
     virtual const char *objectName() const { return #classname; }
#endif
