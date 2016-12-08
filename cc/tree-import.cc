#include <stack>

#include "rapidjson/reader.h"

#include "tree-import.hh"
#include "acmacs-base/read-file.hh"
#include "tree.hh"

// ----------------------------------------------------------------------

class Failure : public std::exception { public: using std::exception::exception; };
class Pop : public std::exception { public: using std::exception::exception; };

class HandlerBase
{
 public:
    inline HandlerBase(Tree& aTree) : mTree(aTree), mIgnore(false) {}
    virtual ~HandlerBase();

    inline virtual HandlerBase* StartObject() { std::cerr << "HandlerBase StartObject " << typeid(*this).name() << std::endl; throw Failure(); }
    inline virtual HandlerBase* EndObject() { throw Pop(); }
    inline virtual HandlerBase* StartArray() { throw Failure(); }
    inline virtual HandlerBase* EndArray() { throw Pop(); }
    inline virtual HandlerBase* Double(double d) { std::cerr << "Double: " << d << std::endl; throw Failure(); }
    inline virtual HandlerBase* Int(int i) { std::cerr << "Int: " << i << std::endl; throw Failure(); }
    inline virtual HandlerBase* Uint(unsigned u) { std::cerr << "Uint: " << u << std::endl; throw Failure(); }

    inline virtual HandlerBase* Key(const char* str, rapidjson::SizeType length)
        {
            if ((length == 1 && *str == '_') || (length > 0 && *str == '?')) {
                mIgnore = true;
            }
            else {
                std::cerr << "Key: \"" << std::string(str, length) << '"' << std::endl;
                throw Failure();
            }
            return nullptr;
        }

    inline virtual HandlerBase* String(const char* str, rapidjson::SizeType length)
        {
            if (mIgnore) {
                mIgnore = false;
            }
            else {
                std::cerr << "String: \"" << std::string(str, length) << '"' << std::endl;
                throw Failure();
            }
            return nullptr;
        }

 protected:
    Tree& mTree;
    bool mIgnore;
};

HandlerBase::~HandlerBase()
{
}

// ----------------------------------------------------------------------

class DocRootHandler : public HandlerBase
{
 public:
    inline DocRootHandler(Tree& aTree) : HandlerBase(aTree) {}

      // inline virtual HandlerBase* StartObject() { return new TreeRootHandler(mTree); }
};

// ----------------------------------------------------------------------

class TreeReaderEventHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, TreeReaderEventHandler>
{
 private:
    template <typename... Args> inline bool handler(HandlerBase* (HandlerBase::*aHandler)(Args... args), Args... args)
        {
            try {
                auto new_handler = ((*mHandler.top()).*aHandler)(args...);
                if (new_handler)
                    mHandler.emplace(new_handler);
            }
            catch (Pop&) {
                if (mHandler.empty())
                    return false;
                mHandler.pop();
            }
            catch (Failure&) {
                return false;
            }
            return true;
        }

 public:
    inline TreeReaderEventHandler(Tree& aTree)
        : mTree(aTree)
        {
            mHandler.emplace(new DocRootHandler(mTree));
        }

    inline bool StartObject() { return handler(&HandlerBase::StartObject); }
    inline bool EndObject(rapidjson::SizeType /*memberCount*/) { return handler(&HandlerBase::EndObject); }
    inline bool StartArray() { return handler(&HandlerBase::StartArray); }
    inline bool EndArray(rapidjson::SizeType /*elementCount*/) { return handler(&HandlerBase::EndArray); }
    inline bool Key(const char* str, rapidjson::SizeType length, bool /*copy*/) { return handler(&HandlerBase::Key, str, length); }
    inline bool String(const Ch* str, rapidjson::SizeType length, bool /*copy*/) { return handler(&HandlerBase::String, str, length); }
    inline bool Int(int i) { return handler(&HandlerBase::Int, i); }
    inline bool Uint(unsigned u) { return handler(&HandlerBase::Uint, u); }
    inline bool Double(double d) { return handler(&HandlerBase::Double, d); }

 private:
    Tree& mTree;
    std::stack<std::unique_ptr<HandlerBase>> mHandler;
};


// ----------------------------------------------------------------------

void read_tree(std::string aFilename, Tree& aTree)
{
    std::string buffer;
    if (aFilename == "-")
        buffer = acmacs_base::read_stdin();
    else
        buffer = acmacs_base::read_file(aFilename);
    if (buffer[0] == '{') {
        TreeReaderEventHandler handler{aTree};
        rapidjson::Reader reader;
        rapidjson::StringStream ss(buffer.c_str());
        reader.Parse(ss, handler);
        if (reader.HasParseError())
            throw Error("cannot import tree: data parsing failed at pos " + std::to_string(reader.GetErrorOffset()) + ": " +  GetParseError_En(reader.GetParseErrorCode()) + "\n" + buffer.substr(reader.GetErrorOffset(), 50));
    }
    else
        throw Error("cannot import tree: unrecognized source format");
}

// ----------------------------------------------------------------------
